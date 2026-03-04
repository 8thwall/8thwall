import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import type {QueuedEvent} from '../events'
import {Hidden, ImageTarget} from '../components'
import {makeSystemHelper} from './system-helper'
import {CameraEvents} from '../camera-events'
import {vec3, mat4} from '../math/math'
import {events} from '../event-ids'

interface ImageTargetData {
  name: string
  position: {x: number; y: number; z: number}
  rotation: {x: number; y: number; z: number; w: number}
  scale: number
}

const TEMP_SCALE = vec3.zero()
const TEMP_TRANSFORM = mat4.i()

const makeImageTargetSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(ImageTarget)
  const listenerCleanup: Map<Eid, () => void> = new Map()

  const setUpImageListeners = (eid: Eid) => {
    // Shows the image target if the image target is found or updated, and updates its position.
    const show = (e: QueuedEvent) => {
      if (!ImageTarget.has(world, eid)) {
        return
      }
      const data = e.data as ImageTargetData
      const {position, rotation, scale} = data
      const detectedName = data.name
      const targetName = ImageTarget.get(world, eid).name

      if (detectedName !== targetName) {
        return
      }

      Hidden.remove(world, eid)

      // note(owenmech): setting world transform is more expensive, only do when necessary
      if (world.getParent(eid)) {
        TEMP_SCALE.makeScale(scale)
        TEMP_TRANSFORM.makeTrs(position, rotation, TEMP_SCALE)
        world.transform.setWorldTransform(eid, TEMP_TRANSFORM)
      } else {
        world.setPosition(eid, position.x, position.y, position.z)
        world.setQuaternion(eid, rotation.x, rotation.y, rotation.z, rotation.w)
        world.setScale(eid, scale, scale, scale)
      }
    }

    // Hides the image target if the image target is lost.
    const hide = (e: QueuedEvent) => {
      if (!ImageTarget.has(world, eid)) {
        return
      }
      const data = e.data as ImageTargetData
      const detectedName = data.name
      const targetName = ImageTarget.get(world, eid).name

      if (detectedName !== targetName) {
        return
      }

      Hidden.set(world, eid)
    }

    // Hide image target if XR camera is stopped.
    const handleXrCameraStop = () => {
      Hidden.set(world, eid)
    }

    // Listen for image target events bubbled up to the global id.
    world.events.addListener(world.events.globalId, events.REALITY_IMAGE_FOUND, show)
    world.events.addListener(world.events.globalId, events.REALITY_IMAGE_UPDATED, show)
    world.events.addListener(world.events.globalId, events.REALITY_IMAGE_LOST, hide)
    world.events.addListener(world.events.globalId, CameraEvents.XR_CAMERA_STOP, handleXrCameraStop)
    listenerCleanup.set(eid, () => {
      world.events.removeListener(world.events.globalId, events.REALITY_IMAGE_FOUND, show)
      world.events.removeListener(world.events.globalId, events.REALITY_IMAGE_UPDATED, show)
      world.events.removeListener(world.events.globalId, events.REALITY_IMAGE_LOST, hide)
      world.events.removeListener(
        world.events.globalId,
        CameraEvents.XR_CAMERA_STOP,
        handleXrCameraStop
      )
    })
  }

  // Set up listeners for image targets when they are added.
  const addImageTarget = (eid: Eid) => {
    setUpImageListeners(eid)

    // Hide the image target by default.
    Hidden.set(world, eid)
  }

  // Hide the image target if it is edited. It will show again if it is updated or found again.
  const editImageTarget = (eid: Eid) => {
    Hidden.set(world, eid)
  }

  // Remove listeners for image targets when they are removed.
  const removeImageTarget = (eid: Eid) => {
    listenerCleanup.get(eid)?.()
    listenerCleanup.delete(eid)
  }

  return () => {
    enter(world).forEach(addImageTarget)
    exit(world).forEach(removeImageTarget)
    changed(world).forEach(editImageTarget)
  }
}

export {makeImageTargetSystem}
