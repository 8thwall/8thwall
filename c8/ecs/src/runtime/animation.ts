import {Easing} from '@tweenjs/tween.js'

import THREE from './three'
import type {World} from './world'
import {getAttribute, registerComponent} from './registry'
import {Position, Scale, Quaternion} from './components'
import type {Eid} from '../shared/schema'
import {mat4} from './math/math'
import type * as THREE_TYPES from './three-types'
import {degreesToRadians} from '../shared/angle-conversion'
import {events} from './event-ids'

// Interface to schema for all animation components.
interface AnimationSchema {
  readonly target: Eid
  readonly duration: number
  readonly loop: boolean
  readonly reverse: boolean
  readonly autoFrom: boolean
  readonly easeIn: boolean
  readonly easeOut: boolean
  readonly easingFunction: string
}

// Interface to data shared by all animation components.
interface AnimationData {
  start: number
}

// Interface for parameters shared by all animation components.
interface Component<Schema, Data> {
  eid: Eid
  schema: Schema
  data: Data
}

const EULER_ORDER = 'YXZ'

// Objects that persist across ticks during animations to avoid creating
// new instances (and therefore unnecessary garbage) every tick:
const transform8 = mat4.i()  // new THREE.Matrix4()
const transform = new THREE.Matrix4()
const entityPos = new THREE.Vector3()
const parentPos = new THREE.Vector3()
const targetDir = new THREE.Vector3()
const moveDelta = new THREE.Vector3()
const parentScale = new THREE.Vector3()
const upVector = new THREE.Vector3(0, 1, 0)
const parentRotation = new THREE.Quaternion()
const targetRotation = new THREE.Quaternion()
const tempRotation = new THREE.Quaternion()
const tempEuler = new THREE.Euler(0, 0, 0, EULER_ORDER)
const targetEuler = new THREE.Euler(0, 0, 0, EULER_ORDER)
const tempMatrix = new THREE.Matrix4()

let didWarnInvalidLock = false

// Calculate the progress for an animation component.
const linearProgress = (
  world: World, component: Component<AnimationSchema, AnimationData>
): number => {
  // Instantly complete if duration is zero or negative
  if (component.schema.duration <= 0) {
    return 1
  }
  const progress = (world.time.elapsed - component.data.start) / component.schema.duration

  if (component.schema.loop) {
    if (component.schema.reverse) {
      const res = 1 - Math.abs((progress % 2) - 1)
      // note: if the duration is 1, it will end the loop and remove the component.
      return Math.min(res, 0.999)
    } else {
      return progress % 1
    }
  } else if (progress >= 1) {
    return 1
  }

  return progress
}

// Type of easing supported by three.js tween module for anything non-linear.
interface EasingMathFuncs {
  InOut: (k: number) => number
  In: (k: number) => number
  Out: (k: number) => number
}

const getEasingMathFuncs = (easingMath: string): EasingMathFuncs => {
  switch (easingMath) {
    default:  // default to quadratic if an easing function is required but not provided
    case 'Quadratic': return Easing.Quadratic
    case 'Cubic': return Easing.Cubic
    case 'Quartic': return Easing.Quartic
    case 'Quintic': return Easing.Quintic
    case 'Sinusoidal': return Easing.Sinusoidal
    case 'Exponential': return Easing.Exponential
    case 'Circular': return Easing.Circular
    case 'Elastic': return Easing.Elastic
    case 'Back': return Easing.Back
    case 'Bounce': return Easing.Bounce
  }
}

const easedProgress = (
  progress: number, easeIn: boolean, easeOut: boolean, easingFunction: string
): number => {
  if (easeIn) {
    if (easeOut) {
      return (getEasingMathFuncs(easingFunction)).InOut(progress)
    }

    return (getEasingMathFuncs(easingFunction)).In(progress)
  } else if (easeOut) {
    return (getEasingMathFuncs(easingFunction)).Out(progress)
  }

  return progress
}

interface AttributeAnimationSchema extends AnimationSchema {
  attribute: string
}

