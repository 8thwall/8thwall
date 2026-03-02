// The transcoded worker offloads the realtime recording of the video to the native recorder,
// but applies a trancoding from webm to mp4 after the recording stops to allow for wider sharing.

import {createNativeRecorder} from './native-recorder'
import {initWorker} from './worker'

// This list is sorted by preferred first, vp8 is better as it is more efficient to encode.
// The audio codec "opus" must be specified, otherwise Firefox will reject.
const supportedEncodings = [
  'video/webm;codecs=vp8,opus',
  'video/webm;codecs=vp9,opus',
]

const getSupportedType = () => {
  if (!window.MediaRecorder || !window.MediaRecorder.isTypeSupported) {
    return null
  }

  return supportedEncodings.find(type => window.MediaRecorder.isTypeSupported(type))
}

const createTranscodedRecorder = (audioMixer, compositor) => {
  let mediaWorker_
  let resolveFinish_
  let rejectFinish_
  let progressCallback_

  const nativeRecorder_ = createNativeRecorder(audioMixer, compositor, {
    mimeType: getSupportedType(),
  })

  const clearCallbacks = () => {
    resolveFinish_ = null
    rejectFinish_ = null
    progressCallback_ = null
  }

  const connectPromise_ = initWorker().then((worker) => {
    mediaWorker_ = worker
    mediaWorker_.onmessage = (e) => {
      const result = e.data
      switch (result.type) {
        case 'transcodeComplete':
          if (resolveFinish_) {
            resolveFinish_({videoBlob: result.blob})
          }
          clearCallbacks()
          break
        case 'transcodeProgress':
          if (progressCallback_) {
            progressCallback_({progress: e.data.progress, total: e.data.total})
          }
          break
        case 'error':
          if (rejectFinish_) {
            rejectFinish_(result)
          }
          clearCallbacks()
          break
        default:
          // Ignore
      }
    }
  })

  // This promise will be resolved after the last chunk of data is returned from the recorder
  // and the transcoding completes
  const finish = ({onFinalizeProgress, onPreviewReady}) => (
    new Promise((resolve, reject) => {
      resolveFinish_ = resolve
      rejectFinish_ = reject
      progressCallback_ = onFinalizeProgress

      Promise.all([
        nativeRecorder_.finish(),
        connectPromise_,
      ]).then(([{videoBlob}]) => {
        onPreviewReady({videoBlob})
        mediaWorker_.postMessage({
          type: 'transcodeStart',
          blob: videoBlob,
        })
      }).catch(rejectFinish_)
    })
  )

  return {
    start: nativeRecorder_.start,
    recordFrame: nativeRecorder_.recordFrame,
    finish,
  }
}

const deviceSupportsTranscodedRecorder = () => !!getSupportedType()

export {
  deviceSupportsTranscodedRecorder,
  createTranscodedRecorder,
}
