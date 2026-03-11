import {v4 as uuidv4} from 'uuid'

interface TransformPromise {
  resolve: (result?: Uint8Array) => void
  reject: (reason?: any) => void
}

let worker: Worker = null
const pendingPromises = new Map<string, TransformPromise>()

const getWorker = () => {
  if (!worker) {
    worker = Build8.PLATFORM_TARGET === 'desktop'
      ? new Worker('desktop://dist/worker/client/gltfTransform-worker.js')
      : new Worker(`/${Build8.DEPLOYMENT_PATH}/client/gltfTransform-worker.js`)
    worker.onmessage = (event) => {
      const {id, result} = event.data
      const pending = pendingPromises.get(id)
      if (pending) {
        if (result.success) {
          pending.resolve(result.result)
        } else if (result.canceled) {
          pending.resolve()
        } else {
          pending.reject(new Error(result.error))
        }
        pendingPromises.delete(id)
      }
    }

    worker.onerror = (error) => {
      pendingPromises.forEach(({reject}) => reject(error))
      pendingPromises.clear()
    }
  }
  return worker
}

const resetGltfTransformWorker = () => {
  getWorker().postMessage({type: 'reset'})
}

const changeTextureSize = (newValue: string, gltfExport: ArrayBuffer) => (
  new Promise<Uint8Array>((resolve, reject) => {
    const id = uuidv4()
    pendingPromises.set(id, {resolve, reject})
    getWorker().postMessage({
      id,
      type: 'changeTextureSize',
      data: {newValue, gltfExport},
    })
  }))

const simplifyMesh = (newMeshRatio: number, gltfExport: ArrayBuffer) => (
  new Promise<Uint8Array>((resolve, reject) => {
    const id = uuidv4()
    pendingPromises.set(id, {resolve, reject})
    getWorker().postMessage({
      id,
      type: 'simplifyMesh',
      data: {newMeshRatio, gltfExport},
    })
  }))

export {
  changeTextureSize,
  simplifyMesh,
  resetGltfTransformWorker,
}
