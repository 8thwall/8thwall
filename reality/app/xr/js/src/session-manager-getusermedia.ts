import type {XrccModule} from 'reality/app/xr/js/src/types/xrcc'

import {enableDoubleRAF, disableDoubleRAF} from './animation-frame-helper'
import {XrConfigFactory} from './config'
import {getCameraConstraints, hasResolutionIndex} from './constraints-helper'
import {checkGLError} from './gl-renderer'
import {XrDeviceFactory} from './js-device'
import {XrPermissionsFactory} from './permissions'
import {
  requestPermission, showPermissionPrompt, hidePermissionPrompt, PermissionState,
} from './permissions-helper'
import type {
  CameraConfig, CameraStatusChangeDetailsFailReason, PermissionsManager, RunConfig,
  RunOnCameraStatusChangeCb,
} from './types/pipeline'
import type {CameraDirection, XrPermission} from './types/common'

const SessionManagerGetUserMedia = (xrccPromise: Promise<XrccModule>) => {
  let cameraLoadId_ = 0  // Monotonic counter to handle cancelling camera feed initialization.
  let hasMicrophoneAccess_ = false
  let requiredPermissionsManager_: PermissionsManager
  let onError_ = null
  let video_: HTMLVideoElement
  let drawCanvas_: HTMLCanvasElement
  let runOnCameraStatusChange_: RunOnCameraStatusChangeCb | null = null
  let config_: RunConfig
  let paused_ = false
  let videoTime_ = -1
  let xrcc_: XrccModule
  const xrccPromise_ = xrccPromise.then((xrcc) => { xrcc_ = xrcc })

  const verboseLog = (...args) => {
    if (config_ && config_.verbose) {
      console.log(...args)  // eslint-disable-line no-console
    }
  }

  const verboseError = (...args) => {
    if (config_ && config_.verbose) {
      console.error(...args)  // eslint-disable-line no-console
    }
  }

  const initialize = ({
    drawCanvas, config, runOnCameraStatusChange, onError,
  }) => {
    const device = XrDeviceFactory().deviceEstimate()
    const browser = device.browser.name
    if (browser === 'Oculus Browser') {
      // We have to return a promise here since this is not an async function.
      return Promise.reject(new Error('getUserMedia is not supported with Oculus'))
    }

    if (!drawCanvas) {
      verboseError('[XR] drawCanvas is required')
      throw new Error('drawCanvas is required')
    }
    drawCanvas_ = drawCanvas
    runOnCameraStatusChange_ = runOnCameraStatusChange
    onError_ = onError
    config_ = config
    return Promise.resolve()
  }

  const getCameraConfig = (): CameraConfig => {
    const cameraConfig = (config_ && config_.cameraConfig) || {} as CameraConfig
    if (!cameraConfig.direction) {
      cameraConfig.direction = XrConfigFactory().camera().BACK as CameraDirection
    }
    return cameraConfig
  }

  const getDimensionString = (constraints) => {
    const vc = constraints.video
    if (vc.width.min) {
      return `${vc.width.min}x${vc.height && vc.height.min} (fuzzy)`
    } else {
      return `${vc.width.exact}x${vc.height && vc.height.exact}`
    }
  }

  const doiOSWeChatWorkaround = (video: HTMLVideoElement) => {
    const {os, browser} = XrDeviceFactory().deviceEstimate()
    if (os === 'iOS' && browser.name === 'WeChat') {
      // play() can throw 'if for any reason playback cannot be started'. Haven't seen in practice,
      // but catch just in case.
      try {
        video.play()
      // eslint-disable-next-line no-empty
      } catch (ignore) { }
    }
  }

  const shouldDoSafariWorkarounds = () => {
    const {os, browser} = XrDeviceFactory().deviceEstimate()
    return os === 'iOS' || browser.name === 'Safari'
  }

  const setVideoForSafari = () => {
    video_.setAttribute('playsinline', 'true')
    video_.style.display = 'block'
    video_.style.top = '0px'
    video_.style.left = '0px'
    video_.style.width = '2px'
    video_.style.height = '2px'
    video_.style.position = 'fixed'
    video_.style.opacity = '0.01'
    video_.style.pointerEvents = 'none'
    video_.style.zIndex = '999'

    // Hack around WebKit bug: https://bugs.chromium.org/p/chromium/issues/detail?id=675795
    // WebKit before iOS 14 might be too eager about queuing rAF so double buffer them just in case.
    // Make a copy of the rAF function so we can call it later without infinite recursion.
    const {os, osVersion} = XrDeviceFactory().deviceEstimate()
    if (os === 'iOS' && Number.parseInt(osVersion.split('.')[0], 10) < 14) {
      enableDoubleRAF()
    }
  }

  // TODO(dat): Move camera permission request to obtainPermissions?
  type PermissionRequest = {
    status: PermissionState
    permission: XrPermission
  }
  const requestPermissions = (requiredPermissionsManager: PermissionsManager) => {
    requiredPermissionsManager_ = requiredPermissionsManager
    const permissionRequests =
      Array.from(requiredPermissionsManager_.get()).map(permission => (
        requestPermission(permission)?.then((status): PermissionRequest => ({status, permission}))
      )).filter(e => !!e)

    return Promise.all(permissionRequests)
      .then((permissionResults) => {
        const notGrantedPermission = permissionResults.find(result => result.status !== 'granted')
        return notGrantedPermission || {status: 'granted'}
      })
  }

  const obtainPermissions =
    (requiredPermissionsManager, hasUserGesture) => requestPermissions(requiredPermissionsManager)
      .then((result) => {
        if (result.status === 'granted') {
          return Promise.resolve()
        } else if (result.status === 'retry' && !hasUserGesture) {
          // Calling showPermissionPrompt guarantees the next attempt to obtain permissions is
          // started from a user gesture handler which ensures permissions will be requested with a
          // user prompt.
          return showPermissionPrompt()
            .then(() => obtainPermissions(requiredPermissionsManager, true))
        } else {
          throw ({type: 'permission', ...result})  // eslint-disable-line no-throw-literal
        }
      })

  const closeStream = (stream) => {
    const videoTracks = stream.getVideoTracks()
    // Close all video tracks for the stream.
    if (videoTracks && videoTracks.length > 0) {
      videoTracks.forEach((videoTrack) => {
        videoTrack.stop()
      })
    }
  }

  const closeCamera = () => {
    const videoSrcObject = video_?.srcObject as MediaStream
    if (videoSrcObject?.active) {
      closeStream(videoSrcObject)
    }
  }

  const pickBackCamera = (videoDevices) => {
    const backCamera = videoDevices.find(d => d.label === 'Back Camera')

    return (backCamera || videoDevices[1])?.deviceId
  }

  // Starts a getUserMedia stream with the given cameraConfig
  // Handles getting the correct camera constraints and retrying with each resolution level
  const startCameraStream = (loadId, cameraConfig, tryIndex = 0) => (
    new Promise<MediaStream>((resolve, reject) => {
      getCameraConstraints(tryIndex, cameraConfig).then((constraints) => {
        if (loadId !== cameraLoadId_) {
          return
        }
        navigator.mediaDevices.getUserMedia(constraints).then((stream) => {
          if (loadId !== cameraLoadId_) {
            closeStream(stream)
            return
          }

          // Workaround for an iOS 16.4 bug where the ultra wide camera was selected. See:
          // - https://bugs.webkit.org/show_bug.cgi?id=253186
          const device = XrDeviceFactory().deviceEstimate()
          const facingMode = constraints.video.facingMode as string | string[]
          if (device.os === 'iOS' &&
              parseFloat(device.osVersion) >= 16.4 &&
              !facingMode?.includes('user')) {
            // Close the stream we just opened as we will re-open it.
            closeStream(stream)
            // Request devices so that we can determine which to re-open with.
            navigator.mediaDevices.enumerateDevices().then((devices) => {
              // Filter out audio devices.
              const videoDevices = devices.filter(d => d.kind !== 'audioinput')

              // Since newer iPhones have different ordering for back cameras, making
              // videoDevices[1] the triple back camera, this will ensure we only use back camera
              const backCameraId = pickBackCamera(videoDevices)

              // Specify the deviceId of the first index 'videoinput'.
              constraints.video.deviceId = backCameraId
              // Re-open with the correct deviceId.
              navigator.mediaDevices.getUserMedia(constraints).then((iosStream) => {
                verboseLog(`[XR] Opened iOS >= 16.4 camera stream at ${
                  getDimensionString(constraints)}`)
                resolve(iosStream)
              }).catch((err) => {
                verboseLog(`[XR] Couldn't open iOS >= 16.4 camera stream at ${
                  getDimensionString(constraints)}`)
                reject(err)
              })
              // Cleanup so that if we use the front camera it doesn't have deviceId set.
              delete constraints.video.deviceId
            }).catch((err) => {
              verboseLog('[XR] Failed to enumerate devices', err?.name, err?.message)
            })
            return
          }

          verboseLog(`[XR] Opened camera stream at ${getDimensionString(constraints)}`)
          resolve(stream)
        }).catch((err) => {
          if (loadId !== cameraLoadId_) {
            return
          }
          verboseLog(`[XR] Couldn't open camera stream at ${getDimensionString(constraints)}`)

          const nextIndex = tryIndex + 1
          if (!hasResolutionIndex(nextIndex)) {
            reject(err)
          } else {
            startCameraStream(loadId, cameraConfig, nextIndex).then(resolve, reject)
          }
        })
      }).catch((err) => {
        if (loadId !== cameraLoadId_) {
          return
        }
        reject(err)
      })
    })
  )

  const resolveWaitForFrame = (loadId, resolve) => {
    if (loadId !== cameraLoadId_) {
      // No longer waiting for this initialization process to complete, cancel.
      return resolve(true)  // cancelled.
    }
    // Keep waiting until video has a width and time.
    if (video_.videoWidth <= 0 || !video_.currentTime) {
      requestAnimationFrame(() => resolveWaitForFrame(loadId, resolve))
      return null
    }

    if (window._c8) {
      window._c8.isAllZeroes = 0
      window._c8.wasEverNonZero = 0
    }

    return resolve(false)  // not cancelled.
  }

  // While loadId matches cameraLoadId, waitForFrame waits until the video has a valid frame, then
  // calls the callback.
  const waitForFrame = (loadId: number): Promise<boolean> => xrccPromise_.then(
    () => new Promise(resolve => resolveWaitForFrame(loadId, resolve))
  )

  // Resets the stream on the video element.
  const resetStream = (): Promise<boolean> => {
    const loadId = ++cameraLoadId_
    const cameraConfig = getCameraConfig()
    // eslint-disable-next-line @typescript-eslint/no-use-before-define
    runOnCameraStatusChange_!({
      status: 'requesting', cameraConfig, config: config_, rendersOpaque: false,
    })

    return startCameraStream(loadId, cameraConfig).then((stream) => {
      // eslint-disable-next-line @typescript-eslint/no-use-before-define
      runOnCameraStatusChange_!({status: 'hasStream', stream, rendersOpaque: false})
      video_.srcObject = stream
      doiOSWeChatWorkaround(video_)

      return waitForFrame(loadId).then((cancelled) => {
        if (cancelled) {
          return cancelled
        }
        // eslint-disable-next-line @typescript-eslint/no-use-before-define
        runOnCameraStatusChange_!({status: 'hasVideo', video: video_, rendersOpaque: false})

        return cancelled
      })
    }).catch(onError_)
  }

  // For Safari, if the user leaves a page, the page is suspended and stored in Safari's cache.
  // If the user navigates back to the page, it does not reload but rather resumes with the same
  // state.  The only difference is that the streams are ended, so we need to reset the video
  // stream.  This method is called on a 'pageshow' event, which is Safari-specific notification
  // https://webkit.org/blog/516/webkit-page-cache-ii-the-unload-event/
  const onSafariPageShow = () => {
    if (paused_) {
      // If the experience is paused and the user navigates away and back, do not reset the camera
      // media stream.  It will be reset once the user calls resume().
      return
    }
    resetStream()
  }

  const stop = () => {
    // If stop() is called twice, video will be null the second time and this is a no-op.
    closeCamera()

    // Enable/disable double raf are both idempotent; calling multiple times has no extra effect.
    disableDoubleRAF()

    // Increment cameraLoadId to ensure camera loads are cancelled
    ++cameraLoadId_

    // If stop() is called twice, video will be null so it won't be re-removed.
    if (video_ && video_.parentNode) {
      video_.parentNode.removeChild(video_)
    }

    video_ = null

    if (shouldDoSafariWorkarounds()) {
      window.removeEventListener('pageshow', onSafariPageShow)
    }

    hidePermissionPrompt()
  }

  const handleFirstFrame = (cancelled) => {
    if (cancelled) {
      return cancelled
    }
    if (shouldDoSafariWorkarounds()) {
      xrcc_._c8EmAsm_setCheckAllZeroes(true)
    }

    runOnCameraStatusChange_!({status: 'hasVideo', video: video_, rendersOpaque: false})

    verboseLog(`[XR] Received first video frame at ${video_.videoWidth}x${video_.videoHeight}`)

    if (shouldDoSafariWorkarounds()) {
      window.addEventListener('pageshow', onSafariPageShow)
    }

    return cancelled
  }

  const startSession = () => {
    // NOTE(dat): We used to throw if the device is incompatible. This was a legacy design.
    // For projects that uses xrextras, the loading and almost-there modules already have this info
    // and can provide a better assistance to the user than us just throwing an error.
    // We switched the throw to a cancellation so camera-loop can skip this session if wanted.
    if (!XrDeviceFactory().isDeviceBrowserCompatible(config_)) {
      return Promise.resolve(true)
    }

    const loadId = ++cameraLoadId_

    const cameraConfig = getCameraConfig()

    runOnCameraStatusChange_!({
      status: 'requesting', cameraConfig, config: config_, rendersOpaque: false,
    })

    let microphonePromise = null

    const microphoneValue = XrPermissionsFactory().permissions().MICROPHONE
    const needsMicrophone = requiredPermissionsManager_.get().has(microphoneValue)

    if (needsMicrophone && !hasMicrophoneAccess_) {
      microphonePromise = navigator.mediaDevices.getUserMedia({video: true, audio: true})
        .then((stream) => {
          stream.getTracks().forEach(t => t.stop())
          hasMicrophoneAccess_ = true
        })
        .catch((err) => {
          hasMicrophoneAccess_ = false
          let reason: CameraStatusChangeDetailsFailReason = 'UNSPECIFIED'
          if (err.name === 'NotAllowedError') {
            reason = 'DENY_MICROPHONE'
          } else if (err.name === 'NotFoundError') {
            reason = 'NO_MICROPHONE'
          }
          verboseError('[XR] getUserMedia failed. Reason =', reason)
          runOnCameraStatusChange_!({status: 'failed', reason, config: config_})
          throw new Error('Cannot get microphone access')
        })
    }

    return Promise.resolve(microphonePromise)
      .then(() => startCameraStream(loadId, cameraConfig))
      .then((stream) => {
        runOnCameraStatusChange_!({status: 'hasStream', stream, rendersOpaque: false})
        video_ = document.createElement('video')
        drawCanvas_.parentNode.append(video_)
        video_.setAttribute('width', '1')
        video_.setAttribute('height', '1')
        video_.setAttribute('autoplay', 'true')
        video_.setAttribute('muted', 'true')
        video_.style.display = 'none'

        // Make sure to hack this only on Mobile Safari - this hangs Chrome on iOS
        if (shouldDoSafariWorkarounds()) {
          setVideoForSafari()  // hack mobile safari
        }

        video_.srcObject = stream
        doiOSWeChatWorkaround(video_)
        // TODO(alvin): getCapabilities() does not work on Firefox for Android. Find workaround
        //    when we decide to try to focus the camera.
        // if (stream.getVideoTracks()
        //     .filter(t => t.getCapabilities().focusDistance).length) {
        //   console.log('TODO(scott) We can focus the camera!')
        // }

        // Start update loop
        return waitForFrame(loadId)
      })
      .then(cancelled => handleFirstFrame(cancelled))
      .catch((e) => {
        if (!needsMicrophone || hasMicrophoneAccess_ !== false) {
          // we didn't fail microphone access and get here on the rethrow
          // the failure comes from the camera request
          let reason: CameraStatusChangeDetailsFailReason = 'UNSPECIFIED'
          if (e.name === 'NotAllowedError') {
            reason = 'DENY_CAMERA'
          } else if (e.name === 'NotFoundError') {
            reason = 'NO_CAMERA'
          }
          runOnCameraStatusChange_!({status: 'failed', reason, config: config_})
          verboseError('[XR] getUserMedia failed', e)
        }
        // resolve to true here to indicate to `waitForFrame` that it's cancelled and should stop
        // the rest of the setup
        return Promise.resolve(true)
      })
  }

  const pause = () => {
    // Increment cameraLoadId to ensure camera loads are cancelled
    ++cameraLoadId_
    paused_ = true
    closeCamera()
  }

  const resume = () => {
    // If for some reason video has a playing stream, continue without starting a new stream
    const videoSrcObject = video_.srcObject as MediaStream
    if (videoSrcObject) {
      const tracks = videoSrcObject.getVideoTracks()
      if (tracks.some(t => t.readyState === 'live')) {
        paused_ = false
        return Promise.resolve(false)
      }
    }

    // Once we have reset the stream, mark the resume process as complete
    return resetStream().then((cancelled: boolean) => {
      if (cancelled) {
        return cancelled
      }
      paused_ = false
      return cancelled
    })
  }

  const hasNewFrame = () => {
    if (!video_) {
      return {repeatFrame: true, videoSize: {width: 1, height: 1}}
    }
    const pausedOrUnstarted = video_.paused || video_.videoWidth <= 0 || video_.videoHeight <= 0
    return {
      repeatFrame: pausedOrUnstarted || video_.currentTime === videoTime_,
      videoSize: {
        width: video_.videoWidth,
        height: video_.videoHeight,
      },
    }
  }

  const frameStartResult = ({repeatFrame, drawCtx, computeCtx, drawTexture, computeTexture}) => {
    // TODO(yuyan): figure out why this is called when video_ is null
    if (!video_) {
      return {
        textureWidth: 1,
        textureHeight: 1,
        videoTime: videoTime_,
      }
    }

    if (!repeatFrame) {
      // The video can be paused or have 0 dimensions if the user does something like navigate away
      // from the page and then come back on Safari, which would resume the page instead of
      // reloading. In that case, just return the previous frame by keeping repeatFrame true.
      computeCtx.bindTexture(computeCtx.TEXTURE_2D, computeTexture)
      videoTime_ = video_.currentTime
      computeCtx.texImage2D(
        computeCtx.TEXTURE_2D, 0, computeCtx.RGBA, computeCtx.RGBA, computeCtx.UNSIGNED_BYTE, video_
      )

      // also load the camera feed into the draw context.  The computeTexture has a name, so we get
      // the corresponding drawCtx buffer from the drawTexMap cache.  Then we re-use the drawCtx
      // buffer and populate it with the camera feed frame.
      // Cache current bindings.
      const restoreTex = drawCtx.getParameter(drawCtx.TEXTURE_BINDING_2D)
      const restoreUnpackFlipY = drawCtx.getParameter(drawCtx.UNPACK_FLIP_Y_WEBGL)

      // Bind texture and configure pixelStorei.
      drawCtx.bindTexture(drawCtx.TEXTURE_2D, drawTexture)
      // We only need to change UNPACK_FLIP_Y_WEBGL if its value was 'true' before.
      if (restoreUnpackFlipY) {
        drawCtx.pixelStorei(drawCtx.UNPACK_FLIP_Y_WEBGL, false)
      }

      // Read the texture from the video.
      drawCtx.texImage2D(
        drawCtx.TEXTURE_2D,
        0,
        drawCtx.RGBA,
        drawCtx.RGBA,
        drawCtx.UNSIGNED_BYTE,
        video_
      )

      if (config_ && config_.verbose) {
        checkGLError({GLctx: drawCtx, msg: 'CameraLoop.readCameraToTexture'})
      }

      // Restore bindings.
      drawCtx.bindTexture(drawCtx.TEXTURE_2D, restoreTex)

      // We only need to restore UNPACK_FLIP_Y_WEBGL if its value was 'true' before.
      if (restoreUnpackFlipY) {
        drawCtx.pixelStorei(drawCtx.UNPACK_FLIP_Y_WEBGL, restoreUnpackFlipY)
      }
    }

    return {
      textureWidth: video_.videoWidth,
      textureHeight: video_.videoHeight,
      videoTime: videoTime_,
    }
  }

  const resumeIfAutoPaused = () => {
    if (video_ && video_.paused) {
      video_.play()
    }
  }

  const providesRunLoop = () => false

  const getSessionAttributes = () => ({
    cameraLinkedToViewer: false,
    controlsCamera: false,
    fillsCameraTexture: true,
    providesRunLoop: false,
    supportsHtmlEmbedded: false,
    supportsHtmlOverlay: true,
    usesMediaDevices: true,
    usesWebXr: false,
  })

  return {
    initialize,
    obtainPermissions,
    stop,
    start: startSession,
    pause,
    resume,
    resumeIfAutoPaused,
    hasNewFrame,
    frameStartResult,
    providesRunLoop,
    getSessionAttributes,
  }
}

export {
  SessionManagerGetUserMedia,
}
