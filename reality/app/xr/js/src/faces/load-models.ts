// Loads tensorflow, wasm backend, and models.
import type {XrFaceccModule} from 'reality/app/xr/js/src/types/xrfacecc'

import {ResourceUrls} from '../resources'

const loadDataPromise = async (url): Promise<Uint8Array> => {
  const data = await fetch(url)
  const buffer = await data.arrayBuffer()
  return new Uint8Array(buffer)
}

// Load face mesh models into xrcc module
const loadFacemeshModels = async (
  xrFaceccPromise: Promise<XrFaceccModule>
): Promise<void> => {
  const faceModelPromise = loadDataPromise(ResourceUrls.resolveFaceModel())
  const meshModelPromise = loadDataPromise(ResourceUrls.resolveFaceMeshModel())
  const xrFacecc = await xrFaceccPromise

  // Load face model
  {
    const faceModel = await faceModelPromise
    const dataPtr = xrFacecc._malloc(faceModel.length)
    xrFacecc.writeArrayToMemory(faceModel, dataPtr)
    xrFacecc._c8EmAsm_loadFaceDetectModel(dataPtr, faceModel.length)
    xrFacecc._free(dataPtr)
  }

  // Load mesh model
  {
    const meshModel = await meshModelPromise
    const dataPtr = xrFacecc._malloc(meshModel.length)
    xrFacecc.writeArrayToMemory(meshModel, dataPtr)
    xrFacecc._c8EmAsm_loadFaceMeshModel(dataPtr, meshModel.length)
    xrFacecc._free(dataPtr)
  }
}

const loadEarModel = async (xrFaceccPromise: Promise<XrFaceccModule>): Promise<void> => {
  const earModelPromise = loadDataPromise(ResourceUrls.resolveFaceEarModel())
  const xrFacecc = await xrFaceccPromise
  const earModel = await earModelPromise
  const dataPtr = xrFacecc._malloc(earModel.length)
  xrFacecc.writeArrayToMemory(earModel, dataPtr)
  xrFacecc._c8EmAsm_loadEarModel(dataPtr, earModel.length)
  xrFacecc._free(dataPtr)
}

export {
  loadFacemeshModels,
  loadEarModel,
}
