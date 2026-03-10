// @rule(js_binary)
// @attr(polyfill = "none")
// @attr(npm_rule = "@npm-capnp-ts//:npm-capnp-ts")

/* eslint-disable no-restricted-globals */

/* eslint-disable import/no-unresolved */
// @dep(//reality/app/xr/js:semantics-worker-wasm)
import SEMANTICSMODULE from 'reality/app/xr/js/semantics-worker-wasm'

import {ResourceUrls} from '../resources'

let init_ = false

let Module = null

const MODEL_THRESHOLD = 0.4

self.SEMANTICSMODULE = () => SEMANTICSMODULE().then((module) => {
  Module = module
  return module
})

const postWorkerError = (code, message) => {
  postMessage({
    type: 'error',
    code,
    message,
  })
}

const loadDataPromise = async (url) => {
  const data = await fetch(url)
  const buffer = await data.arrayBuffer()
  return new Uint8Array(buffer)
}

const loadSemanticsModel = () => {
  // Loads the semantics classifier model. (currently fixed at the landscape view).
  const loadSemanticsTfliteP = Promise.all([
    loadDataPromise(ResourceUrls.resolveSemanticsModel()),
  ]).then(([model]) => {
    // Pass the bytes to Module.
    const dataPtr = Module._malloc(model.length)
    Module.writeArrayToMemory(model, dataPtr)
    Module._c8EmAsm_loadSemanticsModel(dataPtr, model.length)
    Module._free(dataPtr)
  })

  return Promise.all([loadSemanticsTfliteP])
}

// Loads semantics model at the start.
const start = () => {
  if (init_) {
    return
  }
  init_ = true
  Promise.all([
    loadSemanticsModel(),
  ]).then(() => {
    Module._c8EmAsm_semanticsWorkerStart()
  }).then(() => {
    postMessage({type: 'started'})
  })
}

const invoke = (img, position, rotation) => {
  // Copies the input image into an array.
  const totalTimeStart = Date.now()
  const ptr = Module._malloc(img.byteLength)
  Module.writeArrayToMemory(new Uint8Array(img), ptr)

  const timeStart = Date.now()
  Module._c8EmAsm_processStagedSemanticsWorkerFrame(ptr, 256, 144)
  Module._free(ptr)
  const invokeTime = Date.now() - timeStart

  if (!self._c8.responseReceived) {
    postWorkerError(1, '[XR] Could not perform semantics classifier invoke')
    return
  }

  // Receives array from semantics worker cc.
  const startAddr = self._c8.response
  const endAddr = startAddr + self._c8.responseSize

  // Need to account of size of the float (4 bytes) since subarray expects index.
  const semanticsArray = Module.HEAPF32.subarray(
    startAddr / 4,
    endAddr / 4
  )

  // Copies to a detachable array buffer.
  let validCount = 0
  const buffer = new ArrayBuffer(semanticsArray.length * 4)
  const semanticsView = new Float32Array(buffer)
  for (let i = 0; i < semanticsArray.length; i++) {
    semanticsView[i] = semanticsArray[i]
    if (semanticsView[i] > MODEL_THRESHOLD) {
      validCount++
    }
  }
  const percentage = validCount / semanticsArray.length

  const totalTime = Date.now() - totalTimeStart
  const debug = {
    invokeTime,
    totalTime,
  }

  const output = {
    data: buffer,
    position,
    rotation,
    percentage,
    debug,
  }

  postMessage({type: 'invoke', output}, [buffer])
}

const cleanup = () => {
  // We have set the values of the SemanticsWorkerData singleton inside semantics-worker.cc
  // to nullptr when calling semanticsWorkerCleanup.  We need to unreference those values
  // in JS or else we will be pointing to an already deleted object.
  self._c8.response = null
  self._c8.responseSize = null
  self._c8.responseReceived = null

  if (Module._c8EmAsm_SemanticsWorkerCleanup) {
    Module._c8EmAsm_SemanticsWorkerCleanup()
  }
}

self.onmessage = (e) => {
  switch (e.data.type) {
    case 'start':
      ResourceUrls.setBaseUrl(e.data.config.baseUrl)
      start()
      break
    case 'invoke':
      invoke(e.data.img, e.data.position, e.data.rotation)
      break
    case 'cleanup':
      cleanup()
      break
    default:
      postWorkerError(1, '[XR] Media message missing \'type\' field')
      break
  }
}

self.initWorker = () => { /* eslint-disable-line no-restricted-globals */
  postMessage({type: 'loaded'})
}
