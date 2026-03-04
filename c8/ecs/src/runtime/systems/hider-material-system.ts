import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {HiderMaterial} from '../components'
import {makeSystemHelper} from './system-helper'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import {removeMaterial} from './material-systems-helpers'
import type {QueuedEvent} from '../events'
import {events} from '../event-ids'

const makeHiderMaterial = () => {
  const material = new THREE.MeshBasicMaterial()
  material.userData.disposable = true
  material.userData.hider = true
  material.colorWrite = false
  material.visible = true
  return material
}

const applyGroupHiderMaterial = (model: THREE_TYPES.Group) => {
  if (!model) {
    return
  }

  const hiderMaterial = makeHiderMaterial()

  // Set renderOrder for the whole group
  // (this makes it relative to all other objects in the scene without applying to individual nodes)
  model.renderOrder = -1

  model.traverse((node) => {
    if (node instanceof THREE.Mesh) {
      node.material = hiderMaterial
    }
  })
}

const applyHiderOnModelLoad = (event: QueuedEvent) => {
  const data = event.data as {model: THREE_TYPES.Group}
  // Make sure that the event target matches the model (events bubble up the hierarchy)
  if (event.target === event.currentTarget) {
    applyGroupHiderMaterial(data.model)
  }
}

const makeHiderMaterialSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(HiderMaterial)

  const applyMaterial = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object?.userData.gltf) {
      applyGroupHiderMaterial(object.userData.gltf.scene)
    } else if (object instanceof THREE.Mesh) {
      if (!object.material?.userData.hider) {
        object.material = makeHiderMaterial()
      }

      // renderOrder is 0 by default. For now, we always want to render this material first
      // TODO: Make renderOrder configurable with layers
      object.renderOrder = -1
    }

    // Have a listener waiting for a GLTF model to load (in case it hasn't loaded yet, or changes)
    world.events.addListener(eid, events.GLTF_MODEL_LOADED, applyHiderOnModelLoad)
  }

  return () => {
    exit(world).forEach((eid) => {
      removeMaterial(eid, world)
      const object = world.three.entityToObject.get(eid)
      // Reset renderOrder back to 0
      // TODO: Make renderOrder configurable with layers
      if (object?.userData.gltf) {
        object.userData.gltf.scene.renderOrder = 0
      } else if (object instanceof THREE.Mesh) {
        object.renderOrder = 0
      }

      world.events.removeListener(eid, events.GLTF_MODEL_LOADED, applyHiderOnModelLoad)
    })
    enter(world).forEach(applyMaterial)
    changed(world).forEach(applyMaterial)
  }
}

export {
  makeHiderMaterialSystem,
}
