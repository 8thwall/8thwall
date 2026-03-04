import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Camera} from '../components'
import {makeSystemHelper} from './system-helper'
import {
  CameraConfig, createCameraObject, editCameraObject, editXrCameraObject, sameCameraType,
  XR_CAMERA_PROPERTIES,
} from '../camera'
import {CameraEvents} from '../camera-events'
import {addChild} from '../matrix-refresh'

const xrCameraChanged = (
  curr: CameraConfig, change: CameraConfig
): boolean => {
  if (!curr || !change) {
    return false
  }
  return XR_CAMERA_PROPERTIES.some((property => curr[property] !== change[property]))
}

const makeCameraSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Camera)

  const addCamera = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (!object) {
      return
    }

    const camera = createCameraObject(Camera.get(world, eid))

    addChild(object, camera)
    object.userData.camera = camera
    world.camera.notifyCameraAdded(eid)
  }

  const editCamera = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (!object) {
      return
    }

    const {camera} = object.userData
    if (!camera) {
      return
    }

    const cameraAttributes = Camera.get(world, eid)
    if (!sameCameraType(camera, cameraAttributes.type)) {
      object.remove(camera)
      const newCamera = createCameraObject(cameraAttributes)
      addChild(object, newCamera)
      object.userData.camera = newCamera
      world.camera.notifyCameraAdded(eid)
    } else {
      editCameraObject(camera, cameraAttributes)
    }

    const xrCamera = object.userData.camera
    const cameraChanged = xrCameraChanged(xrCamera.userData.xr, cameraAttributes)
    if (cameraChanged) {
      editXrCameraObject(xrCamera, cameraAttributes)
    }

    if (world.camera.getActiveEid() === eid && cameraChanged) {
      // note(alancastillo): dispatch event to trigger xr pipeline restart
      world.events.dispatch(world.events.globalId, CameraEvents.XR_CAMERA_EDIT, {camera: xrCamera})
    }
  }

  const removeCamera = (eid: Eid) => {
    world.camera.notifyCameraRemoved(eid)
    const object = world.three.entityToObject.get(eid)
    if (!object) {
      return
    }

    object.remove(object.userData.camera)
    object.userData.camera = null
  }

  return () => {
    exit(world).forEach(removeCamera)
    enter(world).forEach(addCamera)
    changed(world).forEach(editCamera)
  }
}

export {
  makeCameraSystem,
}
