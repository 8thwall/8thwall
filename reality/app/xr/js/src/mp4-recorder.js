import {createFrameCompositor} from './media/frame-compositor'
import {createAudioMixer, RequestMicOptions} from './media/audio-mixer'
import {createWasmRecorder} from './media/wasm-recorder'
import {deviceSupportsDirectRecorder, createDirectRecorder} from './media/direct-recorder'
import {
  deviceSupportsTranscodedRecorder, createTranscodedRecorder,
} from './media/transcoded-recorder'

const DIRECT_CONFIG_PARAMS = [
  'enableEndCard',
  'maxDurationMs',
]

const END_CARD_DURATION_MS = 3000
const FADE_DURATION_MS = 1000

class Mp4Recorder {
  constructor(
    sourceCanvas,
    {
      maxDurationMs,
      shortLink,
      enableEndCard,
      coverImage,
      footerImage,
      foregroundCanvas,
      endCardCallToAction,
      maxDimension,
      configureAudioOutput,
      customAudioContext,
      requestMic,
    }
  ) {
    this.startTime = null
    this.maxDurationMs = maxDurationMs
    this.enableEndCard = enableEndCard
    this.coverImage = coverImage
    this.footerImage = footerImage
    this.isFading = false
    this.fadeStart = null

    this.recording = false

    this.compositor = createFrameCompositor(sourceCanvas, {
      endCardCallToAction,
      foregroundCanvas,
      maxDimension,
      shortLink,
    })

    // Callback when recording has started.
    this.onRecordingStart = null

    // Callback when recording has stopped.
    this.onRecordingStop = null

    // Callback when recording has stopped.
    this.onRecordingStop = null

    // Callback when there is an error.
    this.onError = null

    // Callback when video is complete.
    this.onVideoComplete = null

    // Callback to allow users to add overlay.
    this.onProcessFrame = null

    // Callback when a video is ready to be previewed, but not downloaded/shared, pending transcode
    this.onPreviewReady = null

    // Callback to report percentage progress of the finalize step after recording completes.
    this.onFinalizeProgress = null

    this.audioMixer = createAudioMixer({
      requestMic,
      // Over-writable function by the user.
      configureAudioOutput,

      // User provided audio context.  This is necessary for the recordings to contain audio created
      // via engines like THREE.js, BABYLON.js, etc
      customAudioContext,

    })

    window.AudioContext = window.AudioContext || window.webkitAudioContext

    if (deviceSupportsDirectRecorder()) {
      // The direct recorder is preferred as it lets use the native recorder directly and doesn't
      // require a worker.
      this.recorder = createDirectRecorder(this.audioMixer, this.compositor)
    } else if (deviceSupportsTranscodedRecorder()) {
      // Our second choice is the transcoded recorder because it offloads the realtime recording
      // to the browser, transcoding after the recording ends
      this.recorder = createTranscodedRecorder(this.audioMixer, this.compositor)
    } else {
      this.recorder = createWasmRecorder(this.audioMixer, this.compositor)
    }

    this.setupPromise = Promise.all([
      this.recorder.initialSetup && this.recorder.initialSetup(),
      this.audioMixer.initialSetup(),
    ])
  }

  configure(config) {
    DIRECT_CONFIG_PARAMS.forEach((param) => {
      if (config[param] !== undefined) {
        this[param] = config[param]
      }
    })

    this.audioMixer.configure(config)

    this.compositor.configure(config)
  }

  // Start a new recording. Will continue until a call to stop() or the max recording time has
  // elapsed.
  start() {
    this.setupPromise.then(() => {
      this._doStart()
    })
  }

  // Requests microphone permission and sets up the mic audio stream
  startAudioStream() {
    return this.audioMixer.startAudioStream()
  }

  _doStart() {
    const {width, height} = this.compositor.start()

    this.startTime = Date.now()

    // We always want to draw a frame to the canvas before starting the recording to ensure the
    // first frame of the recording contains a non-black video. This affects the native recorder
    // because the first frame is recorded before waiting for the first call to recordFrame.
    this.compositor.drawFrame({
      elapsedTimeMs: 0,
      maxRecordingMs: this._getMaxRecordingMs(),
      endCardOpacity: 0,
      coverImage: this.coverImage,
      footerImage: this.footerImage,
      onProcessFrame: this.onProcessFrame,
    })

    this.audioMixer.onRecordStart()

    this.recorder.start(width, height)

    this.recording = true
    this.onRecordingStart && this.onRecordingStart()
  }

  // Stop recording and finish video.
  stop() {
    if (!this.recording || this.isFading) {
      return
    }

    if (this.enableEndCard) {
      this._startFade()
    } else {
      this._finishRecording()
    }
  }

  _startFade() {
    this.isFading = true
    this.fadeStart = Date.now()
  }

  _encodeFinalEndCard() {
    const elapsedTime = Date.now() - this.startTime
    this.compositor.drawEndCard({coverImage: this.coverImage, footerImage: this.footerImage})
    this.recorder.recordFrame(elapsedTime, END_CARD_DURATION_MS)
  }

  captureFrame() {
    this._doCaptureFrame()
  }

  _doCaptureFrame() {
    const now = Date.now()
    const elapsedTime = now - this.startTime

    if (this.isFading && now - this.fadeStart > FADE_DURATION_MS) {
      this.isFading = false
      this._encodeFinalEndCard()
      this._finishRecording()
      return
    }

    this._encodeVideo()

    if (!this.isFading && elapsedTime >= this._getStopDeadline()) {
      this.stop()
    }
  }

  _getMaxRecordingMs() {
    if (this.enableEndCard) {
      return this.maxDurationMs - END_CARD_DURATION_MS
    } else {
      return this.maxDurationMs
    }
  }

  _getStopDeadline() {
    if (this.enableEndCard) {
      return this.maxDurationMs - END_CARD_DURATION_MS - FADE_DURATION_MS
    } else {
      return this.maxDurationMs
    }
  }

  _finishRecording() {
    this.recording = false
    this.onRecordingStop && this.onRecordingStop()
    // Finish the recording.
    this.recorder.finish({
      onFinalizeProgress: this.onFinalizeProgress,
      onPreviewReady: this.onPreviewReady,
    }).then(this.onVideoComplete, this.onError)
  }

  _encodeVideo() {
    const now = Date.now()
    const elapsedTimeMs = now - this.startTime

    this.compositor.drawFrame({
      elapsedTimeMs,
      maxRecordingMs: this._getMaxRecordingMs(),
      endCardOpacity: this.isFading ? Math.min(1, (now - this.fadeStart) / FADE_DURATION_MS) : 0,
      coverImage: this.coverImage,
      footerImage: this.footerImage,
      onProcessFrame: this.onProcessFrame,
    })

    this.recorder.recordFrame(elapsedTimeMs)
  }

  // This stops the user-mic stream.  This can happen onPause or onDestroy.
  stopAudioStream() {
    this.audioMixer.pause()
  }

  // This resumes the audio stream after stopping it for an onPause event.  The user's custom
  // audio graph is still connected after pausing.
  resumeAudioStream() {
    this.audioMixer.resume()
  }

  cleanUp() {
    this.audioMixer.cleanUp()
  }
}

export {
  Mp4Recorder,
  RequestMicOptions,
}
