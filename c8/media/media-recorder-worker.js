// Emscripten doesn't seem to be able to handle destructuring, it wasn't ending up in the build
/* eslint-disable prefer-destructuring */
/* eslint-disable no-bitwise */

import mr8cc from 'c8/media/media-recorder-js-asm-cc'  // eslint-disable-line

let Module = null
self.MR8CC = () => mr8cc().then((module) => {  /* eslint-disable-line no-restricted-globals */
  Module = module
  return module
})

let framePtr
let frameSize
let yuv
let uOffset
let vOffset

const TEMP_PATH = '/tmp/current-recording.mp4'

const postWorkerError = (code, message) => {
  postMessage({
    type: 'error',
    code,
    message,
  })
}

const start = (config) => {
  if (framePtr) {
    Module._free(framePtr)
    frameSize = 0
  }

  // Set the file path
  Object.assign(config, {
    path: TEMP_PATH,
  })

  // Find the h.264 track.
  const h264Track = config.tracks.find(x => x.codec === 'h264')
  if (h264Track === undefined) {
    postWorkerError(1, 'Missing h264 track')
  }

  frameSize = (h264Track.width * h264Track.height * 3) >> 1
  framePtr = Module._malloc(frameSize)
  yuv = new Uint8ClampedArray(frameSize)
  uOffset = h264Track.width * h264Track.height
  vOffset = uOffset + (h264Track.width >> 1) * (h264Track.height >> 1)

  Module.mediaRecorderStart(JSON.stringify(config))
}

const encodeVideo = (params, pixels) => {
  const {width} = params
  const {height} = params
  const paramsString = JSON.stringify(Object.assign(params, {
    data0: framePtr,
    data1: framePtr + width * height,
    data2: framePtr + width * height + (width >> 1) * (height >> 1),
  }))

  // Convert RGBA to yuv420p, row by row.
  let rgbaStart = 0
  let yStart = 0
  let uStart = uOffset
  let vStart = vOffset
  for (let h = 0; h < height; ++h) {
    const rgbaEnd = rgbaStart + (width << 2)
    const rgba = pixels.subarray(rgbaStart, rgbaEnd)
    const yPlane = yuv.subarray(yStart, yStart + width)
    const uPlane = yuv.subarray(uStart, uStart + (width >> 1))
    const vPlane = yuv.subarray(vStart, vStart + (width >> 1))
    if (h % 2 === 0) {
      uPlane.fill(0)
      vPlane.fill(0)
    }
    for (let w = 0; w < width; ++w) {
      const index = w << 2
      const r = rgba[index]
      const g = rgba[index + 1]
      const b = rgba[index + 2]
      yPlane[w] = 0.299 * r + 0.587 * g + 0.114 * b
      uPlane[w >> 1] += (Math.floor(-0.169 * r + -0.331 * g + 0.5 * b) + 128) >> 2
      vPlane[w >> 1] += (Math.floor(0.500 * r + -0.419 * g + -0.081 * b) + 128) >> 2
    }
    rgbaStart = rgbaEnd
    yStart += width
    uStart += (width >> 1) * (h % 2)
    vStart += (width >> 1) * (h % 2)
  }

  // Write the frame memory.
  Module.writeArrayToMemory(yuv, framePtr)

  // Encode the frame
  Module.mediaRecorderEncode(paramsString, framePtr, frameSize)
}

const encodeAudio = (params, sample) => {
  // Set up the params string.
  const paramsString = JSON.stringify(params)

  // TODO(mc): Reuse this buffer.
  const sampleSize = sample.length * 2  // 16-bits
  const samplePtr = Module._malloc(sampleSize)

  const uBuffer = new Uint8Array(sampleSize)
  const pcm = new DataView(uBuffer.buffer)

  for (let i = 0; i < sampleSize; i += 2) {
    const s = sample[i >> 1]
    pcm.setInt16(i, s < 0 ? s * 0x8000 : s * 0x7FFF, true)
  }

  // Write the sample memory.
  Module.writeArrayToMemory(uBuffer, samplePtr)

  // Encode the frame
  Module.mediaRecorderEncode(paramsString, samplePtr, sampleSize)

  // Free the sample buffer
  Module._free(samplePtr)
}

const finish = () => {
  Module.mediaRecorderFinish()
  const fileBytes = Module.FS.readFile(TEMP_PATH)
  const blob = new Blob([fileBytes], {
    type: 'video/mp4',
  })
  postMessage({
    type: 'videoComplete',
    blob,
  })

  if (framePtr) {
    Module._free(framePtr)
    frameSize = 0
    yuv = null
  }
}

// These correspond to the statuses in media-status.h
const SUCCESS = 0
const NO_MORE_FRAMES = 100

const TRANSCODE_INPUT = '/transcode-input.webm'
const TRANSCODE_OUTPUT = '/transcode-output.mp4'

const SAMPLES_PER_PASS = 10

const doTranscode = (totalSamples, completedSamples = 0) => {
  postMessage({
    type: 'transcodeProgress',
    progress: completedSamples,
    total: totalSamples,
  })

  let result

  for (let i = 0; i < SAMPLES_PER_PASS; i++) {
    result = Module.mediaTranscoderEncode()
    if (result !== SUCCESS) {
      break
    }
  }

  if (result === SUCCESS) {
    setTimeout(() => doTranscode(totalSamples, completedSamples + SAMPLES_PER_PASS), 0)
  } else if (result === NO_MORE_FRAMES) {
    result = Module.mediaTranscoderFinish()

    if (result !== SUCCESS) {
      postWorkerError(result, 'Encountered error while finishing')
      return
    }

    const outputBlob = new Blob([Module.FS.readFile(TRANSCODE_OUTPUT)], {type: 'video/mp4'})

    postMessage({
      type: 'transcodeComplete',
      blob: outputBlob,
    })
  } else {
    postWorkerError(result, 'Encountered error while transcoding')
  }
}

const startTranscode = (blob) => {
  blob.arrayBuffer().then((buffer) => {
    Module.FS.writeFile(TRANSCODE_INPUT, new Uint8Array(buffer))

    const result = Module.mediaTranscoderStart(JSON.stringify({
      path: TRANSCODE_INPUT,
    }), JSON.stringify({
      path: TRANSCODE_OUTPUT,
      tracks: [{name: 'video', codec: 'h264'}, {name: 'audio', codec: 'aac'}],
    }))

    if (result !== SUCCESS) {
      postWorkerError(result, 'Encountered error while starting transcode')
      return
    }

    const infoResult = Module.mediaTranscoderGetInfo()

    if (infoResult.status !== SUCCESS) {
      postWorkerError(result, 'Encountered error while getting info')
      return
    }

    try {
      const {tracks} = JSON.parse(infoResult.inputInfo)
      doTranscode(tracks.reduce((total, track) => total + track.samples || 0, 0))
    } catch (err) {
      postWorkerError(result, 'Encountered error parsing info')
    }
  })
}

self.onmessage = (e) => {  /* eslint-disable-line no-restricted-globals */
  switch (e.data.type) {
    case 'start':
      start(e.data.config)
      break
    case 'encodeVideo':
      encodeVideo(e.data.params, e.data.pixels)
      break
    case 'encodeAudio':
      encodeAudio(e.data.params, e.data.sample)
      break
    case 'finish':
      finish()
      break
    case 'transcodeStart':
      startTranscode(e.data.blob)
      break
    default:
      postWorkerError(1, 'media message missing \'type\' field')
      break
  }
}

self.initWorker = () => {  /* eslint-disable-line no-restricted-globals */
  postMessage({type: 'loaded'})
}