// Get the cursor for the attribute of an animation component.
const getAttributeCursor = (
  world: World, componentId: Eid, schema: AttributeAnimationSchema
) => {
  if (!schema.attribute) {
    return null
  }
  const eid = schema.target || componentId
  return getAttribute(schema.attribute).cursor(world, eid) as Record<string, number>
}

// Interface to any xyz vector type
interface IVector3 {
  x: number
  y: number
  z: number
}

// Define a function type for getting the cursor for a component that animates a Vector3.
type Vector3CursorFn<Schema extends AnimationSchema> =
 (world: World, componentId: Eid, schema: Schema) => IVector3 | null

// Get the cursor for the position of an animation component.
const getPositionCursor: Vector3CursorFn<AnimationSchema> = (world, componentId, schema) => {
  const eid = schema.target || componentId
  return Position.cursor(world, eid)
}

const getScaleCursor: Vector3CursorFn<AnimationSchema> = (world, componentId, schema) => {
  const eid = schema.target || componentId
  return Scale.cursor(world, eid)
}

const getRotationCursor = (world: World, componentId: Eid, schema: AnimationSchema) => {
  const eid = schema.target || componentId
  return Quaternion.cursor(world, eid)
}

const getRotationCursorAsEuler = (world: World, componentId: Eid, schema: AnimationSchema) => {
  const cursor = getRotationCursor(world, componentId, schema)
  return cursor
    ? tempEuler.setFromQuaternion(tempRotation.set(cursor.x, cursor.y, cursor.z, cursor.w))
    : null
}

const convertEulerToQuaternion = (
  eulerX: number, eulerY: number, eulerZ: number, rotation: THREE_TYPES.Quaternion
) => {
  tempEuler.set(eulerX, eulerY, eulerZ, EULER_ORDER)
  rotation.setFromEuler(tempEuler)
  return rotation
}

const getVector3AttributeCursor:
Vector3CursorFn<AttributeAnimationSchema & Vector3AnimationSchema> = (
  world, componentId, schema
) => {
  if (!schema.attribute) {
    return null
  }
  const eid = schema.target || componentId
  return getAttribute(schema.attribute).cursor(world, eid) as IVector3
}

const calculateProgress =
  (from: number, to: number, progress: number) => from * (1 - progress) + (to * progress)

// Interface to the schema shared by all Vector3 animation components.
interface Vector3AnimationSchema extends AnimationSchema {
  readonly fromX: number
  readonly fromY: number
  readonly fromZ: number
  readonly toX: number
  readonly toY: number
  readonly toZ: number
}

// Interface to the data shared by all Vector3 animation components.
interface Vector3AnimationData extends AnimationData {
  startX: number
  startY: number
  startZ: number
}

// Setup function for a Vector3 animation component. This is called when the component is added.
const setupVector3Component = <Schema extends Vector3AnimationSchema>(
  world: World,
  component: Component<Schema, Vector3AnimationData>,
  cursorFn: Vector3CursorFn<Schema>
) => {
  component.data.start = world.time.elapsed
  const {schema} = component
  const cursor = component.schema.autoFrom ? cursorFn(world, component.eid, schema) : null
  component.data.startX = cursor ? cursor.x : schema.fromX
  component.data.startY = cursor ? cursor.y : schema.fromY
  component.data.startZ = cursor ? cursor.z : schema.fromZ
}

// Update a Vector3 component. This is called every tick.
const updateVector3Component = (
  world: World,
  component: Component<Vector3AnimationSchema, Vector3AnimationData>,
  progress: number,
  cursor: IVector3 | null
) => {
  if (!cursor) {
    return
  }

  const {schema} = component
  const result = easedProgress(progress, schema.easeIn, schema.easeOut, schema.easingFunction)

  cursor.x = calculateProgress(component.data.startX, schema.toX, result)
  cursor.y = calculateProgress(component.data.startY, schema.toY, result)
  cursor.z = calculateProgress(component.data.startZ, schema.toZ, result)
}

