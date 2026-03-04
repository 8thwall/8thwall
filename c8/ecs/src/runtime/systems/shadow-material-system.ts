import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {ShadowMaterial} from '../components'
import {makeSystemHelper} from './system-helper'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import {removeMaterial, setMaterialColor} from './material-systems-helpers'
import {sides} from '../three-side'
import type {QueuedEvent} from '../events'
import {events} from '../event-ids'

const makeShadowMaterialSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(ShadowMaterial)

  const applyShadowGroupMaterial = (model: THREE_TYPES.Group, eid: Eid) => {
    if (!model) {
      return
    }

    const {
      r, g, b, side, opacity, depthTest, depthWrite,
    } = ShadowMaterial.get(world, eid)

    const shadowMat = new THREE.ShadowMaterial(
      {color: new THREE.Color(r, g, b)}
    )
    shadowMat.userData.disposable = true

    setMaterialColor(shadowMat.color, r, g, b)
    shadowMat.opacity = opacity
    shadowMat.side = sides[side as 'front' | 'back' | 'double']
    shadowMat.depthTest = depthTest
    shadowMat.depthWrite = depthWrite
    shadowMat.needsUpdate = true

    model.traverse((node: any) => {
      if (node.material as THREE_TYPES.Material) {
        node.material = shadowMat
      }
      node.receiveShadow = true
    })
  }

  const applyShadowOnModelLoad = (event: QueuedEvent) => {
    const data = event.data as {model: THREE_TYPES.Group}
    // Make sure that the event target matches the model (events bubble up the hierarchy)
    if (event.target === event.currentTarget) {
      applyShadowGroupMaterial(data.model, event.currentTarget)
    }
  }

  const applyMaterial = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh) {
      let material: THREE_TYPES.ShadowMaterial
      if (object.material instanceof THREE.ShadowMaterial) {
        material = object.material
      } else {
        material = new THREE.ShadowMaterial()
        material.userData.disposable = true
        object.material = material
      }

      const {
        r, g, b, side, opacity, depthTest, depthWrite,
      } = ShadowMaterial.get(world, eid)

      setMaterialColor(material.color, r, g, b)
      material.side = sides[side as 'front' | 'back' | 'double']
      material.transparent = true
      material.opacity = opacity
      material.depthTest = depthTest
      material.depthWrite = depthWrite
      material.needsUpdate = true
    } else if (object?.userData.gltf) {
      applyShadowGroupMaterial(object.userData.gltf.scene, eid)
    }
  }
  return () => {
    exit(world).forEach((eid) => {
      removeMaterial(eid, world)
      world.events.removeListener(eid, events.GLTF_MODEL_LOADED, applyShadowOnModelLoad)
    })
    enter(world).forEach((eid) => {
      applyMaterial(eid)
      world.events.addListener(eid, events.GLTF_MODEL_LOADED, applyShadowOnModelLoad)
    })
    changed(world).forEach(applyMaterial)
  }
}

export {
  makeShadowMaterialSystem,
}
