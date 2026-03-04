import ModelManagerWasm from 'c8/model/web/model-manager-wasm'
import {writeStringToEmscriptenHeap, writeArrayToEmscriptenHeap} from 'c8/ems/ems'

import type {
  LoadBytesRequestData, ModelManagerRequest, MergeBytesRequestData,
} from './model-manager-types'

declare let OffscreenCanvas: any
const ctx: Worker = self as any  // eslint-disable-line no-restricted-globals
let offscreenCanvas: typeof OffscreenCanvas | null = null

let manager
let currContextHandle
const managerPromise = ModelManagerWasm({}).then((m) => {
  manager = m

  // If the browser supports WebGL2 OffscreenCanvas, create a context for the model manager.
  if (!OffscreenCanvas) {
    return
  }
  offscreenCanvas = new OffscreenCanvas(1, 1)
  const gl = offscreenCanvas?.getContext('webgl2', {antialias: false})
  if (!gl) {
    offscreenCanvas = null
    return
  }

  // synchronize our Emscripten GL to this context
  const attributes = gl.getContextAttributes() || {}
  if (gl.PIXEL_PACK_BUFFER) {
    (attributes as any).majorVersion = 2
  }

  // Reset the old string cache in case there was a different context before.
  manager.GL.stringCache = {}
  currContextHandle = manager.GL.registerContext(gl, attributes)
  manager.GL.makeContextCurrent(currContextHandle)
})

const loadBytes = ({filename, bytes}) => {
  const filenamePtr = writeStringToEmscriptenHeap(manager, filename)
  const dataPtr = writeArrayToEmscriptenHeap(manager, bytes)
  manager._c8EmAsm_loadModel(filenamePtr, dataPtr, bytes.byteLength)
  manager._free(filenamePtr)
  manager._free(dataPtr)
  const data = manager.HEAPU8.subarray(
    (ctx as any)._model8.modelData,
    (ctx as any)._model8.modelData + (ctx as any)._model8.modelDataSize
  )
  delete (ctx as any)._model8

  const transferableData = data.slice(0, data.length)
  ctx.postMessage(
    {msg: 'loadBytes', data: transferableData},
    {transfer: [transferableData.buffer]}
  )
}

const mergeBytes = ({filename, position, rotation, bytes}: MergeBytesRequestData) => {
  const mergeJsonPtr = writeStringToEmscriptenHeap(
    manager, JSON.stringify({filename, position, rotation})
  )
  const dataPtr = writeArrayToEmscriptenHeap(manager, bytes)
  manager._c8EmAsm_mergeModel(mergeJsonPtr, dataPtr, bytes.byteLength)
  manager._free(mergeJsonPtr)
  manager._free(dataPtr)
  const data = manager.HEAPU8.subarray(
    (ctx as any)._model8.modelData,
    (ctx as any)._model8.modelData + (ctx as any)._model8.modelDataSize
  )
  delete (ctx as any)._model8

  const transferableData = data.slice(0, data.length)
  ctx.postMessage(
    {msg: 'mergeBytes', data: transferableData},
    {transfer: [transferableData.buffer]}
  )
}

const loadFiles = (files: LoadBytesRequestData[]) => {
  if (files.length === 1) {
    const {filename, bytes} = files[0]
    const filenamePtr = writeStringToEmscriptenHeap(manager, filename)
    const dataPtr = writeArrayToEmscriptenHeap(manager, bytes)
    manager._c8EmAsm_loadModel(filenamePtr, dataPtr, bytes.byteLength)
    manager._free(filenamePtr)
    manager._free(dataPtr)
  } else if (files.length === 2) {
    const {filename: filename1, bytes: bytes1} = files[0]
    const {filename: filename2, bytes: bytes2} = files[1]
    const filename1Ptr = writeStringToEmscriptenHeap(manager, filename1)
    const data1Ptr = writeArrayToEmscriptenHeap(manager, bytes1)
    const filename2Ptr = writeStringToEmscriptenHeap(manager, filename2)
    const data2Ptr = writeArrayToEmscriptenHeap(manager, bytes2)
    manager._c8EmAsm_loadModelMultiFile(
      filename1Ptr, data1Ptr, bytes1.byteLength,
      filename2Ptr, data2Ptr, bytes2.byteLength
    )
    manager._free(filename1Ptr)
    manager._free(data1Ptr)
    manager._free(filename2Ptr)
    manager._free(data2Ptr)
  } else {
    console.error('Unsupported number of files', files)  // eslint-disable-line no-console
  }

  const data = manager.HEAPU8.subarray(
    (ctx as any)._model8.modelData,
    (ctx as any)._model8.modelData + (ctx as any)._model8.modelDataSize
  )
  delete (ctx as any)._model8

  const transferableData = data.slice(0, data.length)
  ctx.postMessage(
    {msg: 'loadFiles', data: transferableData},
    {transfer: [transferableData.buffer]}
  )
}

const updateView = ({cameraPos, cameraRot, modelPos, modelRot}) => {
  const updateData = JSON.stringify({cameraPos, cameraRot, modelPos, modelRot})
  const updateDataPtr = writeStringToEmscriptenHeap(manager, updateData)
  manager._c8EmAsm_updateView(updateDataPtr)
  manager._free(updateDataPtr)

  const {updated} = (ctx as any)._model8
  const data = manager.HEAPU8.subarray(
    (ctx as any)._model8.modelData,
    (ctx as any)._model8.modelData + (ctx as any)._model8.modelDataSize
  )
  delete (ctx as any)._model8

  const transferableData = data.slice(0, data.length)
  if (updated) {
    ctx.postMessage(
      {msg: 'updateView', updated, data: transferableData},
      {transfer: [transferableData.buffer]}
    )
  } else {
    ctx.postMessage({msg: 'updateView', updated})
  }
}

const configure = (config) => {
  if (config.bakeSkyboxMeters && !offscreenCanvas) {
    // eslint-disable-next-line no-console
    console.warn('[model-manager-worker] OffscreenCanvas not supported, ignoring bakeSkyboxMeters')
    delete config.bakeSkyboxMeters
  }
  const configData = JSON.stringify(config)
  const configDataPtr = writeStringToEmscriptenHeap(manager, configData)
  manager._c8EmAsm_configure(configDataPtr)
  manager._free(configDataPtr)

  ctx.postMessage({msg: 'configure'})
}

const run = (data: ModelManagerRequest) => {
  switch (data.msg) {
    case 'workerLoaded':
      ctx.postMessage({msg: 'workerLoaded'})
      break
    case 'loadBytes':
      loadBytes(data.data)
      break
    case 'loadFiles':
      loadFiles(data.data)
      break
    case 'mergeBytes':
      mergeBytes(data.data)
      break
    case 'updateView':
      updateView(data.data)
      break
    case 'configure':
      configure(data.config)
      break
    case 'dispose':
      manager.GL.deleteContext(currContextHandle)
      manager.GL.makeContextCurrent(null)
      offscreenCanvas._context = null
      ctx.postMessage({msg: 'dispose'})
      break
    default:
      console.error('Worker received unknown message', data)  // eslint-disable-line no-console
  }
}

onmessage = (e) => {
  managerPromise.then(() => run(e.data))
}
