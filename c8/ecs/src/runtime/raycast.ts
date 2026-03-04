import type {IntersectionResult} from './raycast-types'
import type {Intersection, Object3D} from './three-types'
import type {Eid} from '../shared/schema'
import type {World} from './world'
import {findEidForObject} from '../shared/eid-operations'
import {vec3, Vec3Source} from './math/vec3'
import {mat4} from './math/mat4'
import {quat} from './math/quat'
import THREE from './three'
import {makeRaycastIntersections} from '../shared/custom-sorting'

const tempRayVector = vec3.zero()
const tempRaycaster = new THREE.Raycaster()
const tempMat4 = mat4.i()
const tempTrs = {
  t: vec3.zero(),
  r: quat.zero(),
  s: vec3.zero(),
}

const processRaycastResults = (
  intersectedObjects: Intersection[]
): IntersectionResult[] => intersectedObjects.map(item => ({
  eid: findEidForObject(item.object as Object3D),
  point: vec3.from(item.point),
  distance: item.distance,
  threeData: item,
}))

const raycast = (
  world: World,
  origin: Vec3Source,
  direction: Vec3Source,
  near: number = 0,
  far: number = Infinity
): IntersectionResult[] => {
  tempRaycaster.near = near
  tempRaycaster.far = far
  tempRaycaster.camera = world.three.activeCamera
  tempRaycaster.ray.origin.copy(origin)
  tempRaycaster.ray.direction.copy(direction)

  const intersectedObjects: Intersection[] = makeRaycastIntersections([])

  tempRaycaster.intersectObjects(world.three.scene.children, true, intersectedObjects)
  return processRaycastResults(intersectedObjects)
}

const raycastFrom = (
  world: World,
  eid: Eid,
  near: number = 0,
  far: number = Infinity
) => {
  world.getWorldTransform(eid, tempMat4)
  tempMat4.decomposeTrs(tempTrs)
  tempRayVector.setXyz(0, 0, 1)
  tempTrs.r.timesVec(tempRayVector, tempRayVector)
  return raycast(world, tempTrs.t, tempRayVector, near, far)
}

export {
  raycast,
  raycastFrom,
}
