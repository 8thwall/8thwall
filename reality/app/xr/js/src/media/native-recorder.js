// Used by the direct recorder and transcoded recorder to record native video
const createNativeRecorder = (audioMixer, compositor, {
  specifyMimeType = true,
  mimeType,
  timeSlice,
}) => {
  let resolveFinish_
  let rejectFinish_
  let recorderSession_
  let recording_ = false
  let durationTimeout_ = null
  let flushAnimation_ = null
  let recordedChunks_

  const handleRecordingData = (dataEvent) => {
    if (dataEvent.data && dataEvent.data.size > 0) {
      recordedChunks_.push(dataEvent.data)
    }
  }

  const stopSession = () => {
    recorderSession_.stop()
    recorderSession_ = null
  }

  const handleRecordingStop = () => {
    if (recordedChunks_.length === 0) {
      rejectFinish_(new Error(
        'No data received. Browsers may block media features until users interact with the page.'
      ))
      return
    }
    const blob = new Blob(recordedChunks_, {type: mimeType})
    resolveFinish_({videoBlob: blob})
    recordedChunks_ = null
  }

  // When using the native recorder, if the canvas isn't getting drawn to periodically, it may not
  // include the last few seconds of the end card. By continually flushing, whenever the recording
  // ends, this ensures all frames are included.
  const flushFrame = () => {
    compositor.flush()
    cancelAnimationFrame(flushAnimation_)
    flushAnimation_ = requestAnimationFrame(flushFrame)
  }

  const handleFrameDurationEnd = () => {
    durationTimeout_ = null
    cancelAnimationFrame(flushAnimation_)
    flushAnimation_ = null
    if (!recording_) {
      stopSession()
    }
  }

  const start = () => {
    const mediaStream = new MediaStream()
    mediaStream.addTrack(compositor.getVideoTrack())
    mediaStream.addTrack(audioMixer.getAudioTrack())

    recordedChunks_ = []

    // Set recording quality to 10Mbps.  This is the iPhone default, where as Android is by default
    // set to 2.5Mbps, which is too low of quality.
    const recorderOptions = {
      videoBitsPerSecond: 10 * 1000 * 1000,
      audioBitsPerSecond: 128 * 1000,
    }
    if (specifyMimeType) {
      recorderOptions.mimeType = mimeType
    }

    recorderSession_ = new MediaRecorder(mediaStream, recorderOptions)
    recorderSession_.ondataavailable = handleRecordingData
    recorderSession_.onstop = handleRecordingStop

    recorderSession_.start(timeSlice)

    recording_ = true
  }

  // While the stream recording is running, it records automatically, so this is usually a noop.
  const recordFrame = (_, frameDurationMs) => {
    if (frameDurationMs) {
      // This is a workaround for not being able to record the end card without waiting for the
      // duration of the end card. In the wasm recorder, we can specify each frame's duration, so
      //  we can encode the 3 seconds of end card instantly. However, the native MediaRecorder
      // requires that if we want the end card to last 3 seconds, we have to wait 3 realtime seconds
      // before stopping the recording.
      clearTimeout(durationTimeout_)
      durationTimeout_ = setTimeout(handleFrameDurationEnd, frameDurationMs)
      flushFrame()
    }
  }

  // This promise will be resolved after the last chunk of data is returned from the recorder
  const finish = () => (
    new Promise((resolve, reject) => {
      resolveFinish_ = resolve
      rejectFinish_ = reject
      recording_ = false
      if (!durationTimeout_) {
        stopSession()
      }
    })
  )

  return {
    start,
    recordFrame,
    finish,
  }
}

export {
  createNativeRecorder,
}
