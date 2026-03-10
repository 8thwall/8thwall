const RequestMicOptions = {
  AUTO: 'auto',
  MANUAL: 'manual',
}

const DEFAULT_SAMPLE_RATE = 44100

const AudioContext = window.AudioContext || window.webkitAudioContext

// If both are provided, connect, but throw an error if it fails
const maybeConnect = (src, dest) => {
  if (!src || !dest) {
    return
  }

  src.connect(dest)
}

// If both are provided, attempt to disconnect, ignoring errors
const tryDisconnect = (src, dest) => {
  if (!src || !dest) {
    return
  }

  try {
    // disconnect() throws an error if the nodes are not connected.
    src.disconnect(dest)
  } catch (err) {
    // Ignore
  }
}

const createAudioMixer = (initialConfig) => {
  let audioContext_       // The context in which all audio processing takes place
  let audioProcessor_     // A processor that allows us export raw audio data, used by wasm-recorder
  let streamDestination_  // An output destination for the recorded node, used by native recording
  let microphoneVolume_   // The node which outputs the microphone audio, if available
  let recordedNode_       // The node which outputs the audio that should be recorded
  let audioStream_        // The user microphone stream, if open
  let micSource_          // The audio source that outputs the raw microphone audio, if open

  // This is necessary in the following case: the user initializes with MANUAL, then requests
  // mic access, then pauses, and then resumes.  We want to make sure on resume that the user
  // still has the mic stream
  let hadMicBeforePause_ = false

  // The configured options for the recording. The relevant properties are:
  //   customAudioContext: the context to use instead of creating a new one
  //   requestMic: controls if the microphone should be opened on start, or immediately
  //   configureAudioOutput: a function that can be called to customize the recorded audio node
  const config_ = {...initialConfig}

  // Connect or clear the current node that is part of the recording
  const setRecordedNode = (node) => {
    if (recordedNode_ === node) {
      return
    }
    // This path is called if the user updates their configureAudioOutput function and calls
    // MediaRecorder.configure() again
    // disconnect the the user audio input from the audio processing
    tryDisconnect(recordedNode_, audioProcessor_)
    tryDisconnect(recordedNode_, streamDestination_)

    // node, audioProcessor_, and/or streamDestination_ may be falsy
    maybeConnect(node, audioProcessor_)
    maybeConnect(node, streamDestination_)

    recordedNode_ = node
  }

  // Called during initialization as well as when the user updates the customAudioContext.
  // Separating this logic from outside the stream initialization which is inside a promise ensures
  // the user can request the audioProcessor inside onAttach
  const setupNonMicAudio = () => {
    audioContext_ = config_.customAudioContext || new AudioContext({
      sampleRate: DEFAULT_SAMPLE_RATE,
    })

    microphoneVolume_ = audioContext_.createGain()
    // Ramp up initial audio gain from 0.01->1.0 over .2s to remove the pop.
    microphoneVolume_.gain.setValueAtTime(0.01, audioContext_.currentTime)
    microphoneVolume_.gain.exponentialRampToValueAtTime(1, audioContext_.currentTime + 0.2)
  }

  // Ensure the audio processor is set up and ready to use
  const ensureAudioProcessor = () => {
    if (audioProcessor_) {
      return
    }

    // This function is deprecated according to Mozilla but the suggested alternative,
    // AudioWorklet API, doesn't work on iOS
    // https://developer.mozilla.org/en-US/docs/Web/API/BaseAudioContext/createScriptProcessor
    audioProcessor_ = audioContext_.createScriptProcessor(1024 * 8, 1, 1)

    // this function is idempotent, so we don't need to disconnect and reconnect it
    audioProcessor_.connect(audioContext_.destination)

    maybeConnect(recordedNode_, audioProcessor_)
  }

  // Ensure the stream destination is set up and ready to use
  const ensureStreamDestination = () => {
    if (streamDestination_) {
      return
    }

    streamDestination_ = audioContext_.createMediaStreamDestination()

    maybeConnect(recordedNode_, streamDestination_)
  }

  // Must be called after the creation of the audioStream.  Connects the audio stream to the audio
  // graph
  const constructMicGraph = () => {
    if (audioStream_) {
      // connect the mic input into the mic volume gain node
      micSource_ = audioContext_.createMediaStreamSource(audioStream_)
      micSource_.connect(microphoneVolume_)
    }
  }

  // Requests microphone permission and sets up the mic audio stream
  const startAudioStream = () => (
    navigator.mediaDevices.getUserMedia({audio: true})
      .then((stream) => {
        audioStream_ = stream
        hadMicBeforePause_ = true
        constructMicGraph()
      })
      .catch((err) => {
        // eslint-disable-next-line no-console
        console.error('[XR] Could not open audio stream')
        throw err
      })
  )

  const stopAudioStream = () => {
    // audioStream may not have been set up if request-mic is manual
    if (!audioStream_) {
      return
    }
    // A MediaStreamTrack has two states in its life-cycle: live and ended.
    audioStream_.getTracks().forEach((track) => {
      track.stop()
    })

    // disconnect and delete the mic input node
    micSource_.disconnect(microphoneVolume_)
    micSource_ = null
    audioStream_ = null
  }

  // This incorporates the user's audio graph into the shared audio context by calling
  // configureAudioOutput
  const setUserAudioGraph = () => {
    // Attach the user's audio graph to the microphone gain node that is returned by
    // configureAudioOutput if it is defined.
    if (config_.configureAudioOutput) {
      // disconnect the gain node from all outputs, including the audioProcessor
      microphoneVolume_.disconnect()

      const processedNode = audioContext_.createGain()

      const returnedNode = config_.configureAudioOutput({
        // may not provide any mic audio if MIC_RECORD_MANUAL
        microphoneInput: microphoneVolume_,
        // Allows the user to add engine audio to the recorder easily
        // Connections can be added directly to this node and they'll be recorded.  Users also have
        // access to the audio context through audioProcessor.context.
        audioProcessor: processedNode,
      })

      if (returnedNode !== processedNode) {
        returnedNode.connect(processedNode)
      }

      // Prevent unwanted connection made by ThreeJS in AudioListener.setFilter (for example)
      // Without this, implementations may inadvertently connect userOutputNode to
      // context.destination, which was a noop before, but would now cause users to hear their own
      // voice. If they really need to make this connection, they can use an intermediary node.
      try {
        processedNode.disconnect(audioContext_.destination)
      } catch (err) {
        // Ignore
      }

      setRecordedNode(processedNode)
    } else {
      setRecordedNode(microphoneVolume_)
    }
  }

  const initialSetup = () => {
    setupNonMicAudio()

    // add in the user's portion of the audio graph if configured
    setUserAudioGraph()

    if (config_.requestMic === RequestMicOptions.AUTO) {
      return startAudioStream()
    } else {
      return Promise.resolve()
    }
  }

  // clears up the audio graph but keeps the stream and configureAudioOutput function untouched
  const clearAudioContext = () => {
    audioContext_ = null
    if (audioProcessor_) {
      audioProcessor_.onaudioprocess = undefined
      audioProcessor_ = null
    }
    if (streamDestination_) {
      streamDestination_.stream.getTracks().forEach(t => t.stop())
      streamDestination_ = null
    }
    microphoneVolume_ = null
    // We're setting the following values to null to clear the state.  This requires the user to
    // redefine their portion of the audio graph and custom audio context
    config_.customAudioContext = null
    recordedNode_ = null
  }

  // lets the user switch in a new audio context after the mp4-recorder has been initialized
  const switchAudioContext = (newContext) => {
    // if the mp4Recorder is already initialized and the user is passing in a new audio context,
    // then everything must be re-initialized.  A node in one context cannot connect to a node
    // in another context.
    clearAudioContext()

    config_.customAudioContext = newContext
    setupNonMicAudio()

    // only set up the mic input if the audio stream has been set up
    constructMicGraph()
    setUserAudioGraph()
  }

  const configure = (config) => {
    if (config.requestMic !== undefined) {
      config_.requestMic = config.requestMic
    }

    const updatingConfigure = config.configureAudioOutput !== undefined

    if (updatingConfigure) {
      config_.configureAudioOutput = config.configureAudioOutput
    }

    // _switchAudioContext calls _setUserAudioGraph so this prevents it from being called twice
    if (config.audioContext !== undefined) {
      switchAudioContext(config.audioContext)
    } else if (updatingConfigure) {
      setUserAudioGraph()
    }
  }

  const getSampleRate = () => audioContext_.sampleRate || DEFAULT_SAMPLE_RATE

  // Register a function that is called periodically with the audio data received by the processor
  const setAudioHandler = (handler) => {
    // Only initialize audio processor when needed for the wasm recorder
    ensureAudioProcessor()
    audioProcessor_.onaudioprocess = handler
  }

  // Get access to a media track that can be used as input to the native MediaRecorder
  const getAudioTrack = () => {
    // Only initialize output stream when needed for native recording
    ensureStreamDestination()
    return streamDestination_.stream.getAudioTracks()[0]
  }

  // Right before the recording starts, we need to ensure the audio context is in an active state.
  // If the browser would limit the page's ability to play audio (even if no audio is on the page),
  // the audio context may not be in an active state, causing the recording to fail. Therefore,
  // calling resume here (after a user gesture on the page) ensures the audio is ready to
  // be recorded.
  // If an experience programmatically starts the recording without a user gesture, browsers may
  // still block the context from becoming active.
  const onRecordStart = () => {
    audioContext_.resume()
  }

  const pause = () => {
    stopAudioStream()
  }

  const resume = () => {
    // requestMic may be MANUAL but the user has already requested the mic manually, in which case
    // we still want to request the mic.
    if (hadMicBeforePause_ || config_.requestMic === RequestMicOptions.AUTO) {
      startAudioStream()
    }
  }

  const cleanUp = () => {
    stopAudioStream()
    clearAudioContext()
  }

  return {
    initialSetup,
    configure,
    startAudioStream,
    setAudioHandler,
    getAudioTrack,
    getSampleRate,
    onRecordStart,
    pause,
    resume,
    cleanUp,
  }
}

export {
  RequestMicOptions,
  createAudioMixer,
}
