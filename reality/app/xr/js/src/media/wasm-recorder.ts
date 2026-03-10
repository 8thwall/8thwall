import {initWorker} from './worker'

// Timescale units are default 90,000/s:
//  https://github.com/8thwall/code8/blob/master/c8/media/codec/mp4v2-muxer.cc#L80
const TIMESCALE_UNITS_PER_MS = 90

const createWasmRecorder = (audioMixer, compositor) => {
  let mediaWorker_
  let videoWidth_
  let videoHeight_
  let resolveFinish_
  let rejectFinish_

  const initialSetup = () => (
    initWorker().then((worker) => {
      mediaWorker_ = worker
      mediaWorker_.onmessage = (e) => {
        const result = e.data
        switch (result.type) {
          case 'videoComplete':
            if (resolveFinish_) {
              resolveFinish_({videoBlob: result.blob})
            }
            break
          case 'error':
            // eslint-disable-next-line no-console
            console.error('workererror', result.message)
            if (rejectFinish_) {
              rejectFinish_(result)
            }
            break
          default:
          // Ignore
        }
      }
    })
  )

  const encodeAudio = (audioEvent) => {
    const channel = audioEvent.inputBuffer.getChannelData(0)
    mediaWorker_.postMessage({
      type: 'encodeAudio',
      // Partially create params, leaving dataX for the worker to fill in.
      params: {
        name: 'audio',
      },
      sample: channel,
    })
  }

  const start = (width, height) => {
    videoWidth_ = width
    videoHeight_ = height
    mediaWorker_.postMessage({
      type: 'start',
      config: {
        container: 'mp4',
        tracks: [{
          name: 'video',
          codec: 'h264',
          rcMode: 3,  // Timestamp-based rate control. Appropriate for variable frame rates.
          width,
          height,
          qp: 23,
          bitrate: 5000000,
        }, {
          name: 'audio',
          codec: 'aac',
          profile: 2,
          sampleRate: audioMixer.getSampleRate(),
          bitrate: 131072,
        }],
      },
    })

    // Start audio capture.
    audioMixer.setAudioHandler(encodeAudio)
  }

  const recordFrame = (elapsedTime, frameDurationMs = undefined) => {
    const timestamp = elapsedTime * TIMESCALE_UNITS_PER_MS
    const frameDuration = frameDurationMs ? frameDurationMs * TIMESCALE_UNITS_PER_MS : undefined
    const pixels = compositor.getCanvasData()

    // Post the frame to the mediaWorker to encode
    mediaWorker_.postMessage({
      type: 'encodeVideo',
      // Partially create params, leaving dataX for the worker to fill in.
      params: {
        name: 'video',
        width: videoWidth_,
        height: videoHeight_,
        timestamp,
        frameDuration,
        rowBytes0: videoWidth_,
        // eslint-disable-next-line no-bitwise
        rowBytes1: videoWidth_ >> 1,
        // eslint-disable-next-line no-bitwise
        rowBytes2: videoWidth_ >> 1,
      },
      pixels,
    })
  }

  const finish = () => {
    audioMixer.setAudioHandler(undefined)
    return new Promise((resolve, reject) => {
      resolveFinish_ = resolve
      rejectFinish_ = reject
      mediaWorker_.postMessage({
        type: 'finish',
      })
    })
  }

  return {
    initialSetup,
    start,
    recordFrame,
    finish,
  }
}

export {
  createWasmRecorder,
}