const PositionAnimation = registerComponent({
  name: 'position-animation',
  schema: {
    autoFrom: 'boolean',
    // @group start from:vector3
    // @group condition autoFrom=false
    fromX: 'f32',
    fromY: 'f32',
    fromZ: 'f32',
    // @group end
    // @group start to:vector3
    toX: 'f32',
    toY: 'f32',
    toZ: 'f32',
    // @group end
    duration: 'f32',
    loop: 'boolean',
    // @condition loop=true
    reverse: 'boolean',
    easeIn: 'boolean',
    easeOut: 'boolean',
    // eslint-disable-next-line max-len
    // @enum Quadratic, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular, Elastic, Back, Bounce
    // @condition easeIn=true|easeOut=true
    easingFunction: 'string',
    // @label Target (optional)
    target: 'eid',
  },
  schemaDefaults: {
    duration: 1000,
    loop: true,
  },
  data: {
    start: 'ui32',
    startX: 'f32',
    startY: 'f32',
    startZ: 'f32',
  },
  add: (world, component) => {
    setupVector3Component(world, component, getPositionCursor)
  },
  tick: (world, component) => {
    const {schema} = component
    const position = getPositionCursor(world, component.eid, schema)

    const progress = linearProgress(world, component)
    updateVector3Component(world, component, progress, position)
    if (progress === 1) {
      // Animation is complete, can remove.
      PositionAnimation.remove(world, component.eid)
      world.events.dispatch(component.eid, events.POSITION_ANIMATION_COMPLETE)
    }
  },
})

const ScaleAnimation = registerComponent({
  name: 'scale-animation',
  schema: {
    autoFrom: 'boolean',
    // @group start from:vector3
    // @group condition autoFrom=false
    fromX: 'f32',
    fromY: 'f32',
    fromZ: 'f32',
    // @group end
    // @group start to:vector3
    toX: 'f32',
    toY: 'f32',
    toZ: 'f32',
    // @group end
    duration: 'f32',
    loop: 'boolean',
    // @condition loop=true
    reverse: 'boolean',
    easeIn: 'boolean',
    easeOut: 'boolean',
    // eslint-disable-next-line max-len
    // @enum Quadratic, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular, Elastic, Back, Bounce
    // @condition easeIn=true|easeOut=true
    easingFunction: 'string',
    // @label Target (optional)
    target: 'eid',
  },
  schemaDefaults: {
    duration: 1000,
    loop: true,
  },
  data: {
    start: 'ui32',
    startX: 'f32',
    startY: 'f32',
    startZ: 'f32',
  },
  add: (world, component) => {
    setupVector3Component(world, component, getScaleCursor)
  },
  tick: (world, component) => {
    const {schema} = component
    const cursor = getScaleCursor(world, component.eid, schema)

    const progress = linearProgress(world, component)
    updateVector3Component(world, component, progress, cursor)
    if (progress === 1) {
      // Animation is complete, can remove.
      ScaleAnimation.remove(world, component.eid)
      world.events.dispatch(component.eid, events.SCALE_ANIMATION_COMPLETE)
    }
  },
})

