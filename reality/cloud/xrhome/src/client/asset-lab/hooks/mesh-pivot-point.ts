import * as THREE from 'three'
import type {TypedArray} from '@gltf-transform/core'

import type {Vec3Tuple} from '@ecs/shared/scene-graph'

import type {PivotPointOptions} from '../types'

type BoundingBox = [number, number, number, number, number, number]
const getBoundingBox = (positionArray: TypedArray): BoundingBox => {
  let minX = Infinity,
    minY = Infinity,
    minZ = Infinity
  let maxX = -Infinity,
    maxY = -Infinity,
    maxZ = -Infinity
  for (let i = 0; i < positionArray.length; i += 3) {
    minX = Math.min(minX, positionArray[i])
    minY = Math.min(minY, positionArray[i + 1])
    minZ = Math.min(minZ, positionArray[i + 2])
    maxX = Math.max(maxX, positionArray[i])
    maxY = Math.max(maxY, positionArray[i + 1])
    maxZ = Math.max(maxZ, positionArray[i + 2])
  }
  return [
    minX, minY, minZ,
    maxX, maxY, maxZ,
  ]
}

const getPivot = (
  pivotPoint: PivotPointOptions,
  boundingBox: BoundingBox
): Vec3Tuple => {
  const [minX, minY, minZ, maxX, maxY, maxZ] = boundingBox
  const pivotOffset = new THREE.Vector3()
  if (pivotPoint !== 'center') {
    const size = new THREE.Vector3(maxX - minX, maxY - minY, maxZ - minZ)
    const center = new THREE.Vector3(
      (minX + maxX) / 2,
      (minY + maxY) / 2,
      (minZ + maxZ) / 2
    )
    switch (pivotPoint) {
      case 'right': pivotOffset.x = -center.x + (minX + size.x); break
      case 'left': pivotOffset.x = -center.x + minX; break
      case 'bottom': pivotOffset.y = -center.y + (minY + size.y); break
      case 'top': pivotOffset.y = -center.y + minY; break
      case 'front': pivotOffset.z = -center.z + (minZ + size.z); break
      case 'back': pivotOffset.z = -center.z + minZ; break
      default:  // 'center'
    }
  }
  return [pivotOffset.x, pivotOffset.y, pivotOffset.z]
}

const getPositionForPivotPoint = (
  pivotPoint: PivotPointOptions, positionArray: TypedArray
): Vec3Tuple => {
  // Calculate bounding box for pivot point adjustment
  const bb = getBoundingBox(positionArray)

  // Calculate pivot offset based on settings
  return getPivot(pivotPoint, bb)
}

export {
  getPositionForPivotPoint,
  getBoundingBox,
  getPivot,
}
