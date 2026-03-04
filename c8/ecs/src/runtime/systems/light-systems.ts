import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Light, Position, Shadow} from '../components'
import {makeSystemHelper} from './system-helper'
import {createLightObject, editLightObject, sameLightType} from '../lights'
import type * as THREE_TYPES from '../three-types'
import THREE from '../three'
import {addChild, notifyChanged} from '../matrix-refresh'
import {LIGHT_DEFAULTS} from '../../shared/light-constants'

const FOLLOW_LIGHT_DISTANCE = 10

const makeShadowSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Shadow)

  const editShadows = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object) {
      const {castShadow, receiveShadow} = Shadow.get(world, eid)
      object.castShadow = !!castShadow
      object.receiveShadow = !!receiveShadow
    }
  }

  const disableShadows = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object) {
      object.castShadow = false
      object.receiveShadow = false
    }
  }

  return () => {
    exit(world).forEach(disableShadows)
    enter(world).forEach(editShadows)
    changed(world).forEach(editShadows)
  }
}

const makeLightSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Light)

  const activeFollowLightMap = new Map<Eid, THREE_TYPES.DirectionalLight>()
  const cameraDirection = new THREE.Vector3()
  const targetPosition = new THREE.Vector3()
  const tempPosition = new THREE.Vector3()

  const enableFollowLight = (
    eid: Eid,
    light: THREE_TYPES.DirectionalLight
  ) => {
    if (activeFollowLightMap.has(eid)) {
      return
    }

    activeFollowLightMap.set(eid, light)
    addChild(world.three.scene, light.target)
  }

  const disableFollowLight = (eid: Eid) => {
    if (!activeFollowLightMap.has(eid)) {
      return
    }

    const lightObject = activeFollowLightMap.get(eid)
    if (lightObject && lightObject.target) {
      world.three.scene.remove(lightObject.target)
    }

    activeFollowLightMap.delete(eid)
  }

  const addLight = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const lightConfig = Light.get(world, eid)
    const lightObject = createLightObject(world, eid, lightConfig)
    object.userData.light = lightObject
    addChild(object, lightObject)

    if (lightConfig.type === 'directional' && lightConfig.followCamera) {
      enableFollowLight(eid, lightObject as THREE_TYPES.DirectionalLight)
    }
  }

  const editLight = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const lightObject = object.userData.light

    if (!lightObject) {
      return
    }

    const light = Light.get(world, eid)
    const shouldFollowCamera = light.type === 'directional' && light.followCamera
    if (!shouldFollowCamera && activeFollowLightMap.has(eid)) {
      // only directional lights can have dynamic lighting OR the user turned off dynamic lighting
      disableFollowLight(eid)
    }

    if (!sameLightType(lightObject, light.type)) {
      object.remove(lightObject)
      const newLightObject = createLightObject(world, eid, light)
      object.userData.light = newLightObject
      addChild(object, newLightObject)

      if (shouldFollowCamera) {
        // new directional light
        enableFollowLight(eid, newLightObject as THREE_TYPES.DirectionalLight)
      }

      return
    } else if (shouldFollowCamera && !activeFollowLightMap.has(eid)) {
      // user turned on dynamic lighting
      enableFollowLight(eid, lightObject as THREE_TYPES.DirectionalLight)
    }

    editLightObject(world, eid, lightObject, light)
  }

  const removeLight = (eid: Eid) => {
    disableFollowLight(eid)
    const object = world.three.entityToObject.get(eid)
    if (!object) {
      return
    }

    object.remove(object.userData.light)
  }

  const processCameraFollow = () => {
    world.three.activeCamera.getWorldDirection(cameraDirection)
    cameraDirection.normalize().multiplyScalar(FOLLOW_LIGHT_DISTANCE)

    world.three.activeCamera.getWorldPosition(targetPosition)
    targetPosition.add(cameraDirection)

    for (const [eid, light] of activeFollowLightMap.entries()) {
      // TODO (jeffha): have smart shadows intelligently choose the area size
      light.shadow.camera.left = LIGHT_DEFAULTS.shadowCameraLeft
      light.shadow.camera.right = LIGHT_DEFAULTS.shadowCameraRight
      light.shadow.camera.top = LIGHT_DEFAULTS.shadowCameraTop
      light.shadow.camera.bottom = LIGHT_DEFAULTS.shadowCameraBottom
      light.shadow.camera.near = LIGHT_DEFAULTS.shadowCameraNear
      light.shadow.camera.far = LIGHT_DEFAULTS.shadowCameraFar
      light.shadow.camera.updateProjectionMatrix()

      light.target.position.copy(targetPosition)
      light.target.updateMatrixWorld()
      notifyChanged(light.target)

      const pos = Position.get(world, eid)
      tempPosition.set(pos.x, pos.y, pos.z)
      light.position.copy(targetPosition).add(tempPosition)
      notifyChanged(light)
    }
  }

  return () => {
    exit(world).forEach(removeLight)
    enter(world).forEach(addLight)
    changed(world).forEach(editLight)

    if (activeFollowLightMap.size > 0) {
      processCameraFollow()
    }
  }
}

export {
  makeShadowSystem,
  makeLightSystem,
}
