import type {World} from '../world'
import type {QueuedEvent} from '../events'
import {Hidden, Face, FaceGeometry, Camera} from '../components'
import {makeSystemHelper} from './system-helper'
import {CameraEvents} from '../camera-events'
import {mat4, vec3, quat} from '../math/math'
import THREE from '../three'
import type {Eid} from '../../shared/schema'
import {
  FaceMeshPositions, FaceMeshNormals, FaceMeshUvs, FaceMeshIndices, FaceMeshEyeIndices,
  FaceMeshIrisIndices, FaceMeshMouthIndices,
} from '../../shared/face-mesh-data'
import {events} from '../event-ids'

interface FaceData {
  id: number
  transform: {
    position: {x: number; y: number; z: number}
    rotation: {x: number; y: number; z: number; w: number}
    scale: number
  }
}

interface CameraData {
  camera: {
    userData: {
      xr: {
        xrCameraType: string
        mirroredDisplay: boolean
        meshGeometryFace: boolean
        meshGeometryEyes: boolean
        meshGeometryIris: boolean
        meshGeometryMouth: boolean
      }
    }
  }
}
const {XR_CAMERA_STOP, XR_CAMERA_EDIT} = CameraEvents

const scratch = {
  rotY180: quat.yDegrees(180),
  cameraTransform: mat4.i(),
  cameraPos: vec3.zero(),
  cameraRot: quat.zero(),
  cameraScale: vec3.zero(),
  engineRot: quat.zero(),
  meshPos: vec3.zero(),
  meshRot: quat.zero(),
  vertexPositionData: new Float32Array(1),
  tempPos: new THREE.Vector3(),
  tempQuat: new THREE.Quaternion(),
}

