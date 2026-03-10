const POSITION_RADIUS = 1.25
const POSITION_OFFSET_Y = 0.25  // Center the alignment sphere slightly below the head

const FOLLOW_RADIUS = 0.25             // Movement distance that would start a reset
const FOLLOW_ANGLE = Math.PI * 0.75    // Spin angle that would start a reset
const FOLLOW_CHECK_CADENCE = 1000      // Periodically check instead of on every frame
const FOLLOW_DELAY = 2000              // Wait before resetting in case user moves back
const FOLLOW_MOVEMENT_THRESHOLD = 0.2  // Threshold for user to be considered "in motion"
const FOLLOW_LERP_END = 0.995          // Need a stopping point because of Zeno's paradox
const FOLLOW_LERP_AMOUNT = 0.2

const lerp = (from, to, amount) => from * (1 - amount) + to * amount
const lerpProperty = (property, amount) => lerp(property.from, property.to, amount)

const YXZ = 'YXZ'

const createDomTabletAlignment = ({camera, controllersGroup}) => {
  const {THREE} = window
  const attachmentPoint_ = new THREE.Group()

  const controllerRay_ = new THREE.Ray()

  const inverseControllerSpace_ = new THREE.Matrix4()
  const cameraWorldPosition_ = new THREE.Vector3()
  const cameraLocalPosition_ = new THREE.Vector3()
  const currentCameraTransform_ = new THREE.Matrix4()

  const homeCameraTransform_ = new THREE.Matrix4()
  const previousCameraPosition_ = new THREE.Vector3()

  const dragOffsetQuaternion_ = new THREE.Quaternion()

  let lastCheckedFollowAt_ = 0
  let exitedRadiusAt_ = 0
  let dragging_ = false
  let following_ = false
  let followAmount_ = 0
  let paused_ = false

  // The follow path defines an animation where the tablet starts in a given offset from an origin,
  // the moves towards a new offset with new origin. The offset rotates around the origin point
  // as the origin point animates from start to finish. This allows the tablet to rotate around
  // the user in the case where the origin isn't moving, or translate along the origin, or a
  // hybrid.
  const followPath_ = {
    origin: {from: new THREE.Vector3(), to: new THREE.Vector3()},
    rotationY: {from: 0, to: 0},
    rotationX: {from: 0, to: 0},
    distance: {from: 1, to: 1},
  }

  const getFollowPosition = (() => {
    const tempEuler = new THREE.Euler()
    const tempOffset = new THREE.Vector3()

    return (amount, targetVector) => {
      const distance = lerpProperty(followPath_.distance, amount)
      const rotationY = lerpProperty(followPath_.rotationY, amount)
      const rotationX = lerpProperty(followPath_.rotationX, amount)

      tempEuler.set(rotationX, rotationY, 0, YXZ)

      tempOffset.set(0, 0, 1)
      tempOffset.applyEuler(tempEuler)
      tempOffset.multiplyScalar(distance)

      targetVector.copy(followPath_.origin.from).lerp(followPath_.origin.to, amount)
      targetVector.add(tempOffset)

      return targetVector
    }
  })()

  const startFollowPath = (() => {
    const identity = new THREE.Vector3(0, 0, 1)
    const tempQuaternion = new THREE.Quaternion()
    const tempEuler = new THREE.Euler()
    const tempVector = new THREE.Vector3()

    return (goalPosition) => {
      followPath_.origin.from.setFromMatrixPosition(homeCameraTransform_)

      tempVector.copy(attachmentPoint_.position).sub(followPath_.origin.from)
      followPath_.distance.from = tempVector.length()

      tempVector.multiplyScalar(1 / followPath_.distance.from)
      tempQuaternion.setFromUnitVectors(identity, tempVector)
      tempEuler.setFromQuaternion(tempQuaternion, YXZ)
      followPath_.rotationX.from = tempEuler.x
      followPath_.rotationY.from = tempEuler.y

      followPath_.origin.to.setFromMatrixPosition(currentCameraTransform_)

      tempVector.copy(goalPosition).sub(followPath_.origin.to)
      followPath_.distance.to = tempVector.length()

      tempVector.multiplyScalar(1 / followPath_.distance.to)
      tempQuaternion.setFromUnitVectors(identity, tempVector)
      tempEuler.setFromQuaternion(tempQuaternion, YXZ)
      followPath_.rotationX.to = tempEuler.x
      followPath_.rotationY.to = tempEuler.y

      const rotationChange = followPath_.rotationY.to - followPath_.rotationY.from
      // Make sure we rotate from 1 to -1 degrees instead of 1 to 359
      if (rotationChange > Math.PI) {
        followPath_.rotationY.to -= Math.PI * 2
      } else if (rotationChange < -Math.PI) {
        followPath_.rotationY.to += Math.PI * 2
      }

      following_ = true
      followAmount_ = 0
    }
  })()

  const updateTransforms = () => {
    inverseControllerSpace_.copy(controllersGroup.matrixWorld).invert()
    cameraWorldPosition_.setFromMatrixPosition(camera.matrixWorld)
    cameraLocalPosition_.copy(cameraWorldPosition_)
    cameraLocalPosition_.applyMatrix4(inverseControllerSpace_)
  }

  // Taking the current camera position, get a matrix in controller space that represents the
  // camera's position and y rotation, ignoring x/y rotation and scale, giving us an unscaled matrix
  const extractCameraReferenceTransform = (() => {
    const tempPosition = new THREE.Vector3()
    const tempQuaternion = new THREE.Quaternion()
    const tempEuler = new THREE.Euler()
    const identityScale = new THREE.Vector3(1, 1, 1)

    return (targetMatrix) => {
      targetMatrix.copy(camera.matrixWorld)
      targetMatrix.premultiply(inverseControllerSpace_)

      tempPosition.setFromMatrixPosition(targetMatrix)

      tempEuler.setFromRotationMatrix(targetMatrix, YXZ)
      tempEuler.x = 0
      tempEuler.z = 0
      tempQuaternion.setFromEuler(tempEuler)

      targetMatrix.compose(tempPosition, tempQuaternion, identityScale)
    }
  })()

  const saveCameraHome = () => {
    extractCameraReferenceTransform(homeCameraTransform_)
  }

  const resetToFront = () => {
    updateTransforms()
    saveCameraHome()

    const cameraForwardRay = new THREE.Ray()
    cameraForwardRay.applyMatrix4(homeCameraTransform_)

    const cameraQuaternion = new THREE.Quaternion()
    cameraQuaternion.setFromUnitVectors(new THREE.Vector3(0, 0, 1), cameraForwardRay.direction)

    const cameraRotation = new THREE.Euler()
    cameraRotation.setFromQuaternion(cameraQuaternion, YXZ)
    cameraRotation.z = 0
    cameraRotation.x = -Math.PI / 6

    cameraForwardRay.origin.y = cameraLocalPosition_.y - POSITION_OFFSET_Y

    cameraForwardRay.direction.set(0, 0, 1)
    cameraForwardRay.direction.applyEuler(cameraRotation)

    cameraForwardRay.at(POSITION_RADIUS, attachmentPoint_.position)
    attachmentPoint_.lookAt(cameraWorldPosition_)
  }

  const checkDidExitHomeZone = (() => {
    const tempEuler = new THREE.Euler()
    const tempVector = new THREE.Vector3()

    return () => {
      tempVector.setFromMatrixPosition(homeCameraTransform_)
      tempVector.sub(cameraLocalPosition_)
      tempVector.y = 0
      const distanceSq = tempVector.lengthSq()

      const didLeaveRadius = distanceSq > (FOLLOW_RADIUS ** 2)

      if (didLeaveRadius) {
        return true
      }

      const currentRotation = tempEuler.setFromRotationMatrix(currentCameraTransform_, YXZ).y
      const homeRotation = tempEuler.setFromRotationMatrix(homeCameraTransform_, YXZ).y

      const rotationChange = Math.abs(currentRotation - homeRotation)
      // If rotationChange is 361 deg, that is equivalent to 1 deg
      const fixedRotationChange = rotationChange % (Math.PI * 2)

      return fixedRotationChange > FOLLOW_ANGLE
    }
  })()

  const checkShouldFollow = () => {
    if (dragging_) {
      return
    }

    if (following_) {
      return
    }

    const now = performance.now()

    if (now < lastCheckedFollowAt_ + FOLLOW_CHECK_CADENCE) {
      return
    }

    lastCheckedFollowAt_ = now

    updateTransforms()
    extractCameraReferenceTransform(currentCameraTransform_)

    const didExitHomeZone = checkDidExitHomeZone()

    if (!didExitHomeZone) {
      exitedRadiusAt_ = 0
      return
    }

    if (!exitedRadiusAt_) {
      exitedRadiusAt_ = now
      previousCameraPosition_.copy(cameraLocalPosition_)
      return
    }

    if (now < exitedRadiusAt_ + FOLLOW_DELAY) {
      previousCameraPosition_.copy(cameraLocalPosition_)
      return
    }

    const lastMoveSq = previousCameraPosition_.distanceToSquared(cameraLocalPosition_)
    const isMoving = lastMoveSq > (FOLLOW_MOVEMENT_THRESHOLD ** 2)

    const shouldStartFollow = didExitHomeZone && !isMoving

    if (!shouldStartFollow) {
      previousCameraPosition_.copy(cameraLocalPosition_)
      return
    }

    const inverseHomeMatrixTransform = homeCameraTransform_.clone()
    inverseHomeMatrixTransform.invert()

    const newGoal = attachmentPoint_.position.clone()
    newGoal.applyMatrix4(inverseHomeMatrixTransform)
    newGoal.applyMatrix4(currentCameraTransform_)

    startFollowPath(newGoal)
    saveCameraHome()
    exitedRadiusAt_ = 0
  }

  const extractControllerRay = (controller) => {
    const tracer = controller.getObjectByName('tracer')
    controllerRay_.origin.set(0, 0, 0)
    controllerRay_.direction.set(0, 0, -1)
    controllerRay_.applyMatrix4(tracer.matrixWorld)
    controllerRay_.applyMatrix4(inverseControllerSpace_)
    controllerRay_.origin.copy(cameraLocalPosition_)
    controllerRay_.origin.y -= POSITION_OFFSET_Y
  }

  const onDragStart = (controller) => {
    dragging_ = true
    exitedRadiusAt_ = 0

    updateTransforms()
    extractControllerRay(controller)

    // The controller is aimed at the handle when the drag starts, so we use this drag
    // offset to adjust the controller aim to match the exact correct direction, ensuring the
    // tablet doesn't jump when the drag starts.
    const correctDirection = attachmentPoint_.position.clone()
      .sub(controllerRay_.origin)
      .normalize()

    dragOffsetQuaternion_.setFromUnitVectors(controllerRay_.direction, correctDirection)
  }

  const onDragMove = (currentController) => {
    updateTransforms()
    extractControllerRay(currentController)

    controllerRay_.direction.applyQuaternion(dragOffsetQuaternion_)

    // TODO(christoph): Add minimum up/down aim angle to reduce wild rotations at extremes
    controllerRay_.at(POSITION_RADIUS, attachmentPoint_.position)
    attachmentPoint_.lookAt(cameraWorldPosition_)
    following_ = false
  }

  const update = () => {
    if (paused_) {
      return
    }

    checkShouldFollow()

    if (!dragging_ && following_) {
      followAmount_ = lerp(followAmount_, 1, FOLLOW_LERP_AMOUNT)
      if (followAmount_ > FOLLOW_LERP_END) {
        followAmount_ = 1
      }
      getFollowPosition(followAmount_, attachmentPoint_.position)
      attachmentPoint_.lookAt(cameraWorldPosition_)
      following_ = followAmount_ < 1
    }
  }

  const onDragEnd = () => {
    dragging_ = false
    saveCameraHome()
  }

  const setPaused = (paused) => {
    paused_ = paused
    if (paused_) {
      exitedRadiusAt_ = 0
      following_ = false
      dragging_ = false
    }
  }

  const attach = () => {
    controllersGroup.add(attachmentPoint_)
  }

  const detach = () => {
    controllersGroup.remove(attachmentPoint_)
    lastCheckedFollowAt_ = 0
    exitedRadiusAt_ = 0
    dragging_ = false
    following_ = false
    followAmount_ = 0
    paused_ = false
  }

  return {
    attach,
    detach,
    getObject: () => attachmentPoint_,
    resetToFront,
    onDragStart,
    onDragMove,
    onDragEnd,
    update,
    setPaused,
  }
}

export {
  createDomTabletAlignment,
}