const RotateAnimation = registerComponent({
  name: 'rotate-animation',
  schema: {
    autoFrom: 'boolean',
    // @group start from:vector3
    // @group condition autoFrom=false
    fromX: 'f32',
    fromY: 'f32',
    fromZ: 'f32',
    // @group end
    // @group start to:vector3
    toX: 'f32',
    toY: 'f32',
    toZ: 'f32',
    // @group end
    shortestPath: 'boolean',
    duration: 'f32',
    loop: 'boolean',
    // @condition loop=true
    reverse: 'boolean',
    easeIn: 'boolean',
    easeOut: 'boolean',
    // eslint-disable-next-line max-len
    // @enum Quadratic, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular, Elastic, Back, Bounce
    // @condition easeIn=true|easeOut=true
    easingFunction: 'string',
    // @label Target (optional)
    target: 'eid',
  },
  schemaDefaults: {
    shortestPath: true,
    duration: 1000,
    loop: true,
  },
  data: {
    start: 'ui32',
    startX: 'f32',
    startY: 'f32',
    startZ: 'f32',
  },
  add: (world, component) => {
    if (component.schema.autoFrom) {
      setupVector3Component(world, component, getRotationCursorAsEuler)
    } else {
      component.data.startX = degreesToRadians(component.schema.fromX)
      component.data.startY = degreesToRadians(component.schema.fromY)
      component.data.startZ = degreesToRadians(component.schema.fromZ)
    }
  },
  tick: (world, component) => {
    const {schema} = component
    const {data} = component

    const progress = linearProgress(world, component)
    const result = easedProgress(progress, schema.easeIn, schema.easeOut, schema.easingFunction)

    const toX = degreesToRadians(schema.toX)
    const toY = degreesToRadians(schema.toY)
    const toZ = degreesToRadians(schema.toZ)

    if (schema.shortestPath) {
      // Take the shortest path to the target rotation, using quaternions to avoid gimbal issues.
      convertEulerToQuaternion(data.startX, data.startY, data.startZ, tempRotation)
      convertEulerToQuaternion(toX, toY, toZ, targetRotation)
      tempRotation.slerp(targetRotation, result)
    } else {
      // Interpolate between the two euler angles directly. This lets us model progress with angles
      // that are greater than 360 degrees. The trade-off is that gimbal lock can occur with large
      // differences in angles, and the interpolation may not take the shortest path.
      tempEuler.set(
        calculateProgress(data.startX, toX, result),
        calculateProgress(data.startY, toY, result),
        calculateProgress(data.startZ, toZ, result),
        EULER_ORDER
      )

      tempRotation.setFromEuler(tempEuler)
    }

    const cursor = getRotationCursor(world, component.eid, schema)
    if (cursor) {
      cursor.x = tempRotation.x
      cursor.y = tempRotation.y
      cursor.z = tempRotation.z
      cursor.w = tempRotation.w
    }

    if (progress === 1) {
      // Animation is complete, can remove.
      RotateAnimation.remove(world, component.eid)
      world.events.dispatch(component.eid, events.ROTATE_ANIMATION_COMPLETE)
    }
  },
})

const CustomVec3Animation = registerComponent({
  name: 'custom-vec3-animation',
  schema: {
    // @attribute vector3
    attribute: 'string',
    autoFrom: 'boolean',
    // @group start from:vector3
    // @group condition autoFrom=false
    fromX: 'f32',
    fromY: 'f32',
    fromZ: 'f32',
    // @group end
    // @group start to:vector3
    toX: 'f32',
    toY: 'f32',
    toZ: 'f32',
    // @group end
    duration: 'f32',
    loop: 'boolean',
    // @condition loop=true
    reverse: 'boolean',
    easeIn: 'boolean',
    easeOut: 'boolean',
    // eslint-disable-next-line max-len
    // @enum Quadratic, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular, Elastic, Back, Bounce
    // @condition easeIn=true|easeOut=true
    easingFunction: 'string',
    // @label Target (optional)
    target: 'eid',
  },
  schemaDefaults: {
    duration: 1000,
    loop: true,
  },
  data: {
    start: 'ui32',
    startX: 'f32',
    startY: 'f32',
    startZ: 'f32',
  },
  add: (world, component) => {
    setupVector3Component<Vector3AnimationSchema & {attribute: string}>(
      world, component, getVector3AttributeCursor
    )
  },
  tick: (world, component) => {
    const {schema} = component
    const cursor = getVector3AttributeCursor(world, component.eid, schema)

    const progress = linearProgress(world, component)
    updateVector3Component(world, component, progress, cursor)
    if (progress === 1) {
      // Animation is complete, can remove.
      CustomVec3Animation.remove(world, component.eid)
      world.events.dispatch(component.eid, events.CUSTOM_VEC3_ANIMATION_COMPLETE)
    }
  },
})

