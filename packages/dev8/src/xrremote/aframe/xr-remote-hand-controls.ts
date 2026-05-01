import CONTROLLER_EVENTS from '../constants/controllerEvents'
import type XRRemote from '../xrremote-core'

const questV2Left =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-2-left-lw9imr66.glb'
const questV2Right =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-2-right-lw9imrel.glb'
const questV3Left =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-3-left-lw9imr9k.glb'
const questV3Right =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-3-right-lw9imr7w.glb'
const questTouchProLeft =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-pro-left-lw9imqzk.glb'
const questTouchProRight =
  'https://cdn.8thwall.com/web/assets/models/controllers/meta-quest-pro-right-lw9imrba.glb'

let _registered = false
const _quaternion = new window.THREE.Quaternion()

const _vec3 = new window.THREE.Vector3()
const _vec3ForControllerOrigin = new window.THREE.Vector3()
const _vec3ForHandPosition = new window.THREE.Vector3()

const CONTROLLER_TYPE = {
  META_QUEST_2: 'meta-quest-2',
  META_QUEST_3: 'meta-quest-3',
  META_QUEST_PRO: 'meta-quest-pro',
}

// https://github.com/aframevr/aframe/blob/master/src/components/oculus-touch-controls.js
const CONTROLLER_MODEL_MAPPING = {
  [CONTROLLER_TYPE.META_QUEST_2]: {
    left: {
      modelUrl: questV2Left,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
    right: {
      modelUrl: questV2Right,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
  },
  [CONTROLLER_TYPE.META_QUEST_3]: {
    left: {
      modelUrl: questV3Left,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
    right: {
      modelUrl: questV3Right,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
  },
  [CONTROLLER_TYPE.META_QUEST_PRO]: {
    left: {
      modelUrl: questTouchProLeft,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
    right: {
      modelUrl: questTouchProRight,
      modelPivotOffset: new window.THREE.Vector3(0, 0, 0),
      modelPivotRotation: new window.THREE.Euler(0, 0, 0),
    },
  },
}

// Position offset is moving controllers to a position
// in front of the camera.
// TODO: Rotation is unused for now, but potentially if we can figure out
// how to counteract the headset's current pointing direction, then we can
// define how its oriented default.
const UNUSED_QUATERNION_OFFSET = {
  x: 0,
  y: 0,
  z: 0,
  w: 1,
}
const DEFAULT_OFFSETS = {
  left: {
    position: {
      x: -0.25,
      y: -0.5,
      z: -1,
    },
    quaternion: UNUSED_QUATERNION_OFFSET,
  },
  right: {
    position: {
      x: 0.25,
      y: -0.5,
      z: -1,
    },
    quaternion: UNUSED_QUATERNION_OFFSET,
  },
}

const computeHandPosition = (
  camera,
  position,
  rotation,
  offsets,
  positionScaling = {x: 1, y: 1, z: 1}
) => {
  const cameraPosition = camera.position
  const cameraOrientation = camera.quaternion

  // Position the controller such that it is below and in front of the camera
  // Also apply scaling to the positions so that developers that want to move farther
  // with less physical movement can do that.
  const handPosition = _vec3ForHandPosition.set(
    position.x,
    position.y,
    position.z
  )

  _vec3.set(
    offsets.position.x,
    offsets.position.y,
    offsets.position.z
  )

  // Subtract the saved vector to get origin
  const vectorToLastSyncedOrigin = handPosition.add(_vec3)

  // Apply scaling and position controller such that it appears in front of the
  const vectorToDesiredCenteredPositions = vectorToLastSyncedOrigin.set(
    vectorToLastSyncedOrigin.x * positionScaling.x + offsets.relativePosition.x,
    vectorToLastSyncedOrigin.y * positionScaling.y + offsets.relativePosition.y,
    vectorToLastSyncedOrigin.z * positionScaling.z + offsets.relativePosition.z
  )
  const hmdVector = vectorToDesiredCenteredPositions
  const hmdVectorWithCameraRotation = hmdVector.applyQuaternion(cameraOrientation)
  const controllerPosition = cameraPosition.clone().add(hmdVectorWithCameraRotation)

  // TODO: Apply any offset rotation desired. Possibly to more seamlessly
  // handle recentering of hands based on the HMD's headset orientation.
  // Basically, controllers point in the direction of the HMD's last configured
  // headset direction, since you can look left, but hands still point in some direction
  // in the world. But long pressing the quest menu button will "recenter" the hmd,
  // which causes the headset direction to change.
  _quaternion.set(
    rotation.x,
    rotation.y,
    rotation.z,
    rotation.w
  )
  // Apply the camera's rotation so that controller points in same
  // direction as the camera.
  const controllerOrientation = _quaternion.premultiply(cameraOrientation)

  return {
    position: controllerPosition,
    quaternion: controllerOrientation,
  }
}
// Adjusts local position and orientation of hand model based on remote hand data.
// Adjustable scaling is applied for dev convenience (So you dont have to move as much)
const moveHands = (
  sceneEl,
  controllerType,
  localRight,
  localLeft,
  remoteRightHand,
  remoteLeftHand,
  offsets,
  positionScaling
) => {
  const right = remoteRightHand
  const left = remoteLeftHand

  const rightPosition = right.position
  const rightRotation = right.quaternion

  const leftPosition = left.position
  const leftRotation = left.quaternion

  const rightHandEntity = localRight
  const leftHandEntity = localLeft

  const offsetsPositionRight = offsets.right.position
  const offsetsPositionLeft = offsets.left.position
  const offsetsRotationRight = offsets.right.quaternion
  const offsetsRotationLeft = offsets.left.quaternion

  // If no camera is available yet, do nothing
  if (!sceneEl.camera) {
    return
  }

  const camera = sceneEl.camera?.el?.object3D

  if (rightHandEntity) {
    const rightOffsets = {
      relativePosition: DEFAULT_OFFSETS.right.position,
      position: offsetsPositionRight,
      quaternion: offsetsRotationRight,
    }
    const {position, quaternion} = computeHandPosition(
      camera,
      rightPosition,
      rightRotation,
      rightOffsets,
      positionScaling
    )

    rightHandEntity.object3D.position.set(
      position.x,
      position.y,
      position.z
    )
    rightHandEntity.object3D.rotation.setFromQuaternion(quaternion)
    rightHandEntity.object3D.visible = true
  }

  if (leftHandEntity) {
    const leftOffsets = {
      relativePosition: DEFAULT_OFFSETS.left.position,
      position: offsetsPositionLeft,
      quaternion: offsetsRotationLeft,
    }
    const {position, quaternion} = computeHandPosition(
      camera,
      leftPosition,
      leftRotation,
      leftOffsets,
      positionScaling
    )

    leftHandEntity.object3D.position.set(
      position.x,
      position.y,
      position.z
    )
    leftHandEntity.object3D.rotation.setFromQuaternion(quaternion)
    leftHandEntity.object3D.visible = true
  }
}

const computeControllerOffsets = (xrRemoteInstance) => {
  // Compute a vector that negates the last known hmd controller origin so that
  // we can recenter controllers independently from the real raw input position.
  const hmdControllerOriginRightPosition = xrRemoteInstance.state.inputSource.origin.right.position
  const hmdControllerOriginRightAsVec3 = _vec3ForControllerOrigin.set(
    hmdControllerOriginRightPosition.x,
    hmdControllerOriginRightPosition.y,
    hmdControllerOriginRightPosition.z
  )

  // This is the vector that will negate the position from the perspective of the HMD
  // So that we can position controllers where the scene camera is.
  const offsetRightVec3 = hmdControllerOriginRightAsVec3.negate()
  const offsetRight = {
    x: offsetRightVec3.x,
    y: offsetRightVec3.y,
    z: offsetRightVec3.z,
  }

  const hmdControllerOriginLeftPosition = xrRemoteInstance.state.inputSource.origin.left.position
  const hmdControllerOriginLeftAsVec3 = _vec3ForControllerOrigin.set(
    hmdControllerOriginLeftPosition.x,
    hmdControllerOriginLeftPosition.y,
    hmdControllerOriginLeftPosition.z
  )
  const offsetLeftVec3 = hmdControllerOriginLeftAsVec3.negate()
  const offsetLeft = {
    x: offsetLeftVec3.x,
    y: offsetLeftVec3.y,
    z: offsetLeftVec3.z,
  }
  // Now that we have the offsets that we should apply to the camera,
  // we can position the controller entities and reposition them such that
  // they "reset" back to a position.
  const offsets = {
    right: {
      position: offsetRight,
      quaternion: UNUSED_QUATERNION_OFFSET,
    },
    left: {
      position: offsetLeft,
      quaternion: UNUSED_QUATERNION_OFFSET,
    },
  }

  return offsets
}

const register = (xrRemoteInstance: XRRemote) => {
  if (_registered) {
    return
  }

  if (!window.AFRAME) {
    return
  }

  // Handles position updates
  window.AFRAME.registerSystem('xr-remote-hand-controls', {
    schema: {
      scalePositionsBy: {type: 'string', default: '1.0 1.0 1.0'},
      leftHandSelector: {type: 'string', default: '#leftHand'},
      rightHandSelector: {type: 'string', default: '#rightHand'},
    },
    init() {
      const {controllerType, connected} = xrRemoteInstance.state.inputSource

      this.controllerType = controllerType || CONTROLLER_TYPE.META_QUEST_3
      this.connected = connected || false
      this.handlersToRemove = []

      // Attach handlers for controller button presses
      CONTROLLER_EVENTS.forEach((controllerEvent) => {
        const handler = (event) => {
          const {hand, remoteEventName, remoteEventDetail} = event.detail

          if (hand === 'right') {
            const rightHandEntity = this.el.querySelector(this.data.rightHandSelector)
            rightHandEntity.emit(remoteEventName, remoteEventDetail)
          } else if (hand === 'left') {
            const leftHandEntity = this.el.querySelector(this.data.leftHandSelector)
            leftHandEntity.emit(remoteEventName, remoteEventDetail)
          }
        }

        this.handlersToRemove.push({event: controllerEvent, handler})
        xrRemoteInstance.controller.addEventListener(controllerEvent, handler)
      })

      // Attach handler for controller position
      this.updateHandPositions = (event) => {
        const handPositions = event.detail
        const rightHandEntity = this.el.querySelector(this.data.rightHandSelector)
        const leftHandEntity = this.el.querySelector(this.data.leftHandSelector)
        const scaleArr = this.data.scalePositionsBy.split(' ').map(n => parseFloat(n))
        const positionScaling = {
          x: scaleArr[0] || 1.0,
          y: scaleArr[1] || 1.0,
          z: scaleArr[2] || 1.0,
        }

        // Store the positions of the controllers
        // This will be used as the "origin" so we can recenter controllers
        // for the user regardless of where the controllers are located
        // with respect to the headset
        if (xrRemoteInstance.originNeedsSync()) {
          xrRemoteInstance.updateControllerOrigin(
            {
              position: handPositions.left.position,
              quaternion: handPositions.left.quaternion,
            },
            {
              position: handPositions.right.position,
              quaternion: handPositions.right.quaternion,
            }
          )
        }

        const offsets = computeControllerOffsets(xrRemoteInstance)

        moveHands(
          this.el.sceneEl,
          this.controllerType || CONTROLLER_TYPE.META_QUEST_3,
          rightHandEntity,
          leftHandEntity,
          handPositions.right,
          handPositions.left,
          offsets,
          positionScaling
        )
      }

      xrRemoteInstance.controllerPosition.addEventListener(
        'controller-position-update',
        this.updateHandPositions
      )
    },
    remove() {
      this.handlersToRemove.forEach((handlerObj) => {
        xrRemoteInstance.controller.removeEventListener(handlerObj.event, handlerObj.handler)
      })

      xrRemoteInstance.controllerPosition.removeEventListener(
        'controller-position-update',
        this.updateHandPositions
      )
    },
    tick() {
      if (this.controllerType !== xrRemoteInstance.state.inputSource.controllerType) {
        this.controllerType = xrRemoteInstance.state.inputSource.controllerType
        this.el.sceneEl.emit('remote-controller-type-changed')
      }

      if (this.connected !== xrRemoteInstance.state.inputSource.connected) {
        if (xrRemoteInstance.state.inputSource.connected) {
          this.el.sceneEl.emit('controllers-connected')
        } else {
          this.el.sceneEl.emit('controllers-disconnected')
        }
        this.connected = xrRemoteInstance.state.inputSource.connected
      }
    },
  })

  window.AFRAME.registerComponent('xr-remote-hand-controls', {
    schema: {
      hand: {type: 'string', default: 'left', oneOf: ['left', 'right']},
      autoHide: {type: 'boolean', default: true},
    },
    init() {
      const handleControllerTypeChange = this.updateControllerModel.bind(this)
      const handleControllerConnected = this.showController.bind(this)
      const handleControllerDisconnected = this.hideController.bind(this)
      const handleModelLoaded = this.onModelLoaded.bind(this)

      this.updateControllerModel()
      this.el.sceneEl.addEventListener('remote-controller-type-changed', handleControllerTypeChange)
      this.el.sceneEl.addEventListener('controllers-connected', handleControllerConnected)
      this.el.sceneEl.addEventListener('controllers-disconnected', handleControllerDisconnected)
      this.el.addEventListener('model-loaded', handleModelLoaded)
      this.el.object3D.visible = !this.data.autoHide

      this.cleanup = () => {
        this.el.sceneEl.removeEventListener(
          'remote-controller-type-changed',
          handleControllerTypeChange
        )
        this.el.sceneEl.removeEventListener(
          'controllers-connected',
          handleControllerConnected
        )
        this.el.sceneEl.removeEventListener(
          'controllers-disconnected',
          handleControllerDisconnected
        )
        this.el.removeEventListener('model-loaded', handleModelLoaded)
        this.el.object3D.visible = false
      }
    },
    remove() {
      this.cleanup()
    },
    onModelLoaded() {
      const models = CONTROLLER_MODEL_MAPPING[
        this.system.controllerType || CONTROLLER_TYPE.META_QUEST_3
      ]
      const mesh = this.el.getObject3D('mesh')

      // Since models are jacked from AFrame, they need to be slightly rotated
      // so they are oriented correctly
      if (mesh) {
        const pivotOffsetRight = models[this.data.hand].modelPivotOffset
        const pivotRotationRight = models[this.data.hand].modelPivotRotation

        mesh.position.copy(pivotOffsetRight)
        mesh.rotation.copy(pivotRotationRight)
      }
    },
    updateControllerModel() {
      const models = CONTROLLER_MODEL_MAPPING[
        this.system.controllerType || CONTROLLER_TYPE.META_QUEST_3
      ]
      const handModelUrl = models[this.data.hand].modelUrl

      this.el.setAttribute('gltf-model', handModelUrl)
    },
    showController() {
      this.el.object3D.visible = true
    },
    hideController() {
      this.el.object3D.visible = false
    },
  })

  _registered = true
}

export {
  register,
}
