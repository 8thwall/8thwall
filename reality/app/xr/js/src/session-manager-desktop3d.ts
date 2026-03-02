import {XrConfigFactory} from './config'
import {createMouseToTouchTranslator} from './mouse-to-touch-translator'

const SessionManagerDesktop3D = () => {
  let emulateTouch_ = false
  let paused_ = false
  let drawCanvas_ = null
  let config_ = null
  let runOnCameraStatusChange_ = null

  const mouseToTouchTranslator = createMouseToTouchTranslator()

  const initialize = ({config, drawCanvas, runOnCameraStatusChange}) => {
    if (config.allowedDevices !== XrConfigFactory().device().ANY) {
      return Promise.reject(new Error('Desktop 3D Session Manager requires allowedDevices ANY'))
    }
    config_ = config
    drawCanvas_ = drawCanvas
    runOnCameraStatusChange_ = runOnCameraStatusChange
    return Promise.resolve()
  }

  const startSession = () => {
    const {sessionConfiguration} = config_
    emulateTouch_ = !(sessionConfiguration && sessionConfiguration.disableDesktopTouchEmulation)
    if (emulateTouch_) {
      mouseToTouchTranslator.attach({drawCanvas: drawCanvas_})
    }

    runOnCameraStatusChange_({status: 'hasDesktop3D', rendersOpaque: true})
  }

  const stop = () => {
    if (emulateTouch_) {
      mouseToTouchTranslator.detach()
    }
    emulateTouch_ = false
  }

  const pause = () => {
    paused_ = true
  }

  const resume = () => {
    paused_ = false
    return Promise.resolve(false)
  }

  const hasNewFrame = () => ({repeatFrame: paused_, videoSize: {width: 1980, height: 1980}})

  const frameStartResult = () => ({
    // Full window canvas caps resolution based on textureWidth and textureHeight. Setting these
    // to fake values is required for a non-1-pixel canvas.
    textureWidth: 1980,
    textureHeight: 1980,
    videoTime: window.performance.now() / 1000,  // unused, but convert to seconds for consistency
  })

  const getSessionAttributes = () => ({
    cameraLinkedToViewer: false,
    controlsCamera: false,
    fillsCameraTexture: false,
    providesRunLoop: false,
    supportsHtmlEmbedded: false,
    supportsHtmlOverlay: true,
    usesMediaDevices: false,
    usesWebXr: false,
  })

  return {
    initialize,
    obtainPermissions: () => Promise.resolve(),
    stop,
    start: startSession,
    pause,
    resume,
    resumeIfAutoPaused: () => {},
    hasNewFrame,
    frameStartResult,
    providesRunLoop: () => false,
    getSessionAttributes,
  }
}

export {
  SessionManagerDesktop3D,
}
