import * as ecs from './core-api'

const degreesToRadians = (degrees: number): number => degrees * (Math.PI / 180)

const INERTIA_TIMER = 200
const MOVING_TIMER = 30
const MAX_SPEED_FACTOR = 0.01

const capSpeed = (speed: number, maxSpeed: number) => Math.sign(speed) *
Math.min(Math.abs(speed), maxSpeed * MAX_SPEED_FACTOR)

const UP_VECTOR = ecs.math.vec3.from({x: 0, y: 1, z: 0})
const tempCameraPos = ecs.math.vec3.zero()
const tempPivotPoint = ecs.math.vec3.zero()
const tempMatrix = ecs.math.mat4.i()
const tempCameraRotation = ecs.math.quat.zero()

const OrbitControls = ecs.registerComponent({
  name: 'orbit-controls',
  schema: {
    speed: ecs.f32,
    maxAngularSpeed: ecs.f32,
    maxZoomSpeed: ecs.f32,
    distanceMin: ecs.f32,
    distanceMax: ecs.f32,
    pitchAngleMin: ecs.f32,
    pitchAngleMax: ecs.f32,
    constrainYaw: ecs.boolean,
    // @condition constrainYaw=true
    yawAngleMin: ecs.f32,
    // @condition constrainYaw=true
    yawAngleMax: ecs.f32,
    inertiaFactor: ecs.f32,
    focusEntity: ecs.eid,
    invertedX: ecs.boolean,
    invertedY: ecs.boolean,
    invertedZoom: ecs.boolean,
    controllerSupport: ecs.boolean,
    // @condition controllerSupport=true
    horizontalSensitivity: ecs.f32,
    // @condition controllerSupport=true
    verticalSensitivity: ecs.f32,
  },
  schemaDefaults: {
    speed: 5,
    maxAngularSpeed: 10,
    maxZoomSpeed: 10,
    distanceMin: 5,
    distanceMax: 20,
    pitchAngleMin: -90,
    pitchAngleMax: 90,
    constrainYaw: false,
    yawAngleMin: 0,
    yawAngleMax: 0,
    inertiaFactor: 0.3,
    horizontalSensitivity: 1,
    verticalSensitivity: 1,
  },
  data: {
    pitch: ecs.f32,
    yaw: ecs.f32,
    distance: ecs.f32,
    dx: ecs.f32,
    dy: ecs.f32,
    delta: ecs.f32,
    moving: ecs.f32,
    inertia: ecs.f32,
    minPitch: ecs.f32,
    maxPitch: ecs.f32,
    curPitch: ecs.f32,
    constrainYaw: ecs.boolean,
    minYaw: ecs.f32,
    maxYaw: ecs.f32,
    curYaw: ecs.f32,
    curDistance: ecs.f32,
  },
  stateMachine: ({world, eid, schemaAttribute, dataAttribute}) => {
    if (!dataAttribute) {
      return
    }

    type DataCursor = ReturnType<typeof dataAttribute.cursor>

    let didLockPointer = false

    const setCameraPosition = (focusEntity: ecs.Eid, data: DataCursor) => {
      if (focusEntity) {
        world.transform.getWorldPosition(focusEntity, tempPivotPoint)
      } else {
        tempPivotPoint.setXyz(0, 0, 0)
      }

      // NOTE(johnny): This can maybe be code golfed.
      // https://github.com/8thwall/code8/pull/364/files#r2080169028
      data.curPitch = data.pitch
      data.curYaw = data.yaw
      data.curDistance = data.distance

      const sinPitch = Math.sin(data.pitch)
      const cosPitch = Math.cos(data.pitch)
      const sinYaw = Math.sin(data.yaw)
      const cosYaw = Math.cos(data.yaw)

      tempCameraPos.setXyz(
        data.distance * cosPitch * cosYaw,
        data.distance * sinPitch,
        data.distance * cosPitch * sinYaw
      )

      // Add the pivot point to the position to get the final position
      tempCameraPos.setPlus(tempPivotPoint)

      tempMatrix.makeT(tempCameraPos)
      tempMatrix.setLookAt(tempPivotPoint, UP_VECTOR)
      tempMatrix.decomposeR(tempCameraRotation)

      ecs.Position.set(world, eid, tempCameraPos)
      ecs.Quaternion.set(world, eid, tempCameraRotation)
    }

    const updateDistance = (data: DataCursor, distanceMin: number, distanceMax: number) => {
      const newDistance = data.distance + data.delta
      const distanceClamped = Math.min(Math.max(newDistance, distanceMin), distanceMax)
      data.distance = distanceClamped
    }

    const handleMoveController = (data: DataCursor) => {
      const up = world.input.getAction('lookUp')
      const down = world.input.getAction('lookDown')
      const left = world.input.getAction('lookLeft')
      const right = world.input.getAction('lookRight')
      const {
        speed,
        maxAngularSpeed,
        maxZoomSpeed,
        distanceMin,
        distanceMax,
        horizontalSensitivity,
        verticalSensitivity,
        invertedX,
        invertedY,
        invertedZoom,
      } = schemaAttribute.get(eid)

      data.dx = capSpeed(
        (left - right) * (invertedX ? -1 : 1) * horizontalSensitivity * speed, maxAngularSpeed
      )

      data.dy = capSpeed(
        (up - down) * (invertedY ? -1 : 1) * verticalSensitivity * speed, maxAngularSpeed
      )

      const pitchAngle = data.pitch - data.dy
      const yawAngle = data.yaw - data.dx
      const pitchClamped = Math.min(Math.max(pitchAngle, data.minPitch),
        data.maxPitch)
      const yawAngleClamped = Math.min(Math.max(yawAngle, data.minYaw), data.maxYaw)
      data.pitch = pitchClamped
      data.yaw = data.constrainYaw ? yawAngleClamped : yawAngle

      const zoomIn = world.input.getAction('zoomIn')
      const zoomOut = world.input.getAction('zoomOut')
      data.delta = capSpeed((zoomIn - zoomOut) * (invertedZoom ? -1 : 1) * speed, maxZoomSpeed)
      if (left || right || up || down || zoomIn || zoomOut) {
        data.moving = MOVING_TIMER
        data.inertia = INERTIA_TIMER
      }
      updateDistance(data, distanceMin, distanceMax)
    }

    const handleEnter = () => {
      const {
        pitchAngleMax, pitchAngleMin, distanceMax, distanceMin,
        focusEntity, yawAngleMax, yawAngleMin, constrainYaw,
        controllerSupport,
      } = schemaAttribute.get(eid)
      const data = dataAttribute.cursor(eid)

      const validPitchInterval = pitchAngleMin <= pitchAngleMax
      const validYawInterval = yawAngleMin <= yawAngleMax

      const minPitch = validPitchInterval
        ? degreesToRadians(Math.max(pitchAngleMin, -89.9999))
        : 0
      const maxPitch = validPitchInterval ? degreesToRadians(Math.min(pitchAngleMax, 89.9999)) : 0
      const minYaw = validYawInterval ? degreesToRadians(yawAngleMin) : 0
      const maxYaw = validYawInterval ? degreesToRadians(yawAngleMax) : 0

      data.minPitch = minPitch
      data.maxPitch = maxPitch
      data.minYaw = minYaw
      data.maxYaw = maxYaw
      data.distance = (distanceMin + distanceMax) / 2
      data.pitch = (minPitch + maxPitch) / 2
      data.constrainYaw = constrainYaw
      data.yaw = (minYaw + maxYaw) / 2

      // // Set the initial camera position
      setCameraPosition(focusEntity, data)

      // Note: Enable the orbit controls preset on the input manager for controller support
      if (controllerSupport) {
        world.input.setActiveMap('orbit-controls')
        world.input.enablePointerLockRequest()
        didLockPointer = true
      }
    }

    const handleTick = () => {
      const {
        speed, inertiaFactor, maxZoomSpeed, maxAngularSpeed,
        distanceMin, distanceMax, controllerSupport, focusEntity,
      } = schemaAttribute.get(eid)
      const data = dataAttribute.cursor(eid)

      if (controllerSupport) {
        handleMoveController(data)
      }

      if (data.inertia > 0 && inertiaFactor !== 0 && data.moving <= 0) {
        const currentInertia = Math.min(
          inertiaFactor * world.time.delta * (data.inertia / INERTIA_TIMER), 1
        )

        if (data.delta !== 0) {
          const newDistance = data.distance +
            capSpeed((data.delta / speed) * currentInertia, maxZoomSpeed)
          const distanceClamped = Math.min(Math.max(newDistance, distanceMin), distanceMax)
          data.distance = distanceClamped
        }

        if (data.dy !== 0 || data.dx !== 0) {
          const pitchAngle = data.pitch -
            capSpeed((data.dy / speed) * currentInertia, maxAngularSpeed)
          const yawAngle = data.yaw - capSpeed(
            (data.dx / speed) * currentInertia, maxAngularSpeed
          )
          const pitchClamped = Math.min(Math.max(pitchAngle, data.minPitch), data.maxPitch)
          const yawAngleClamped = Math.min(Math.max(yawAngle, data.minYaw), data.maxYaw)
          data.pitch = pitchClamped
          data.yaw = data.constrainYaw ? yawAngleClamped : yawAngle
        }

        data.inertia -= world.time.delta

        if (data.inertia <= 0) {
          data.dx = 0
          data.dy = 0
          data.delta = 0
        }
      }

      if (data.moving > 0) {
        data.moving -= world.time.delta
      }

      setCameraPosition(focusEntity, data)
    }

    const handleExit = () => {
      if (didLockPointer) {
        world.input.disablePointerLockRequest()
      }
    }

    const handleGestureMove = (e: any) => {
      const gestureMoveEvent = e.data as ecs.GestureMoveEvent
      const {
        speed, maxAngularSpeed, maxZoomSpeed, distanceMin, distanceMax, invertedX, invertedY,
        invertedZoom,
      } = schemaAttribute.get(eid)

      const data = dataAttribute.cursor(eid)
      if (gestureMoveEvent.touchCount === 1) {
        const {positionChange} = gestureMoveEvent

        // note: touch controls x axis is inverted by default
        data.dx = capSpeed(positionChange.x * speed, maxAngularSpeed) *
          (invertedX ? -1 : 1) * -1
        data.dy = capSpeed(positionChange.y * speed, maxAngularSpeed) *
          (invertedY ? -1 : 1)

        const pitchAngle = data.pitch - data.dy
        const yawAngle = data.yaw - data.dx
        const pitchClamped = Math.min(Math.max(pitchAngle, data.minPitch),
          data.maxPitch)
        const yawAngleClamped = Math.min(Math.max(yawAngle, data.minYaw), data.maxYaw)
        data.pitch = pitchClamped
        data.yaw = data.constrainYaw ? yawAngleClamped : yawAngle
      } else if (gestureMoveEvent.touchCount === 2) {
        const {spreadChange} = gestureMoveEvent

        data.delta = capSpeed(spreadChange * speed, maxZoomSpeed) * (invertedZoom ? -1 : 1)
      }
      data.moving = MOVING_TIMER
      data.inertia = INERTIA_TIMER
      updateDistance(data, distanceMin, distanceMax)
    }

    ecs.defineState('initial').initial()
      .onEnter(handleEnter)
      .onTick(handleTick)
      .listen(world.events.globalId, ecs.input.GESTURE_MOVE, handleGestureMove)
      .onExit(handleExit)
  },
})

export {
  OrbitControls,
}
