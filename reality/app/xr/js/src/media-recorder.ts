import {initWorker} from './media/worker'
import {Mp4Recorder, RequestMicOptions} from './mp4-recorder'
import {XrPermissionsFactory} from './permissions'
import {deviceSupportsDirectRecorder} from './media/direct-recorder'
import {singleton} from './factory'

const createImageLoader = (onChange) => {
  let currentSrc = null

  const load = (url) => {
    if (url === currentSrc) {
      return
    }

    currentSrc = url

    if (!url) {
      onChange(null)
      return
    }

    const img = document.createElement('img')

    img.onload = () => {
      onChange(img)
    }

    img.onerror = () => {
      console.error(`[XR] Failed to load image from ${url}`)
      onChange(null)
    }

    img.setAttribute('crossorigin', 'anonymous')
    img.setAttribute('src', url)
  }

  return {
    load,
  }
}

const MediaRecorderFactory = singleton(() => {
  // Set default state variables.
  let recording = false
  let mp4Recorder = null
  let currentRun = null
  let coverImage = null
  let footerImage = null
  let enableEndCard = false
  let shortLink = null
  let foregroundCanvas = null
  let maxDurationMs = 15000
  let endCardCallToAction = 'Try it at:'
  let maxDimension = 1280

  let shortLinkConfigured = false
  let coverImageConfigured = false

  let configureAudioOutput = null
  let customAudioContext = null

  let requestMic = RequestMicOptions.AUTO

  const coverImageLoader = createImageLoader((image) => {
    coverImage = image
    if (mp4Recorder) {
      mp4Recorder.coverImage = image
    }
  })

  const footerImageLoader = createImageLoader((image) => {
    footerImage = image
    if (mp4Recorder) {
      mp4Recorder.footerImage = image
    }
  })

  const _initRecorder = (canvas) => {
    if (mp4Recorder) {
      return
    }

    mp4Recorder = new Mp4Recorder(
      canvas,
      // TODO: (nathan): combine media-recorder.js and mp4-recorder.js.
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
    )

    mp4Recorder.onRecordingStart = () => {
      recording = true
      if (currentRun && currentRun.onStart) {
        currentRun.onStart()
      }
    }

    mp4Recorder.onRecordingStop = () => {
      recording = false
      if (currentRun && currentRun.onStop) {
        currentRun.onStop()
      }
    }

    mp4Recorder.onError = (err) => {
      recording = false
      if (currentRun && currentRun.onError) {
        currentRun.onError(err)
      }
      currentRun = null
    }

    mp4Recorder.onVideoComplete = (result) => {
      if (currentRun && currentRun.onVideoReady) {
        currentRun.onVideoReady(result)
      }
      currentRun = null
    }

    mp4Recorder.onFinalizeProgress = (result) => {
      if (currentRun && currentRun.onFinalizeProgress) {
        currentRun.onFinalizeProgress(result)
      }
    }

    mp4Recorder.onPreviewReady = (result) => {
      if (currentRun && currentRun.onPreviewReady) {
        currentRun.onPreviewReady(result)
      }
    }
  }

  const pipelineModule = () => {
    // The direct recorder doesn't use the worker so we don't need to pre-load it.
    if (!deviceSupportsDirectRecorder()) {
      initWorker()
    }

    return {
      name: 'mediarecorder',
      onAttach: ({canvas}) => {
        _initRecorder(canvas)
      },
      onAppResourcesLoaded: (resources) => {
        if (resources.shortLink !== undefined && !shortLinkConfigured) {
          // eslint-disable-next-line prefer-destructuring
          shortLink = resources.shortLink
          if (mp4Recorder) {
            mp4Recorder.configure({shortLink})
          }
        }

        if (resources.coverImageUrl !== undefined && !coverImageConfigured) {
          coverImageLoader.load(resources.coverImageUrl)
        }
      },
      onProcessGpu: () => {
        if (recording) {
          mp4Recorder.captureFrame()
        }
      },
      // A MediaStreamTrack has two states in its life-cycle: live and ended.  So we'll have to kill
      // the tracks on stopAudioStream and create new ones in resumeAudioStream
      onPaused: () => {
        if (mp4Recorder) {
          mp4Recorder.stopAudioStream()
        }
      },
      onResume: () => {
        if (mp4Recorder) {
          mp4Recorder.resumeAudioStream()
        }
      },
      onDetach: () => {
        mp4Recorder.cleanUp()
        mp4Recorder = null

        // set member variables to null so that if MediaRecorder is attached again, it will be
        // created with default parameters.
        // TODO: (nathan) - This should not be necessary once we combine media-recorder.js
        // and mp4-recorder.js
        recording = false
        mp4Recorder = null
        currentRun = null
        coverImageLoader.load(null)
        footerImageLoader.load(null)
        enableEndCard = false
        shortLink = null
        foregroundCanvas = null
        maxDurationMs = 15000
        endCardCallToAction = 'Try it at:'
        maxDimension = 1280
        shortLinkConfigured = false
        coverImageConfigured = false
        configureAudioOutput = null
        customAudioContext = null
        requestMic = RequestMicOptions.AUTO

        if (currentRun) {
          const error = new Error('[XR] Detaching media recorder while recording')
          if (currentRun.onError) {
            currentRun.onError(error)
          } else {
            console.error(error)
          }
          currentRun = null
        }
      },
      requiredPermissions: () => {
        if (requestMic === RequestMicOptions.AUTO) {
          return [XrPermissionsFactory().permissions().MICROPHONE]
        }
        return []
      },
    }
  }

  // Input {
  //   onError: Callback when there is an error.
  //   onProcessFrame: Callback for adding an overlay to the video.
  //   onStart: Callback when recording has started.
  //   onStop: Callback when recording has stopped.
  //   onVideoReady: Callback when recording has completed and video is ready.
  // }
  const recordVideo = (run) => {
    if (!mp4Recorder) {
      const err = new Error('[XR] Cannot start recording before pipeline has been initialized.')
      if (run.onError) {
        run.onError(err)
        return
      } else {
        throw err
      }
    }

    if (currentRun) {
      const err = new Error('[XR] Recording already in progress')
      if (run.onError) {
        run.onError(err)
        return
      } else {
        throw err
      }
    }

    currentRun = {
      onStart: run.onStart,
      onStop: run.onStop,
      onVideoReady: run.onVideoReady,
      onError: run.onError,
      onPreviewReady: run.onPreviewReady,
      onFinalizeProgress: run.onFinalizeProgress,
    }

    mp4Recorder.onProcessFrame = run.onProcessFrame
    mp4Recorder.start()
  }

  const stopRecording = () => {
    if (recording) {
      mp4Recorder.stop()
    }
  }

  // Enables recording of audio (if not enabled automatically), requesting permissions if needed.
  // Returns a promise that lets the client know when the stream is ready.  If you begin recording
  // before the audio stream is ready, then you may miss the user's microphone output at the
  // beginning of the recording.
  const requestMicrophone = () => {
    if (!mp4Recorder) {
      const err = new Error('requestMicrophone called before module was attached. To ' +
                            'request microphone permission automatically at startup use ' +
                            'AUTO.')
      return Promise.reject(err)
    }

    return mp4Recorder.startAudioStream()
  }

  // Configures the MediaRecorder output
  //
  // Inputs:
  // {
  //   coverImageUrl         [Optional] Image source for cover image.
  //   enableEndCard         [Optional] If true, enable end card.
  //   endCardCallToAction   [Optional] Sets the text string for call to action.
  //   footerImageUrl        [Optional] Image source for cover image.
  //   foregroundCanvas      [Optional] The canvas to use as a foreground in the recorded video.
  //   maxDurationMs         [Optional] Maximum duration of video, in milliseconds. Default is 15000
  //   maxDimension          [Optional] Max dimension of the captured recording, in pixels.  Default
  //                                    is 1280.
  //   shortLink             [Optional] Sets the text string for shortlink.  The default is 8th.io.
  //   configureAudioOutput  [Optional] User provided function that will receive the microphoneInput
  //                                    and audioProcessor audio nodes for complete control
  //                                    of the recording's audio.  The nodes attached to the audio
  //                                    processor node will be part of the recording's audio. It is
  //                                    required to return the end node of the user's audio graph.
  //   audioContext          [Optional] User provided AudioContext instance.  Other engines like
  //                                    THREE.js and BABYLON.js have their own internal audio
  //                                    instance.  In order for the recordings to contains sounds
  //                                    defined in those engines, you'll want to provide their
  //                                    AudioContext instance.
  //   requestMic            [Optional] Determines when the audio permissions are requested. The
  //                                    options are provided in XR8.MediaRecorder.RequestMicOptions.
  //                                    AUTO: requests audio in onAttach()
  //                                    MANUAL: user's microphone is not requested
  //                                      in onAttach().  Any other audio added to the app is still
  //                                      recorded if added to the AudioContext and connected
  //                                      to the audioProcessor provided to the user's
  //                                      configureAudioOutput function.  You can request the
  //                                      microphone with XR8.MediaRecorder.requestMicrophone().
  // }
  const configure = (config) => {
    if (config.enableEndCard !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      enableEndCard = config.enableEndCard
    }

    if (config.shortLink !== undefined) {
      shortLinkConfigured = true
      // eslint-disable-next-line prefer-destructuring
      shortLink = config.shortLink
    }

    if (config.coverImageUrl !== undefined) {
      coverImageConfigured = true
      coverImageLoader.load(config.coverImageUrl)
    }

    if (config.footerImageUrl !== undefined) {
      footerImageLoader.load(config.footerImageUrl)
    }

    if (config.foregroundCanvas !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      foregroundCanvas = config.foregroundCanvas
    }

    if (config.maxDurationMs !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      maxDurationMs = config.maxDurationMs
    }

    if (config.endCardCallToAction !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      endCardCallToAction = config.endCardCallToAction
    }

    if (config.maxDimension !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      maxDimension = config.maxDimension
    }

    if (config.requestMic !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      requestMic = config.requestMic
    }

    if (config.audioContext !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      customAudioContext = config.audioContext
    }

    if (config.configureAudioOutput !== undefined) {
      // eslint-disable-next-line prefer-destructuring
      configureAudioOutput = config.configureAudioOutput
    }

    if (mp4Recorder) {
      mp4Recorder.configure(config)
    }
  }

  return {
    pipelineModule,
    recordVideo,
    stopRecording,
    configure,
    requestMicrophone,
    RequestMicOptions: Object.freeze(RequestMicOptions),
  }
})

export {
  MediaRecorderFactory,
}
