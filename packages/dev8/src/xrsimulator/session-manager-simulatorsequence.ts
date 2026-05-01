import type {
  RunConfig,
  RunOnCameraStatusChangeCb,
} from '@repo/reality/app/xr/js/src/types/pipeline'
import {checkGLError} from '@repo/reality/app/xr/js/src/gl-renderer'

import type {
  LoopBoundaries, SimulatorConfig, SimulatorSessionManager, SequenceControls,
} from './simulator-types'
import {loadFrameData, LoadedFrameData} from './frame-data'
import {
  MotionDataCursor, emitMotionQueueData, emitOrientationData, emitFakeOrientationData,
  getBaseTimeOffset,
  makeMotionDataCursor,
} from './motion-orientation-events'
import {makeGpsDataCursor, GpsDataCursor} from './gps-events'
import {broadcastLoadingSequence} from './broadcast-messages'

declare const XR8: any

const START_POINT_OFFSET = 250000000

const SessionManagerSimulatorSequence = (
  simulatorConfig: SimulatorConfig,
  broadcastSequenceProgress?: (simulatorId: string, progress: number) => void
): {sessionManager: SimulatorSessionManager, sequenceControls: SequenceControls} => {
  const defaultConfig = {
    version: -1,
    simulatorId: simulatorConfig.simulatorId,
    cameraUrl: '',
  }
  const broadcastSequenceProgress_ = broadcastSequenceProgress

  let config_: RunConfig
  let frameData_: LoadedFrameData | null = null

  // Keeping track of where we are in the loop and whether we need to seek back to the start
  let currentTimeInNanos_ = 0
  let loopStartInNanos_ = 0
  let loopEndInNanos_ = 0

  // Used to normalize the time of motion events, relative to a base event time
  // During recording, the event time nanos is from the start of the page load. The media / capnp
  // recorded is after the payload (by 10+ seconds). We need to compute this offset so the time of
  // motion events is relative to these recordings.
  let baseTimeOffset_ = 0

  // For seeking motion data within frameData_
  let motionDataCursor_: MotionDataCursor | null = null

  // For seeking gps data within frameData_
  let gpsDataCursor_: GpsDataCursor | null = null

  let framesUntilRecenter_ = -1

  let progressBroadcastId_: ReturnType<typeof setInterval>
  let scrubbing_ = false

  let simulatorConfig_: SimulatorConfig = defaultConfig

  let runOnCameraStatusChange_: RunOnCameraStatusChangeCb | null = null

  const clearBroadcastInterval = () => {
    clearInterval(progressBroadcastId_)
  }

  const startBroadcastingSequenceProgress = () => {
    clearInterval(progressBroadcastId_)
    if (!broadcastSequenceProgress_) {
      return
    }

    const hasVideoStatusChange = () => {
      if (!frameData_?.videoEl) {
        setTimeout(hasVideoStatusChange, 50)
        return
      }

      runOnCameraStatusChange_!({
        status: 'hasVideo',
        video: frameData_?.videoEl,
        rendersOpaque: false,
      })
    }
    // Wait until have a videoEl and then send hasVideo only once.
    setTimeout(hasVideoStatusChange, 50)

    progressBroadcastId_ = setInterval(() => {
      const videoEl = frameData_?.videoEl
      if (videoEl && !scrubbing_) {
        const progress = videoEl.currentTime / videoEl.duration
        broadcastSequenceProgress_(simulatorConfig_.simulatorId, progress)
      }
    }, 50)
  }

  const scrub = (progress: number) => {
    const videoEl = frameData_?.videoEl

    if (!videoEl) {
      return
    }

    if (!scrubbing_) {
      scrubbing_ = true
      videoEl.pause()
    }

    const {start, end} = simulatorConfig_
    const boundedProgress = Math.min(Math.max(progress, start ?? 0), end ?? 1)

    const newTime = boundedProgress * videoEl.duration
    currentTimeInNanos_ = newTime * 1e9
    videoEl.currentTime = newTime
  }

  const stopScrub = () => {
    if (!scrubbing_) {
      return
    }

    scrubbing_ = false
    if (!simulatorConfig_.isPaused) {
      frameData_?.videoEl?.play()
    }
  }

  // Release all frames in the memory. Clear all config settings and state.
  const clear = () => {
    config_ = {}
    frameData_ = null
    motionDataCursor_ = null
    gpsDataCursor_ = null
    currentTimeInNanos_ = 0
    loopStartInNanos_ = 0
    loopEndInNanos_ = 0
    baseTimeOffset_ = 0
    framesUntilRecenter_ = -1

    clearBroadcastInterval()

    simulatorConfig_ = defaultConfig
  }

  // Given the simulatorConfig_, set the start and end frame number,
  // then move the current frame to start
  const setLoopFrames = () => {
    const {start, end} = simulatorConfig_

    if (frameData_?.videoEl) {
      const durationInNanos = frameData_.videoEl.duration * 1e9
      loopStartInNanos_ = (start ?? 0) * durationInNanos
      loopEndInNanos_ = (end ?? 1) * durationInNanos
      currentTimeInNanos_ = loopStartInNanos_
    }
  }

  const updateLoopEndpoints = (loopBoundaries: LoopBoundaries) => {
    const {start, end} = loopBoundaries

    simulatorConfig_.start = start ?? simulatorConfig_.start
    simulatorConfig_.end = end ?? simulatorConfig_.end

    setLoopFrames()
  }

  const updateSequenceSrc = async (cameraUrl?: string, sequenceUrl?: string): Promise<void> => {
    if (!cameraUrl || cameraUrl === simulatorConfig_.cameraUrl) {
      return
    }
    // Update simulatorConfig_ so that we don't try to load the same sequence again.
    simulatorConfig_.cameraUrl = cameraUrl
    simulatorConfig_.sequenceUrl = sequenceUrl

    if (frameData_?.videoEl) {
      broadcastLoadingSequence(true)
    }
    frameData_ = await loadFrameData(cameraUrl, sequenceUrl)
    broadcastLoadingSequence(false)

    if (frameData_?.motionOrientation?.length) {
      motionDataCursor_ = makeMotionDataCursor(frameData_.motionOrientation)
      baseTimeOffset_ = getBaseTimeOffset(frameData_.motionOrientation[0])
    } else {
      motionDataCursor_ = null
      baseTimeOffset_ = 0
    }
    if (frameData_?.gpsData?.length) {
      gpsDataCursor_ = makeGpsDataCursor(frameData_.gpsData)
    } else {
      gpsDataCursor_ = null
    }

    setLoopFrames()
    framesUntilRecenter_ = 1
  }

  const updateConfig = async (config: SimulatorConfig) => {
    const {version, cameraUrl, sequenceUrl, isPaused, start, end, studioPause} = config

    // Make sure we don't apply a stale/redundant update.
    if (version <= simulatorConfig_.version) {
      return
    }
    simulatorConfig_.version = version

    await updateSequenceSrc(cameraUrl, sequenceUrl)

    if (typeof start !== 'undefined' || typeof end !== 'undefined') {
      updateLoopEndpoints({start, end})
    }
    simulatorConfig_.studioPause = !!studioPause
    simulatorConfig_.isPaused = !!isPaused || simulatorConfig_.studioPause
    if (simulatorConfig_.isPaused) {
      frameData_?.videoEl?.pause()
    } else {
      frameData_?.videoEl?.play()
    }
  }

  const initialize = async ({config, runOnCameraStatusChange}) => {
    config_ = config
    runOnCameraStatusChange_ = runOnCameraStatusChange

    return Promise.resolve()
  }

  const obtainPermissions = () => Promise.resolve()

  const stop = () => {
    clearBroadcastInterval()
  }

  const startSession = async () => {
    if (!simulatorConfig.cameraUrl) {
      runOnCameraStatusChange_!({status: 'failed', reason: 'NO_CAMERA_URL_SRC', config: config_})
      throw new Error('No camera source url provided, cannot run simulator')
    }

    await updateConfig(simulatorConfig)

    startBroadcastingSequenceProgress()

    return false
  }

  // (re)set recording to start of loop
  const seekToLoopEndpoints = () => {
    currentTimeInNanos_ = loopStartInNanos_
    const videoEl = frameData_?.videoEl
    if (videoEl) {
      videoEl.currentTime = loopStartInNanos_ / 1e9
      videoEl.addEventListener('seeked', () => {
        if (!simulatorConfig_.isPaused) {
          videoEl.play()
        }
      }, {once: true})
    }
    motionDataCursor_?.reset()
    gpsDataCursor_?.reset()

    XR8.XrController.recenter()
  }

  const pause = () => {
    clearBroadcastInterval()
  }

  const resume = () => {
    startBroadcastingSequenceProgress()
    return Promise.resolve(false)
  }

  const hasNewFrame = () => {
    const videoEl = frameData_?.videoEl
    const repeatFrame = !videoEl

    return {
      repeatFrame,
      videoSize: {
        width: repeatFrame ? 1 : videoEl.videoWidth,
        height: repeatFrame ? 1 : videoEl.videoHeight,
      },
    }
  }

  const frameStartResult = ({repeatFrame, drawCtx, computeCtx, drawTexture, computeTexture}) => {
    if (repeatFrame) {
      return {
        width: 1,
        height: 1,
        videoTime_: currentTimeInNanos_,
      }
    }

    // We should have already called hasNewFrame so we know we have a frame
    const videoEl = frameData_!.videoEl!
    currentTimeInNanos_ = videoEl.currentTime * 1e9

    // Note(Dale): videoEl.currentTime rounds our current time so we add an offset
    if (
      currentTimeInNanos_ >= loopEndInNanos_ ||
      currentTimeInNanos_ + START_POINT_OFFSET < loopStartInNanos_) {
      if (!scrubbing_) {
        seekToLoopEndpoints()
      }
    }

    if (motionDataCursor_) {
      const foundData = motionDataCursor_.seek(currentTimeInNanos_)
      if (foundData) {
        const {orientation, motionDataList, timeNanos} = foundData
        emitOrientationData(orientation, timeNanos)
        emitMotionQueueData(motionDataList, baseTimeOffset_)
      }
    } else {
      emitFakeOrientationData(currentTimeInNanos_)
    }

    // The video can be paused or have 0 dimensions if the user does something like navigate away
    // from the page and then come back on Safari, which would resume the page instead of
    // reloading. In that case, just return the previous frame by keeping repeatFrame true.
    computeCtx.bindTexture(computeCtx.TEXTURE_2D, computeTexture)

    computeCtx.texImage2D(
      computeCtx.TEXTURE_2D,
      0,
      computeCtx.RGBA,
      computeCtx.RGBA,
      computeCtx.UNSIGNED_BYTE,
      videoEl
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
      videoEl
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

    if (framesUntilRecenter_ > -1) {
      if (framesUntilRecenter_ === 0) {
        XR8.XrController.recenter()
      }
      framesUntilRecenter_--
    }

    const result = {
      textureWidth: videoEl.videoWidth,
      textureHeight: videoEl.videoHeight,
      videoTime: videoEl.currentTime,
    }

    return result
  }

  const resumeIfAutoPaused = () => {}

  const getSessionAttributes = () => ({
    cameraLinkedToViewer: false,
    controlsCamera: false,
    fillsCameraTexture: true,
    providesRunLoop: false,
    supportsHtmlEmbedded: false,
    supportsHtmlOverlay: true,
    usesMediaDevices: true,
    usesWebXr: false,
    isSimulator: true,
  })

  return {
    sessionManager: {
      initialize,
      obtainPermissions,
      stop,
      start: startSession,
      pause,
      resume,
      resumeIfAutoPaused,
      hasNewFrame,
      frameStartResult,
      getSessionAttributes,
      updateConfig,
      clear,
    },
    sequenceControls: {
      scrub,
      stopScrub,
    },
  }
}

export {
  SessionManagerSimulatorSequence,
}
