import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {VideoMaterial} from '../components'
import {makeSystemHelper} from './system-helper'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import {events} from '../event-ids'
import type {QueuedEvent} from '../events'
import {MATERIAL_DEFAULTS} from '../../shared/material-constants'
import {removeMaterial, loadTexture, setMaterialColor, TextureId} from './material-systems-helpers'
import {getVideoManager} from '../video-manager'

const makeVideoMaterial = () => {
  const material = new THREE.MeshBasicMaterial()
  material.userData.disposable = true
  material.userData.video = true
  return material
}

const makeVideoMaterialSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(VideoMaterial)
  const videoEidToTextureId = new Map<Eid, TextureId>()
  const eidToAbortController = new Map<Eid, AbortController>()

  const configureVideoMaterial = (eid: Eid, material: THREE_TYPES.MeshBasicMaterial) => {
    eidToAbortController.get(eid)?.abort()
    const abortController = new AbortController()
    eidToAbortController.set(eid, abortController)

    const {r, g, b, textureSrc, opacity} = VideoMaterial.get(world, eid)
    const {repeatX, repeatY, offsetX, offsetY, wrap} = MATERIAL_DEFAULTS

    setMaterialColor(material.color, r, g, b)
    material.transparent = opacity < 1
    material.opacity = opacity

    if (material.map && material.userData.map !== textureSrc) {
      getVideoManager(world).maybeRemoveVideo(eid, material.map.uuid)
      videoEidToTextureId.delete(eid)
    }

    loadTexture(material, textureSrc, 'map', repeatX, repeatY, offsetX, offsetY, wrap,
      THREE.LinearFilter, THREE.LinearFilter,
      THREE.SRGBColorSpace).then((texture) => {
      if (abortController.signal.aborted) {
        return
      }

      if (texture && texture instanceof THREE.VideoTexture) {
        getVideoManager(world).addVideo(eid, texture)
        videoEidToTextureId.set(eid, texture.uuid)
      }
    })
  }

  const applyVideoGroupMaterial = (model: THREE_TYPES.Group, eid: Eid) => {
    if (!model) {
      return
    }

    const object = world.three.entityToObject.get(eid)

    if (!(object instanceof THREE.Mesh)) {
      return
    }

    const hasVideoMaterial = object.material?.userData.video
    const material = hasVideoMaterial ? object.material : makeVideoMaterial()
    configureVideoMaterial(eid, material)

    model.traverse((node: any) => {
      if (node instanceof THREE.Mesh) {
        node.material = material
      }
    })

    if (!hasVideoMaterial) {
      object.material = material
    }
  }

  const applyVideoOnModelLoad = (event: QueuedEvent) => {
    const data = event.data as {model: THREE_TYPES.Group}
    // Make sure that the event target matches the model (events bubble up the hierarchy)
    if (event.target === event.currentTarget) {
      applyVideoGroupMaterial(data.model, event.currentTarget)
    }
  }

  const applyMaterial = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (object?.userData.gltf) {
      applyVideoGroupMaterial(object.userData.gltf.scene, eid)
    } else if (object instanceof THREE.Mesh) {
      if (!object.material?.userData.video) {
        object.material = makeVideoMaterial()
      }
      configureVideoMaterial(eid, object.material)
    }
  }

  const removeVideoMaterial = (eid: Eid) => {
    eidToAbortController.get(eid)?.abort()
    eidToAbortController.delete(eid)

    const textureId = videoEidToTextureId.get(eid)
    if (textureId) {
      getVideoManager(world).maybeRemoveVideo(eid, textureId)
    }
    videoEidToTextureId.delete(eid)

    removeMaterial(eid, world)
    world.events.removeListener(eid, events.GLTF_MODEL_LOADED, applyVideoOnModelLoad)
  }

  const enterVideoMaterial = (eid: Eid) => {
    applyMaterial(eid)
    world.events.addListener(eid, events.GLTF_MODEL_LOADED, applyVideoOnModelLoad)
  }

  return () => {
    exit(world).forEach(removeVideoMaterial)
    enter(world).forEach(enterVideoMaterial)
    changed(world).forEach(applyMaterial)
  }
}

export {
  makeVideoMaterialSystem,
}
