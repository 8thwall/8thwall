import {setWorldTransformGen, rotationForMatrixGen} from './transforms'

const desktopCameraManager = () => {
  const {THREE} = window
  const YXZ = 'YXZ'
  let camera_ = null
  let canvas_ = null
  let motionRaf_ = null
  let scale_ = 1
  let maxDist_ = 100
  const originalTransform_ = {
    position: new THREE.Vector3(),
    quaternion: new THREE.Quaternion(),
    scale: new THREE.Vector3(),
  }

  const dragState_ = {
    dragging: false,        // True if we're currently dragging.
    startQuaternion: null,  // The starting rotation of the camera.
    startVec: null,         // The starting location of the drag in the camera.
  }

  const motionState_ = {
    moving: false,
    up: 0,
    down: 0,
    left: 0,
    right: 0,
    lastTick: null,
  }

  const LEFT_KEYS = [65, 37]  // a, ArrowLeft
  const UP_KEYS = [87, 38]  // w, ArrowUp
  const RIGHT_KEYS = [68, 39]  // d, ArrowRight
  const DOWN_KEYS = [83, 40]  // s, ArrowDown

  const rotationForMatrix = rotationForMatrixGen()
  const setWorldTransform = setWorldTransformGen()
  const preventDefault = e => e.preventDefault()

  // Find a real camera under an object tree and return its projection matrix inverse.
  const projectionMatrixInverse = (obj) => {
    if (obj.projectionMatrixInverse) {
      return obj.projectionMatrixInverse
    }
    return obj.children.reduce((projInv, child) => {
      if (projInv) {
        return projInv
      }
      return projectionMatrixInverse(child)
    }, null)
  }

  // Get a unit vector from the origin to a point at the pixel that was clicked.
  const cameraRay = (event) => {
    const projInv = projectionMatrixInverse(camera_)
    const clipX = (2 * (event.clientX - canvas_.clientLeft)) / canvas_.clientWidth - 1
    const clipY = 1 - (2 * (event.clientY - canvas_.clientTop)) / canvas_.clientHeight
    return new THREE.Vector3(clipX, clipY, 0.5).applyMatrix4(projInv).normalize()
  }

  const recenter = (initialSceneCameraMatrix, camera) => {
    // Set scale based on initial camera height.
    scale_ = initialSceneCameraMatrix.elements[13] || 1

    // Set maxDist based on scale and far clip. The void space sphere is at ~farClip/2, and if
    // the camera gets too close it won't look like an infinite sphere but will have obvious
    // translational artifacts. So at most you can travel half of the distance to the edge of it.
    const intrinsics = camera.projectionMatrix.elements
    const farClip = intrinsics[14] / (intrinsics[10] + 1)
    maxDist_ = Math.min(scale_ * 100, 0.25 * farClip)

    // If the user has not set an x-axis rotation, set one pointed 15 degrees toward the ground.
    initialSceneCameraMatrix.decompose(
      originalTransform_.position, originalTransform_.quaternion, originalTransform_.scale
    )

    if (!camera_) {
      return
    }

    const euler = new THREE.Euler().setFromQuaternion(originalTransform_.quaternion, YXZ)
    if (Math.abs(euler.x) < 1e-3) {
      euler.x = -0.261799  // 15 degrees in radians.
      setWorldTransform(
        camera_, {...originalTransform_, quaternion: new THREE.Quaternion().setFromEuler(euler)}
      )
    }
  }

  const mouseDown = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    // Only right-click events.
    if (!event.ctrlKey && event.button !== 2) {
      return
    }

    dragState_.dragging = true
    dragState_.startVec = cameraRay(event)
    dragState_.startQuaternion = rotationForMatrix(camera_.matrixWorld)

    event.stopImmediatePropagation()
  }

  const mouseUp = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    if (!dragState_.dragging) {
      // If a drag action was started without a key modifier (like ctrl), we may not have a
      // matching event here. Just ignore.
      return
    }

    dragState_.dragging = false
    dragState_.startVecVert = null
    dragState_.startQuaternionVert = null
  }

  const pickThetaDegenerate = (a2, b2) => {
    // We are trying to hit 1 = a2 cos(theta) + b2 sin(theta) but a2 and b2 are both not large
    // enough to make this possible. Instead, we get as close as we can.
    if (Math.abs(b2) <= 1e-3) {
      return a2 >= 0 ? 0 : Math.PI  // Cosine only: 1 at 0 and -1 at pi.
    }
    if (Math.abs(a2) <= 1e-3) {
      return b2 >= 0 ? Math.PI / 2 : -Math.PI / 2  // Sine only: 1 at pi / 2 and -1 and -pi / 2.
    }

    // Solving the derivative and finding roots yields:
    //
    //   theta = 2 * atan (-(a +/- sqrt(a^2 + b^2)) / b)
    //
    // There are two solutions in this case. Try both and see which is closer to 1.
    const len = Math.sqrt(a2 * a2 + b2 * b2)
    const theta1 = 2 * Math.atan(-(a2 - len) / b2)
    const theta2 = 2 * Math.atan(-(a2 + len) / b2)
    const v1 = a2 * Math.cos(theta1) + b2 * Math.sin(theta1)
    const v2 = a2 * Math.cos(theta2) + b2 * Math.sin(theta2)
    return v1 >= v2 ? theta1 : theta2
  }

  // See derivation in `moveMouse` below.
  const pickTheta = (a2, b2, endVec, worldEndVec) => {
    if (Math.abs(a2 + 1) < 1e-20) {
      return 2 * Math.atan(1 / b2)
    }
    const sqLen = a2 * a2 + b2 * b2
    if (sqLen < 1) {
      return pickThetaDegenerate(a2, b2)
    }
    if (worldEndVec.y > 0) {
      return endVec.x > 0
        ? 2 * Math.atan((b2 - Math.sqrt(sqLen - 1)) / (a2 + 1))
        : 2 * Math.atan((b2 + Math.sqrt(sqLen - 1)) / (a2 + 1))
    }
    return endVec.x < 0
      ? 2 * Math.atan((b2 - Math.sqrt(sqLen - 1)) / (a2 + 1))
      : 2 * Math.atan((b2 + Math.sqrt(sqLen - 1)) / (a2 + 1))
  }

  const mouseMove = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    if (!dragState_.dragging) {
      // If a drag action was started without a key modifier (like ctrl), we may not have a
      // matching event here. Just ignore.
      return
    }
    // If no button is pressed, we missed an earlier mouse up event (maybe the mouse went up off the
    // screen). So, we're done.
    if (!event.buttons) {
      mouseUp(event)
      return
    }
    const {startVec, startQuaternion} = dragState_

    // This is the location where the starting drag point should end up. It's a ray-space
    // representation of the current location of the mouse pointer.
    const endVec = cameraRay(event)

    // When the mouse x is at 0 (mid-screen), one of the denominator terms below goes to 0, leading
    // to NaNs. Instead, we simulate the mouse just being pretty close to the middle of the screen.
    if (Math.abs(endVec.x) < 1e-5) {
      endVec.x = endVec.x < 1 ? -1e-5 : 1e-5
    }

    // Find a world transform that brings the start of the mouse drag into the end of the mouse
    // drag. This puts the dragged scene point into the right spot in the camera, but the world may
    // be tilted / not aligned with the horizon.
    const t1 = new THREE.Quaternion().setFromUnitVectors(endVec, startVec)
    t1.premultiply(startQuaternion)

    // We need to find the angle that rotates along the ray from the camera to the current mouse
    // pointer that makes the horizon horizontal in the camera image. To start with, we take a
    // horizontal line in the image and find out where it is in the world, and we find the rotation
    // axis in the world space.
    const rightVec = new THREE.Vector3(1, 0, 0).applyQuaternion(t1)
    const worldEndVec = endVec.clone().applyQuaternion(t1)

    // Now we need to find a rotation along the world rotation axis that will cause the horizon line
    // from the image to be rotated into the x/z plane (y=0). The axis angle rotation equation is:
    //
    //   Vrot = cos(theta) * V + sin(theta) * (e cross v) + (1 - cos(theta)) * (E dot V) * E
    //
    // where E is the angle-axis axis and V is the vector to rotate. In this case, we want the
    // rotated horizon vector to have y=0. Focusing just on y, we have:
    //
    //   Vrot_y = cos(theta) v_y + sin(theta)(e_z v_x - e_x v_z)
    //              + (1 - cos(theta)) * (e_x v_x + e_y v_y + e_z v_z) * e_y
    //
    // Setting Vrot_y = 0 and grouping constant terms, we have:
    //
    //   0 = a cos(theta) + b sin(theta) + c (1 - cos(theta))
    //   0 = (a - c) cos(theta) + b sin(theta) + c
    //  -c = (a - c) cos(theta) + b sin(theta)
    //   1 = (a - c) / -c cos(theta) + b / -c sin(theta)
    //
    // Grouping constants again we have:
    //
    //   1 = a2 cos(theta) + b2 sin(theta)
    //
    // This has the following three solutions:
    //
    //   theta1 = 2 * atan((b2 - sqrt(a2^2 + b2^2 - 1)) / (a2 + 1)) when a2 != -1
    //   theta2 = 2 * atan((b2 + sqrt(a2^2 + b2^2 - 1)) / (a2 + 1)) when a2 != -1
    //   theta3 = 2 * atan(1 / b) when a2 = -1
    //
    // The first two solutions put the horizon horizontal in the image, but either the image is
    // right side up or upside down. When endVec is above the horizon in the world, theta1 is
    // correct when endVec is in the left side of the image and theta2 is correct when endVec is on
    // the right side of the image.  Below the horizon this is reversed. The third solution is for
    // the special case when a2 would otherwise cause a divide by zero error.
    const {x: vx, y: vy, z: vz} = rightVec
    const {x: ex, y: ey, z: ez} = worldEndVec
    const a = vy
    const b = (ez * vx - ex * vz)
    const c = (ex * vx + ey * vy + ez * vz) * ey
    const a2 = -(a - c) / c
    const b2 = -b / c

    const finalTheta = pickTheta(a2, b2, endVec, worldEndVec) || 0  // Protect against NaN.

    const t2 = new THREE.Quaternion().setFromAxisAngle(endVec, finalTheta).premultiply(t1)

    // TODO(nb): Near the poles this sometimes produces an inverted view due to the a^2 + b^2 < 1
    // case. Ensure that these are coerced to positive view values.

    setWorldTransform(
      camera_,
      {
        position: camera_.position,
        quaternion: t2,
        scale: camera_.scale,
      }
    )
  }

  const processMotion = (time) => {
    if (!motionState_.moving) {
      motionRaf_ = null
      return
    }
    motionRaf_ = requestAnimationFrame(processMotion)
    const {up, down, left, right, lastTick} = motionState_
    const timeDelta = time - lastTick
    motionState_.lastTick = time

    const dir = new THREE.Vector3(right - left, 0, down - up)

    dir.multiplyScalar(timeDelta * 5e-3 * scale_)

    const position = new THREE.Vector3()
    const quaternion = new THREE.Quaternion()
    const scale = new THREE.Vector3()
    camera_.matrixWorld.decompose(position, quaternion, scale)
    const finalY = position.y

    // Only move in the x/z plane.
    const cameraDir =
      new THREE.Euler().setFromQuaternion(rotationForMatrix(camera_.matrixWorld), YXZ)
    cameraDir.x = 0
    cameraDir.z = 0

    dir.applyEuler(cameraDir)

    position.add(dir)
    position.clampLength(0, maxDist_)
    position.y = finalY  // Ensure that Y never changes.
    setWorldTransform(camera_, {position, quaternion, scale})
  }

  const keyDown = (event) => {
    // Toggle on if the key is in this event.
    /* eslint-disable no-bitwise */
    motionState_.up |= UP_KEYS.includes(event.keyCode)
    motionState_.down |= DOWN_KEYS.includes(event.keyCode)
    motionState_.left |= LEFT_KEYS.includes(event.keyCode)
    motionState_.right |= RIGHT_KEYS.includes(event.keyCode)
    /* eslint-enable no-bitwise */
    if (motionState_.moving) {
      // don't need to enqueue.
      return
    }
    if (motionState_.up || motionState_.down || motionState_.left || motionState_.right) {
      motionState_.moving = true
      motionState_.lastTick = event.timeStamp
      motionRaf_ = requestAnimationFrame(processMotion)
    }
  }

  const keyUp = (event) => {
    // Toggle off if they key is in this event.
    /* eslint-disable no-bitwise */
    motionState_.up &= !UP_KEYS.includes(event.keyCode)
    motionState_.down &= !DOWN_KEYS.includes(event.keyCode)
    motionState_.left &= !LEFT_KEYS.includes(event.keyCode)
    motionState_.right &= !RIGHT_KEYS.includes(event.keyCode)
    /* eslint-enable no-bitwise */
    if (!(motionState_.up || motionState_.down || motionState_.left || motionState_.right)) {
      motionState_.moving = false
    }
  }

  const attach = ({camera, canvas, sessionConfiguration}) => {
    if (sessionConfiguration && sessionConfiguration.disableDesktopCameraControls) {
      return
    }
    camera_ = camera
    canvas_ = canvas

    canvas_.addEventListener('contextmenu', preventDefault)  // Allow right click handling.
    canvas_.addEventListener('mousedown', mouseDown, {capture: true})
    canvas_.addEventListener('mouseup', mouseUp)
    canvas_.addEventListener('mousemove', mouseMove)

    // Add key handlers to the window. Canvas elements can't get keyboard input without tabIndex=1,
    // and then the treatment is weird (draws a blue box around the canvas). Also, if a user presses
    // a button on a dom overlay and then enters keystrokes, we still want the keystrokes to work
    // without the user clicking off of the button, so event target filters can be tricky.
    window.addEventListener('keydown', keyDown)
    window.addEventListener('keyup', keyUp)
  }

  const detach = () => {
    if (!camera_) {
      return
    }
    setWorldTransform(camera_, originalTransform_)
    camera_ = null

    if (motionRaf_) {
      cancelAnimationFrame(motionRaf_)
      motionRaf_ = null
    }
    canvas_.removeEventListener('contextmenu', preventDefault)
    canvas_.removeEventListener('mousedown', mouseDown, {capture: true})
    canvas_.removeEventListener('mouseup', mouseUp)
    canvas_.removeEventListener('mousemove', mouseMove)
    window.removeEventListener('keydown', keyDown)
    window.removeEventListener('keyup', keyUp)
  }

  return {
    attach,
    detach,
    recenter,
  }
}

export {
  desktopCameraManager,
}
