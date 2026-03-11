import {THREE_LAYERS} from '@ecs/shared/three-layers'
import type {Object3D} from 'three'

const enableOutline = (object: Object3D) => {
  object.layers.enable(THREE_LAYERS.outline)
}

const removeOutline = (object: Object3D) => {
  object.layers.disable(THREE_LAYERS.outline)
}

const enableOutlineRecursive = (object: Object3D) => {
  object.traverse(enableOutline)
}

const removeOutlineRecursive = (object: Object3D) => {
  object.traverse(removeOutline)
}

export {
  enableOutline,
  removeOutline,
  enableOutlineRecursive,
  removeOutlineRecursive,
}