const makeFaceSystem = (world: World) => {
  const {enter: faceEnter, changed: faceChanged, exit: faceExit} = makeSystemHelper(Face)
  const {
    enter: geometryEnter,
    changed: geometryChanged,
    exit: geometryExit,
  } = makeSystemHelper(FaceGeometry)
  const listenerCleanup: Map<Eid, () => void> = new Map()
  const emptyGeometry = new THREE.BufferGeometry()

  const collectFaceMeshIndices = (
    meshGeometryFace: boolean,
    meshGeometryEyes: boolean,
    meshGeometryIris: boolean,
    meshGeometryMouth: boolean,
    mirroredDisplay: boolean
  ): number[] => {
    const numIndices =
        (meshGeometryFace ? FaceMeshIndices.length : 0) +
        (meshGeometryEyes ? FaceMeshEyeIndices.length : 0) +
        (meshGeometryIris ? FaceMeshIrisIndices.length : 0) +
        (meshGeometryMouth ? FaceMeshMouthIndices.length : 0)
    const indices = new Array(numIndices)

    let i = 0
    if (meshGeometryFace) {
      for (let j = 0; i < FaceMeshIndices.length; j += 3) {
        indices[i] = FaceMeshIndices[j]
        indices[i + 1] = mirroredDisplay ? FaceMeshIndices[j + 1] : FaceMeshIndices[j + 2]
        indices[i + 2] = mirroredDisplay ? FaceMeshIndices[j + 2] : FaceMeshIndices[j + 1]
        i += 3
      }
    }
    if (meshGeometryEyes) {
      for (let j = 0; j < FaceMeshEyeIndices.length; j += 3) {
        indices[i] = FaceMeshEyeIndices[j]
        indices[i + 1] = mirroredDisplay ? FaceMeshEyeIndices[j + 1] : FaceMeshEyeIndices[j + 2]
        indices[i + 2] = mirroredDisplay ? FaceMeshEyeIndices[j + 2] : FaceMeshEyeIndices[j + 1]
        i += 3
      }
    }
    if (meshGeometryIris) {
      for (let j = 0; j < FaceMeshIrisIndices.length; j += 3) {
        indices[i] = FaceMeshIrisIndices[j]
        indices[i + 1] = mirroredDisplay ? FaceMeshIrisIndices[j + 1] : FaceMeshIrisIndices[j + 2]
        indices[i + 2] = mirroredDisplay ? FaceMeshIrisIndices[j + 2] : FaceMeshIrisIndices[j + 1]
        i += 3
      }
    }
    if (meshGeometryMouth) {
      for (let j = 0; j < FaceMeshMouthIndices.length; j += 3) {
        indices[i] = FaceMeshMouthIndices[j]
        indices[i + 1] = mirroredDisplay ? FaceMeshMouthIndices[j + 1] : FaceMeshMouthIndices[j + 2]
        indices[i + 2] = mirroredDisplay ? FaceMeshMouthIndices[j + 2] : FaceMeshMouthIndices[j + 1]
        i += 3
      }
    }

    return indices
  }

  const applyGeometry = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    let {
      mirroredDisplay, meshGeometryFace, meshGeometryEyes, meshGeometryIris, meshGeometryMouth,
    } = Camera.get(world, world.camera.getActiveEid())

    if (object instanceof THREE.Mesh) {
      const indices = collectFaceMeshIndices(
        meshGeometryFace,
        meshGeometryEyes,
        meshGeometryIris,
        meshGeometryMouth,
        mirroredDisplay
      )
      const positions = new Float32Array(FaceMeshPositions.length)

      object.geometry = new THREE.BufferGeometry()
      object.geometry.setIndex(indices)
      object.geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3))
      object.geometry.setAttribute('normal', new THREE.BufferAttribute(FaceMeshNormals, 3))
      object.geometry.setAttribute('uv', new THREE.BufferAttribute(FaceMeshUvs, 2))

      object.geometry.computeBoundsTree()
    }

    const handleCameraEdit = (e: QueuedEvent) => {
      const {camera} = e.data as CameraData
      const {
        xrCameraType,
        mirroredDisplay: newMirroredDisplay,
        meshGeometryFace: newMeshGeometryFace,
        meshGeometryEyes: newMeshGeometryEyes,
        meshGeometryIris: newMeshGeometryIris,
        meshGeometryMouth: newMeshGeometryMouth,
      } = camera.userData.xr

      // Update the mesh geometry only if the camera type is 'face' and either one of the following
      // flags has changed: mirroredDisplay, meshGeometryFace, meshGeometryEyes, meshGeometryIris,
      // and meshGeometryMouth.
      if (xrCameraType !== 'face' || (
        mirroredDisplay === newMirroredDisplay &&
        meshGeometryFace === newMeshGeometryFace &&
        meshGeometryEyes === newMeshGeometryEyes &&
        meshGeometryIris === newMeshGeometryIris &&
        meshGeometryMouth === newMeshGeometryMouth
      )) {
        return
      }

      mirroredDisplay = newMirroredDisplay
      meshGeometryFace = newMeshGeometryFace
      meshGeometryEyes = newMeshGeometryEyes
      meshGeometryIris = newMeshGeometryIris
      meshGeometryMouth = newMeshGeometryMouth
      if (object instanceof THREE.Mesh) {
        const indices = collectFaceMeshIndices(
          meshGeometryFace,
          meshGeometryEyes,
          meshGeometryIris,
          meshGeometryMouth,
          mirroredDisplay
        )

        object.geometry.setIndex(indices)
        object.geometry.index.needsUpdate = true
      }
    }

    world.events.addListener(world.events.globalId, XR_CAMERA_EDIT, handleCameraEdit)

    listenerCleanup.set(eid, () => {
      world.events.removeListener(world.events.globalId, XR_CAMERA_EDIT, handleCameraEdit)
    })
  }

  const removeGeometry = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh) {
      // TODO(christoph): Technically switching from one geometry to another would
      // be clobbered here
      object.geometry = emptyGeometry
    }

    listenerCleanup.get(eid)?.()
    listenerCleanup.delete(eid)
  }

  const setUpFaceListeners = (eid: Eid) => {
    // Shows the face if face is found or updated, and updates its position.
    const show = (e: QueuedEvent) => {
      if (!Face.has(world, eid)) {
        return
      }

      const data = e.data as FaceData
      const {id, transform} = data
      const {position, rotation, scale} = transform

      if (Face.get(world, eid).id !== id) {
        return
      }

      Hidden.remove(world, eid)

      world.getWorldTransform(world.camera.getActiveEid(), scratch.cameraTransform)
      scratch.cameraTransform.decomposeTrs({
        t: scratch.cameraPos, r: scratch.cameraRot, s: scratch.cameraScale,
      })

      scratch.tempPos.set(position.x, position.y, position.z)
      scratch.cameraRot = scratch.cameraRot.times(scratch.rotY180)
      scratch.tempQuat.set(scratch.cameraRot.x, scratch.cameraRot.y, scratch.cameraRot.z,
        scratch.cameraRot.w)
      scratch.tempPos.applyQuaternion(scratch.tempQuat)

      // shift data from the face controller by the mesh's position
      world.setPosition(
        eid,
        scratch.cameraPos.x + scratch.tempPos.x,
        scratch.cameraPos.y + scratch.tempPos.y,
        scratch.cameraPos.z + scratch.tempPos.z
      )

      // local rotation
      scratch.engineRot.setXyzw(scratch.cameraRot.x, scratch.cameraRot.y, scratch.cameraRot.z,
        scratch.cameraRot.w)
      scratch.meshRot = scratch.engineRot.times(rotation).times(scratch.rotY180)
      world.setQuaternion(
        eid,
        scratch.meshRot.x,
        scratch.meshRot.y,
        scratch.meshRot.z,
        scratch.meshRot.w
      )

      // scale is left as is
      world.setScale(eid, scale, scale, scale)
    }

    const hide = (e: QueuedEvent) => {
      if (!Face.has(world, eid)) {
        return
      }

      const {id} = e.data as FaceData
      if (Face.get(world, eid).id !== id) {
        return
      }

      Hidden.set(world, eid)
    }

    const handleXrCameraStop = () => {
      Hidden.set(world, eid)
    }

    world.events.addListener(world.events.globalId, events.FACE_FOUND, show)
    world.events.addListener(world.events.globalId, events.FACE_UPDATED, show)
    world.events.addListener(world.events.globalId, events.FACE_LOST, hide)
    world.events.addListener(world.events.globalId, XR_CAMERA_STOP, handleXrCameraStop)
    listenerCleanup.set(eid, () => {
      world.events.removeListener(world.events.globalId, events.FACE_FOUND, show)
      world.events.removeListener(world.events.globalId, events.FACE_UPDATED, show)
      world.events.removeListener(world.events.globalId, events.FACE_LOST, hide)
      world.events.removeListener(
        world.events.globalId,
        XR_CAMERA_STOP,
        handleXrCameraStop
      )
    })
  }

  const addFaceAnchor = (eid: Eid) => {
    setUpFaceListeners(eid)

    // Hide face by default.
    Hidden.set(world, eid)
  }

  // Hides face if face id was changed.
  const editFaceAnchor = (eid: Eid) => {
    Hidden.set(world, eid)
  }

  const removeFaceAnchor = (eid: Eid) => {
    listenerCleanup.get(eid)?.()
    listenerCleanup.delete(eid)
  }

  return () => {
    faceEnter(world).forEach(addFaceAnchor)
    faceExit(world).forEach(removeFaceAnchor)
    faceChanged(world).forEach(editFaceAnchor)
    geometryExit(world).forEach(removeGeometry)
    geometryEnter(world).forEach(applyGeometry)
    geometryChanged(world).forEach(applyGeometry)
  }
}

export {makeFaceSystem}