const CustomPropertyAnimation = registerComponent({
  name: 'custom-property-animation',
  schema: {
    // @attribute number
    attribute: 'string',
    // @property-of attribute
    property: 'string',
    autoFrom: 'boolean',
    // @condition autoFrom=false
    from: 'f32',
    to: 'f32',
    duration: 'f32',
    loop: 'boolean',
    // @condition loop=true
    reverse: 'boolean',
    easeIn: 'boolean',
    easeOut: 'boolean',
    // eslint-disable-next-line max-len
    // @enum Quadratic, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular, Elastic, Back, Bounce
    // @condition easeIn=true|easeOut=true
    easingFunction: 'string',
    // @label Target (optional)
    target: 'eid',
  },
  schemaDefaults: {
    duration: 1000,
    loop: true,
  },
  data: {
    start: 'ui32',
    startFrom: 'f32',
  },
  add: (world, component) => {
    component.data.start = world.time.elapsed

    const {schema} = component
    const cursor = schema.autoFrom ? getAttributeCursor(world, component.eid, schema) : null
    component.data.startFrom = cursor ? cursor[schema.property] : schema.from
  },
  tick: (world, component) => {
    const {schema} = component
    const cursor = getAttributeCursor(world, component.eid, schema)
    if (!cursor) {
      return
    }

    const progress = linearProgress(world, component)
    const result = easedProgress(progress, schema.easeIn, schema.easeOut, schema.easingFunction)
    cursor[schema.property] = calculateProgress(component.data.startFrom, schema.to, result)
    if (progress === 1) {
      // Animation is complete, can remove.
      CustomPropertyAnimation.remove(world, component.eid)
      world.events.dispatch(component.eid, events.CUSTOM_PROPERTY_ANIMATION_COMPLETE)
    }
  },
})

const getElasticMoveDistance = (
  relativeDistance: number, elasticity: number, timeDelta: number
) => {
  // If elasticity is 0 or less, return the full distance (no elasticity)
  if (elasticity <= 0) {
    return relativeDistance
  }

  // Otherwise, return the distance with elasticity applied.
  // We want useful values for elasticity to be roughly between 0 and 1.
  // A value of 1 for a frame that is one second long will move the object the full distance.
  // Frames are normally a fraction of a second, so 1 can be relied on to move most of the way
  // under normal conditions (when aggregated over multiple frames).
  const timeDeltaInSeconds = timeDelta / 1000
  return Math.min(relativeDistance, relativeDistance * elasticity * timeDeltaInSeconds)
}

// Get the position of an entity in world space.
const getWorldPosition = (world: World, eid: Eid, entityVec: THREE_TYPES.Vector3) => {
  world.getWorldTransform(eid, transform8)
  transform.fromArray(transform8.data())
  entityVec.setFromMatrixPosition(transform)
}

// Transform a move delta from world space to local space.
const transformWorldMoveDeltaToLocal = (world: World, eid: Eid, delta: THREE_TYPES.Vector3) => {
  world.getWorldTransform(eid, transform8)
  transform.fromArray(transform8.data())
  // Clear out any translation (this is not relevant for a delta or direction vector)
  transform.elements[12] = 0
  transform.elements[13] = 0
  transform.elements[14] = 0
  // Invert the transform to get the local space
  transform.invert()
  delta.applyMatrix4(transform)
}

// Move an entity by a given distance in a given direction in world space.
const movePosition = (
  world: World, eid: Eid, dir: THREE_TYPES.Vector3, signedMoveDist: number
) => {
  // Conbine direction and distance to get the move delta in world space
  moveDelta.x = dir.x * signedMoveDist
  moveDelta.y = dir.y * signedMoveDist
  moveDelta.z = dir.z * signedMoveDist

  // Transform the move delta to local space
  transformWorldMoveDeltaToLocal(world, world.getParent(eid), moveDelta)

  const cursor = Position.cursor(world, eid)
  cursor.x += moveDelta.x
  cursor.y += moveDelta.y
  cursor.z += moveDelta.z
}

