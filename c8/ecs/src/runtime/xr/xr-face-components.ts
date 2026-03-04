import type {World} from '../world'
import {registerComponent} from '../registry'
import THREE from '../three'
import {Hidden, Face, Camera} from '../components'
import {createInstanced} from '../../shared/instanced'
import type {Eid} from '../../shared/schema'
import {mat4, vec3, quat} from '../math/math'
import {earAttachmentNames} from '../../shared/face-mesh-data'
import {XR_FACE_FOUND, XR_FACE_UPDATED, XR_FACE_LOST} from '../camera-events'
import type {QueuedEvent} from '../events'

type Cleanup = () => void
const cleanups = createInstanced<World, Map<Eid, Cleanup>>(() => new Map())

interface FaceData {
  id: number
  transform: {
    position: {x: number; y: number; z: number}
    rotation: {x: number; y: number; z: number; w: number}
    scale: number
    scaledWidth: number
    scaledHeight: number
    scaledDepth: number
  }
  vertices: {x: number, y: number, z: number}[]
  normals: {x: number, y: number, z: number}[]
  attachmentPoints: Record<string, {position: {x: number, y: number, z: number}}>
}

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
  vertexNormalData: new Float32Array(1),
  tempPos: new THREE.Vector3(),
  tempQuat: new THREE.Quaternion(),
}

const FaceAnchor = registerComponent({
  name: 'face-anchor',
  add: () => {
    // eslint-disable-next-line no-console
    console.warn('face-anchor component is deprecated and should be removed.')
  },
})

const FaceMeshAnchor = registerComponent({
  name: 'face-mesh-anchor',
  add: (world: World, component) => {
    const worldEid = world.events.globalId

    const show = (eid: Eid) => (event: QueuedEvent) => {
      const parentEid = world.getParent(eid)
      if (!Face.has(world, parentEid)) {
        return
      }

      const data = event.data as FaceData
      if (data.id && data.id !== Face.get(world, parentEid).id) {
        return
      }
      Hidden.remove(world, eid)

      const faceObj = world.three.entityToObject.get(eid)
      const {vertices} = data

      if (faceObj instanceof THREE.Mesh) {
        // re-allocate vertex position data if needed
        if (scratch.vertexPositionData.length !== vertices.length * 3) {
          scratch.vertexPositionData = new Float32Array(vertices.length * 3)
        }
        if (faceObj.geometry.getAttribute('position').array.length !== vertices.length * 3) {
          const meshVertices = new Float32Array(vertices.length * 3)
          faceObj.geometry.setAttribute('position', new THREE.BufferAttribute(meshVertices, 3))
        }

        for (let i = 0; i < vertices.length; ++i) {
          scratch.vertexPositionData[i * 3] = -vertices[i].x
          scratch.vertexPositionData[i * 3 + 1] = vertices[i].y
          scratch.vertexPositionData[i * 3 + 2] = -vertices[i].z
        }
        faceObj.geometry.getAttribute('position').copyArray(scratch.vertexPositionData)
        faceObj.geometry.attributes.position.needsUpdate = true
      }
    }

    const hide = (eid: Eid) => () => {
      Hidden.set(world, eid)
    }

    const showFunc = show(component.eid)
    const hideFunc = hide(component.eid)

    world.events.addListener(worldEid, XR_FACE_FOUND, showFunc)
    world.events.addListener(worldEid, XR_FACE_UPDATED, showFunc)
    world.events.addListener(worldEid, XR_FACE_LOST, hideFunc)

    const cleanup = () => {
      world.events.removeListener(worldEid, XR_FACE_FOUND, showFunc)
      world.events.removeListener(worldEid, XR_FACE_UPDATED, showFunc)
      world.events.removeListener(worldEid, XR_FACE_LOST, hideFunc)
    }
    cleanups(world).set(component.eid, cleanup)
  },
  remove: (world: World, component) => {
    cleanups(world).get(component.eid)?.()
    cleanups(world).delete(component.eid)
  },
})

const FaceAttachment = registerComponent({
  name: 'face-attachment',
  schema: {
    // eslint-disable-next-line max-len
    // @enum forehead, rightEyebrowInner, rightEyebrowMiddle, rightEyebrowOuter, leftEyebrowInner, leftEyebrowMiddle, leftEyebrowOuter, leftCheek, rightCheek, noseBridge, noseTip, leftEye, rightEye, leftEyeOuterCorner, rightEyeOuterCorner, upperLip, lowerLip, mouth, mouthRightCorner, mouthLeftCorner, chin, leftIris, rightIris, leftUpperEyelid, rightUpperEyelid, leftLowerEyelid, rightLowerEyelid, leftHelix, leftCanal, leftLobe, rightHelix, rightCanal, rightLobe
    point: 'string',
  },
  schemaDefaults: {
    point: 'forehead',
  },
  add: (world, component) => {
    const {schema} = component
    const camEid = world.camera.getActiveEid()
    const worldEid = world.events.globalId

    if (!Camera.get(world, camEid).enableEars && earAttachmentNames.includes(schema.point)) {
      Hidden.set(world, component.eid)
      return
    }

    const show = (point: string, eid: Eid) => (event: QueuedEvent) => {
      const parentEid = world.getParent(eid)
      if (!Face.has(world, parentEid)) {
        return
      }

      const data = event.data as FaceData
      if (data.id && data.id !== Face.get(world, parentEid).id) {
        return
      }
      Hidden.remove(world, eid)

      const position = data.attachmentPoints[point]?.position

      if (!position) {
        return
      }
      world.setPosition(eid, -position.x, position.y, -position.z)
    }

    const hide = (eid: Eid) => (event: QueuedEvent) => {
      const parentEid = world.getParent(eid)
      if (!Face.has(world, parentEid)) {
        return
      }

      const data = event.data as FaceData
      if (data.id && data.id !== Face.get(world, parentEid).id) {
        return
      }
      Hidden.set(world, eid)
    }

    const showFunc = show(schema.point, component.eid)
    const hideFunc = hide(component.eid)

    world.events.addListener(worldEid, XR_FACE_FOUND, showFunc)
    world.events.addListener(worldEid, XR_FACE_UPDATED, showFunc)
    world.events.addListener(worldEid, XR_FACE_LOST, hideFunc)

    const cleanup = () => {
      world.events.removeListener(worldEid, XR_FACE_FOUND, showFunc)
      world.events.removeListener(worldEid, XR_FACE_UPDATED, showFunc)
      world.events.removeListener(worldEid, XR_FACE_LOST, hideFunc)
    }
    cleanups(world).set(component.eid, cleanup)
  },
  remove: (world, component) => {
    cleanups(world).get(component.eid)?.()
    cleanups(world).delete(component.eid)
  },
})

export {
  FaceAnchor,
  FaceMeshAnchor,
  FaceAttachment,
}
