import {Quaternion, Position} from './components'
import {registerComponent} from './registry'
import {quat} from './math/quat'
import {vec3} from './math/vec3'

const tempEuler = vec3.zero()
const tempQuaternion = quat.zero()
const tempMoveDelta = vec3.zero()
const tempFinalMoveDelta = vec3.zero()

const FlyController = registerComponent({
  name: 'fly-controller',
  schema: {
    verticalSensitivity: 'f32',
    horizontalSensitivity: 'f32',
    moveSpeedX: 'f32',
    moveSpeedY: 'f32',
    moveSpeedZ: 'f32',
    invertedY: 'boolean',
    invertedX: 'boolean',
  },
  schemaDefaults: {
    verticalSensitivity: 1,
    horizontalSensitivity: 1,
    moveSpeedX: 10,
    moveSpeedY: 10,
    moveSpeedZ: 10,
  },
  data: {
  },
  add: (world) => {
    world.input.setActiveMap('fly-controller')
    world.input.enablePointerLockRequest()
  },
  tick: (world, component) => {
    // we use delta time to ensure consistent movement speed even at low framerate
    const dt = Math.min(world.time.delta / 1000, 5)
    const {
      invertedY,
      invertedX,
      horizontalSensitivity,
      verticalSensitivity,
      moveSpeedX,
      moveSpeedY,
      moveSpeedZ,
    } = component.schema

    const cursor = Quaternion.cursor(world, component.eid)
    // convert quaternion to euler for easier manipulation
    tempQuaternion.setXyzw(cursor.x, cursor.y, cursor.z, cursor.w)
    tempQuaternion.pitchYawRollDegrees(tempEuler)

    let rotationDeltaX: number = 0
    let rotationDeltaY: number = 0
    rotationDeltaY += verticalSensitivity * world.input.getAction('lookUp') * dt
    rotationDeltaY -= verticalSensitivity * world.input.getAction('lookDown') * dt
    rotationDeltaX += horizontalSensitivity * world.input.getAction('lookRight') * dt
    rotationDeltaX -= horizontalSensitivity * world.input.getAction('lookLeft') * dt

    if (invertedY) {
      rotationDeltaY *= -1
    }

    if (invertedX) {
      rotationDeltaX *= -1
    }

    // Add clamping for vertical rotation
    tempEuler.setXyz(
      Math.max(-89.99, Math.min(tempEuler.x - rotationDeltaY, 89.99)),
      tempEuler.y - rotationDeltaX,
      0
    )

    // convert back to quaternion and set world rotation
    tempQuaternion.makePitchYawRollDegrees(tempEuler)
    cursor.x = tempQuaternion.x
    cursor.y = tempQuaternion.y
    cursor.z = tempQuaternion.z
    cursor.w = tempQuaternion.w

    const finalMoveSpeedX = moveSpeedX * dt
    const finalMoveSpeedY = moveSpeedY * dt
    const finalMoveSpeedZ = moveSpeedZ * dt

    // handle movement
    const positionCursor = Position.cursor(world, component.eid)
    tempMoveDelta.setFrom({
      x: (world.input.getAction('left') - world.input.getAction('right')) * finalMoveSpeedZ,
      y: (world.input.getAction('up') - world.input.getAction('down')) * finalMoveSpeedY,
      z: (world.input.getAction('forward') - world.input.getAction('backward')) * finalMoveSpeedX,
    })

    tempQuaternion.timesVec(tempMoveDelta, tempFinalMoveDelta)
    positionCursor.x += tempFinalMoveDelta.x
    positionCursor.y += tempFinalMoveDelta.y
    positionCursor.z += tempFinalMoveDelta.z
  },
  remove: (world) => {
    world.input.disablePointerLockRequest()
  },
})

export {
  FlyController,
}