const FollowAnimation = registerComponent({
  name: 'follow-animation',
  schema: {
    target: 'eid',
    minDistance: 'f32',
    maxDistance: 'f32',
    elasticity: 'f32',
    // TODO (jperrins): investigate other tween options like velocity/constant acceleration
  },
  schemaDefaults: {
    elasticity: 1,
  },
  tick: (world, component) => {
    const {schema} = component
    if (!schema.target) {
      return
    }

    // Get the entity position
    getWorldPosition(world, component.eid, entityPos)

    // Get the target position, and convert it to target direction and distance
    getWorldPosition(world, schema.target, targetDir)
    targetDir.sub(entityPos)
    const targetDist = targetDir.length()
    targetDir.normalize()

    const minDistance = Math.max(schema.minDistance, 0)
    const maxDistance = Math.max(schema.maxDistance, minDistance)

    let signedMoveDist = 0
    if (targetDist > maxDistance) {
      const distToClose = targetDist - maxDistance
      signedMoveDist = getElasticMoveDistance(distToClose, schema.elasticity, world.time.delta)
    } else if (targetDist < minDistance) {
      const distToOpen = minDistance - targetDist
      signedMoveDist = -getElasticMoveDistance(distToOpen, schema.elasticity, world.time.delta)
    }

    movePosition(world, component.eid, targetDir, signedMoveDist)
  },
})

const LookAtAnimation = registerComponent({
  name: 'look-at-animation',
  schema: {
    target: 'eid',
    // @group start targetVector:vector3
    // @group condition target=null
    targetX: 'f32',
    targetY: 'f32',
    targetZ: 'f32',
    // @group end
    lockX: 'boolean',
    lockY: 'boolean',
  },
  tick: (world, component) => {
    const {schema} = component

    if (!didWarnInvalidLock && !schema.lockX && schema.lockY) {
      didWarnInvalidLock = true
      // eslint-disable-next-line no-console
      console.warn('Unsupported lock combo (X unlocked, Y locked). Lock X or unlock Y to resolve.')
    }

    if (schema.lockY) {
      // NOTE(jeffha): currently unable to figure out a feasible way of getting the x rotation to
      //               be fully correct without the y rotation (in the event that the x axis is
      //               partially dependent on the y axis), otherwise it's invalid
      return
    }

    // Get the entity position
    getWorldPosition(world, component.eid, entityPos)

    // Get the target position, and convert it to target direction (in world space)
    if (schema.target) {
      getWorldPosition(world, schema.target, targetDir)
    } else {
      // If no target entity is set, use the target x, y and z
      targetDir.set(schema.targetX, schema.targetY, schema.targetZ)
    }

    // Get the target rotation (in world space)
    tempMatrix.lookAt(targetDir, entityPos, upVector)
    targetRotation.setFromRotationMatrix(tempMatrix)

    // Get the parent rotation (in world space)
    const parentId = world.getParent(component.eid)
    if (parentId) {
      world.getWorldTransform(parentId, transform8)
      transform.fromArray(transform8.data())
      transform.decompose(parentPos, parentRotation, parentScale)

      // Use the inverted parent rotation to convert target rotation to local space
      parentRotation.invert()
      targetRotation.premultiply(parentRotation)
    }

    const cursor = Quaternion.cursor(world, component.eid)
    if (schema.lockX) {
      tempRotation.set(cursor.x, cursor.y, cursor.z, cursor.w)

      // Convert current / target rotation to Euler angles
      //  since Euler angles represent rotations as separate angles around each axis
      //  unlike quaternions which represent rotations as a single, compact unit
      //  (each component of a quaternion can contribute to rotation around MULTIPLE axes)
      tempEuler.setFromQuaternion(tempRotation, EULER_ORDER)
      targetEuler.setFromQuaternion(targetRotation, EULER_ORDER)

      tempEuler.y = targetEuler.y

      // Convert back to quaternion once the rotation has been applied
      targetRotation.setFromEuler(tempEuler)
    }

    // Update entity with the final rotation
    cursor.x = targetRotation.x
    cursor.y = targetRotation.y
    cursor.z = targetRotation.z
    cursor.w = targetRotation.w
  },
})

export {
  CustomPropertyAnimation,
  PositionAnimation,
  ScaleAnimation,
  RotateAnimation,
  CustomVec3Animation,
  FollowAnimation,
  LookAtAnimation,
}
