import {MeshoptSimplifier} from 'meshoptimizer'

import {
  TARGET_ERROR, TARGET_INDEX_RATIO, TRIANGLE_VERTEX_COUNT, VERTEX_POSITION_STRIDE,
  VERTEX_ROUNDING_FACTOR,
} from '../shared/mesh-optimizer-constants'
import type * as THREE_TYPES from './three-types'
import {mergeVertices} from './three-merge-vertices'

interface MeshData {
  vertices: Float32Array
  indices: Uint32Array
}

const getGltfMeshData = (scene: THREE_TYPES.Group): MeshData => {
  const allVertices: number[] = []
  const allIndices: number[] = []
  let vertexOffset = 0

  scene.traverse((object) => {
    if (!(object as THREE_TYPES.Mesh).isMesh) {
      return
    }
    const {geometry} = object as THREE_TYPES.Mesh

    try {
      const deduplicatedGeometry = mergeVertices(geometry)

      if (!deduplicatedGeometry.index) {
        return
      }

      const positions = deduplicatedGeometry.attributes.position.array as Float32Array
      const indices = new Uint32Array(deduplicatedGeometry.index.array)

      const targetIndexCount = Math.floor(
        indices.length * (TARGET_INDEX_RATIO / TRIANGLE_VERTEX_COUNT)
      ) * TRIANGLE_VERTEX_COUNT

      const [simplifiedIndices] = MeshoptSimplifier.simplify(
        indices, positions, VERTEX_POSITION_STRIDE, targetIndexCount, TARGET_ERROR
      )

      const simplifiedVertices = new Float32Array(
        simplifiedIndices.length * VERTEX_POSITION_STRIDE
      )

      for (let i = 0; i < simplifiedIndices.length; i++) {
        const index = simplifiedIndices[i] * VERTEX_POSITION_STRIDE
        for (let j = 0; j < VERTEX_POSITION_STRIDE; j++) {
          simplifiedVertices[i * VERTEX_POSITION_STRIDE + j] =
              Math.round(positions[index + j] * VERTEX_ROUNDING_FACTOR) / VERTEX_ROUNDING_FACTOR
        }
      }

      allVertices.push(...simplifiedVertices)

      for (let i = 0; i < simplifiedIndices.length; i++) {
        allIndices.push(vertexOffset + i)
      }

      vertexOffset += simplifiedIndices.length
    } catch (error) {
      throw new Error(`Failed to simplify mesh geometry: ${error}`)
    }
  })

  return {
    vertices: new Float32Array(allVertices),
    indices: new Uint32Array(allIndices),
  }
}

const getGltfVertices = (scene: THREE_TYPES.Group) => getGltfMeshData(scene).vertices

export {
  getGltfVertices,
  getGltfMeshData,
  type MeshData,
}
