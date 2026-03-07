import type {Eid} from '../shared/schema'
import type {World} from './world'
import type {AudioListener} from './three-types'
import type {Events} from './events'
import type {CameraManager, CameraObject} from './camera-manager-types'
import {CameraEvents} from './camera-events'
import {addChild} from './matrix-refresh'

const createCameraManager = (
  world: World,
  audioListener: AudioListener,
  events: Events
): CameraManager => {
  let activeCameraEid_: Eid = 0n
  let activeCamera_: CameraObject
  let attached_ = false
  const defaultCamera_ = world.three.activeCamera
  addChild(defaultCamera_, audioListener)

  const setActiveCamera = (cameraObject: CameraObject) => {
    if (activeCamera_ === cameraObject) {
      return
    }
    activeCamera_ = cameraObject
    addChild(cameraObject, audioListener)
    world.three.activeCamera = activeCamera_
    events.dispatch(events.globalId, CameraEvents.ACTIVE_CAMERA_CHANGE, {camera: cameraObject})
  }

  const getCameraObject = (eid: Eid): CameraObject | undefined => (
    world.three.entityToObject.get(eid)?.userData.camera
  )

  const setActiveEid = (eid: Eid) => {
    if (!eid) {
      throw new Error('eid is required')
    }
    if (activeCameraEid_ !== eid) {
      events.dispatch(events.globalId, CameraEvents.ACTIVE_CAMERA_EID_CHANGE, {eid})
    }
    activeCameraEid_ = eid
    const cameraObject = getCameraObject(eid)
    if (cameraObject) {
      setActiveCamera(cameraObject)
    }
  }

  const notifyCameraAdded = (eid: Eid) => {
    if (eid === activeCameraEid_ && attached_) {
      const cameraObject = getCameraObject(eid)
      if (!cameraObject) {
        throw new Error('camera does not exist')
      }
      setActiveCamera(cameraObject)
    }
  }

  const notifyCameraRemoved = (eid: Eid) => {
    if (eid === activeCameraEid_) {
      activeCameraEid_ = 0n
      if (attached_) {
        setActiveCamera(defaultCamera_)
      }
    }
  }

  const attach = () => {
    attached_ = true
    if (activeCameraEid_) {
      const cameraObject = getCameraObject(activeCameraEid_)
      if (cameraObject) {
        setActiveCamera(cameraObject)
      }
    }
  }

  const detach = () => {
    attached_ = false
    setActiveCamera(defaultCamera_)
  }

  const getActiveEid = () => activeCameraEid_

  return {
    getActiveEid,
    setActiveEid,
    notifyCameraAdded,
    notifyCameraRemoved,
    attach,
    detach,
  }
}

export {
  createCameraManager,
}
