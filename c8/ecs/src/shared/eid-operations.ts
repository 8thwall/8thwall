import type {Eid} from './schema'
import type * as THREE_TYPES from '../runtime/three-types'

// Recursively search for the first parent with an eid
// If I intersect a child object of a gltf for example, this will resolve to the entity that has
// the gltf attached.
const findEidForObject = (object: THREE_TYPES.Object3D): Eid | undefined => {
  if (object.userData.eid) {
    return object.userData.eid
  }
  if (object.parent) {
    return findEidForObject(object.parent)
  }
  return undefined
}

export {
  findEidForObject,
}
