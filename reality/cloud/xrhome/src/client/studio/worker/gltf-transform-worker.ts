import {NodeIO} from '@gltf-transform/core'
import {
  textureCompress, dedup, instance, prune, resample, simplify, sparse, weld,
} from '@gltf-transform/functions'

import {MeshoptSimplifier} from 'meshoptimizer'

const SIMPLIFY_MESH_ERROR = 1.0
const SIMPLIFY_MESH_SPARSE_RATIO = 0.2

type JobQueue = {current: {id: string, data: any} | null, pending: {id: string, data: any} | null}
type JobQueues = {[type: string]: JobQueue}

// Note(cindyhu): we only want to queue the most recent request if there is already one
// in progress, and we will ignore all previous pending requests.
const jobQueues: JobQueues = {
  changeTextureSize: {current: null, pending: null},
  simplifyMesh: {current: null, pending: null},
}

const parseGltfExport = async (gltfExport: ArrayBuffer) => {
  const io = new NodeIO()
  const binToExport = new Uint8Array(gltfExport.byteLength)
  binToExport.set(new Uint8Array(gltfExport))
  const glbDocument = await io.readBinary(binToExport)
  return glbDocument
}

const changeTextureSize = async (newValue: string, gltfExport: ArrayBuffer) => {
  const newTextureNumber = parseInt(newValue, 10)
  const glbDocument = await parseGltfExport(gltfExport)

  if (!glbDocument) {
    throw new Error('Failed to transform ThreeJS glb to gltf-transform glb')
  }

  try {
    await glbDocument.transform(
      textureCompress({
        targetFormat: 'webp',
        resize: [newTextureNumber, newTextureNumber],
      })
    )
  } catch (error) {
    throw new Error(`Failed to compress texture: ${error.message}`)
  }

  const io = new NodeIO()
  const glbBinary = await io.writeBinary(glbDocument)
  return glbBinary
}

const simplifyMesh = async (newMeshRatio: number, gltfExport: ArrayBuffer) => {
  const glbDocument = await parseGltfExport(gltfExport)

  if (!glbDocument) {
    throw new Error('Failed to transform ThreeJS glb to gltf-transform glb')
  }

  try {
    await glbDocument.transform(
      dedup(),

      // Create GPU instancing batches for meshes used 5+ times.
      instance(),

      // Weld (index) all mesh geometry, removing duplicate vertices.
      weld(),

      // Simplify mesh geometry with meshoptimizer.
      simplify({
        simplifier: MeshoptSimplifier,
        error: SIMPLIFY_MESH_ERROR,
        ratio: newMeshRatio,
        lockBorder: true,
      }),

      // Losslessly resample animation frames.
      resample(),

      // Remove unused nodes, textures, materials, etc.
      prune(),

      // Create sparse accessors where >80% of values are zero.
      sparse({ratio: SIMPLIFY_MESH_SPARSE_RATIO})
    )
  } catch (error) {
    throw new Error(`Failed to compress texture: ${error.message}`)
  }

  const io = new NodeIO()
  const glbBinary = await io.writeBinary(glbDocument)
  return glbBinary
}

const executeJob = async (type: string, job) => {
  const {id, data} = job
  const queue = jobQueues[type]

  try {
    let result
    switch (type) {
      case 'changeTextureSize': {
        const {newValue, gltfExport} = data
        result = await changeTextureSize(newValue, gltfExport)
        break
      }
      case 'simplifyMesh': {
        const {newMeshRatio, gltfExport} = data
        result = await simplifyMesh(newMeshRatio, gltfExport)
        break
      }
      default:
        throw new Error(`Unknown message type: ${type}`)
    }
    if (queue.current?.id === id) {
      postMessage({id, type, result: {success: true, result}})
    }
  } catch (error) {
    if (queue.current?.id === id) {
      postMessage({id, type, result: {success: false, error: error.message}})
    }
  } finally {
    queue.current = null

    // Move on to the pending job if there is one
    if (queue.pending) {
      queue.current = queue.pending
      queue.pending = null
      executeJob(type, queue.current)
    }
  }
}

const clearJobQueues = () => {
  Object.entries(jobQueues).forEach(([type, queue]) => {
    if (queue.current) {
      postMessage({id: queue.current.id, type, result: {canceled: true}})
    }
    if (queue.pending) {
      postMessage({id: queue.pending.id, type, result: {canceled: true}})
    }
    queue.current = null
    queue.pending = null
  })
}

onmessage = async (event) => {
  const {id, type, data} = event.data

  if (type === 'reset') {
    clearJobQueues()
    return
  }

  const newJob = {id, data}
  const queue = jobQueues[type]

  if (!queue.current) {
    queue.current = newJob
    executeJob(type, queue.current)
  } else {
    // Cancel the previous pending job and replace it with the new job
    if (queue.pending) {
      postMessage({id: queue.pending.id, type, result: {canceled: true}})
    }
    queue.pending = newJob
  }
}
