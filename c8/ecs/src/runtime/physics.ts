import {asm} from './asm'
import type {World} from './world'
import type {Eid, OrderedSchema, ReadData} from '../shared/schema'
import {getDefaults} from './memory'
import {COLLISION_END_EVENT, COLLISION_START_EVENT, UPDATE_EVENT} from './physics-events'
import {
  DEFAULT_ANGULAR_DAMPING, DEFAULT_FRICTION, DEFAULT_LINEAR_DAMPING, DEFAULT_MASS,
  DEFAULT_RESTITUTION, DEFAULT_GRAVITY_FACTOR, DEFAULT_SIMPLIFICATION_MODE,
  DEFAULT_OFFSET_QUATERNION_X, DEFAULT_OFFSET_QUATERNION_Y, DEFAULT_OFFSET_QUATERNION_Z,
  DEFAULT_OFFSET_QUATERNION_W,
} from '../shared/physic-constants'
import type {RootAttribute} from './world-attribute'
import {createWorldAttribute} from './attribute'
import {createInstanced} from '../shared/instanced'
import {FLOAT_SIZE, UINT32_SIZE} from './constants'

// This schema should match PhysicsCollider in physics.h and colliderMemoryLayout
// in the same file.
const colliderSchema = {
  width: 'f32',
  height: 'f32',
  depth: 'f32',
  radius: 'f32',
  mass: 'f32',
  linearDamping: 'f32',
  angularDamping: 'f32',
  friction: 'f32',
  restitution: 'f32',
  gravityFactor: 'f32',
  offsetX: 'f32',
  offsetY: 'f32',
  offsetZ: 'f32',
  offsetQuaternionX: 'f32',
  offsetQuaternionY: 'f32',
  offsetQuaternionZ: 'f32',
  offsetQuaternionW: 'f32',
  shape: 'ui32',
  type: 'ui8',
  eventOnly: 'boolean',
  lockXPosition: 'boolean',
  lockYPosition: 'boolean',
  lockZPosition: 'boolean',
  lockXAxis: 'boolean',
  lockYAxis: 'boolean',
  lockZAxis: 'boolean',
  highPrecision: 'boolean',
  simplificationMode: 'string',
} as const

const ColliderType = {
  Static: 0,
  Dynamic: 1,
  Kinematic: 2,
} as const

const ColliderShape = {
  Box: 0,
  Sphere: 1,
  Plane: 2,
  Capsule: 3,
  Cone: 4,
  Cylinder: 5,
  Circle: 6,
} as const

type PhysicsColliderSchema = typeof colliderSchema

// NOTE(christoph): This corresponds with PhysicsCollider in physics.cc.
// The first 12 bytes are internal pointers.
const colliderMemoryLayout: OrderedSchema = [
  ['width', 'f32', 8],
  ['height', 'f32', 12],
  ['depth', 'f32', 16],
  ['radius', 'f32', 20],
  ['mass', 'f32', 24],
  ['linearDamping', 'f32', 28],
  ['angularDamping', 'f32', 32],
  ['friction', 'f32', 36],
  ['restitution', 'f32', 40],
  ['gravityFactor', 'f32', 44],
  ['offsetX', 'f32', 48],
  ['offsetY', 'f32', 52],
  ['offsetZ', 'f32', 56],
  ['offsetQuaternionX', 'f32', 60],
  ['offsetQuaternionY', 'f32', 64],
  ['offsetQuaternionZ', 'f32', 68],
  ['offsetQuaternionW', 'f32', 72],
  ['shape', 'ui32', 76],
  ['simplificationMode', 'string', 80],
  ['type', 'ui8', 84],
  ['eventOnly', 'boolean', 85],
  ['lockXAxis', 'boolean', 86],
  ['lockYAxis', 'boolean', 87],
  ['lockZAxis', 'boolean', 88],
  ['lockXPosition', 'boolean', 89],
  ['lockYPosition', 'boolean', 90],
  ['lockZPosition', 'boolean', 91],
  ['highPrecision', 'boolean', 92],
]

const colliderDefaults: ReadData<PhysicsColliderSchema> = getDefaults(colliderSchema, {
  type: ColliderType.Static,
  mass: DEFAULT_MASS,
  linearDamping: DEFAULT_LINEAR_DAMPING,
  angularDamping: DEFAULT_ANGULAR_DAMPING,
  friction: DEFAULT_FRICTION,
  restitution: DEFAULT_RESTITUTION,
  gravityFactor: DEFAULT_GRAVITY_FACTOR,
  offsetQuaternionX: DEFAULT_OFFSET_QUATERNION_X,
  offsetQuaternionY: DEFAULT_OFFSET_QUATERNION_Y,
  offsetQuaternionZ: DEFAULT_OFFSET_QUATERNION_Z,
  offsetQuaternionW: DEFAULT_OFFSET_QUATERNION_W,
  simplificationMode: DEFAULT_SIMPLIFICATION_MODE,
})

type ColliderSchema = typeof colliderSchema

const colliderForWorld = createInstanced((world: World) => {
  const id = asm.getColliderComponentId(world._id)
  return createWorldAttribute<ColliderSchema>(
    world, id, colliderMemoryLayout, colliderDefaults
  )
})

const Collider: RootAttribute<ColliderSchema> = {
  set: (world, eid, data) => colliderForWorld(world).set(eid, data),
  reset: (world, eid) => colliderForWorld(world).reset(eid),
  dirty: (world, eid) => colliderForWorld(world).dirty(eid),
  get: (world, eid) => colliderForWorld(world).get(eid),
  cursor: (world, eid) => colliderForWorld(world).cursor(eid),
  acquire: (world, eid) => colliderForWorld(world).acquire(eid),
  commit: (world, eid) => colliderForWorld(world).commit(eid),
  mutate: (world, eid, fn) => colliderForWorld(world).mutate(eid, fn),
  has: (world, eid) => colliderForWorld(world).has(eid),
  remove: (world, eid) => colliderForWorld(world).remove(eid),
  forWorld: colliderForWorld,
  schema: colliderSchema,
  orderedSchema: colliderMemoryLayout,
  defaults: colliderDefaults,
}

const setWorldGravity = (world: World, gravity: number) => {
  if (!asm.setWorldGravity(world._id, gravity)) {
    throw new Error('Failed to set world gravity')
  }
}

const getWorldGravity = (world: World): number => asm.getWorldGravity(world._id)

const setLinearVelocity = (
  world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number
) => {
  if (!asm.setLinearVelocityToEntity(world._id, eid, velocityX, velocityY, velocityZ)) {
    throw new Error(
      `Entity ${eid} fail to set linear velocity, there may not be a collider on the entity`
    )
  }
}

const getLinearVelocity = (world: World, eid: Eid) => {
  const velocityPtr = asm.getLinearVelocityFromEntity(world._id, eid)

  if (!velocityPtr) {
    throw new Error(`Entity ${eid} does not have a collider`)
  }

  const dataView = new DataView(asm.HEAPF32.buffer)
  const x = dataView.getFloat32(velocityPtr, true)
  const y = dataView.getFloat32(velocityPtr + FLOAT_SIZE, true)
  const z = dataView.getFloat32(velocityPtr + (2 * FLOAT_SIZE), true)
  asm._free(velocityPtr)
  return {x, y, z}
}

const setAngularVelocity = (
  world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number
) => {
  if (!asm.setAngularVelocityToEntity(world._id, eid, velocityX, velocityY, velocityZ)) {
    throw new Error(
      `Entity ${eid} fail to set angular velocity, there may not be a collider on the entity`
    )
  }
}

const getAngularVelocity = (world: World, eid: Eid) => {
  const velocityPtr = asm.getAngularVelocityFromEntity(world._id, eid)

  if (!velocityPtr) {
    throw new Error(
      `Entity ${eid} fail to get angular velocity, there may not be a collider on the entity`
    )
  }

  const dataView = new DataView(asm.HEAPF32.buffer)
  const x = dataView.getFloat32(velocityPtr, true)
  const y = dataView.getFloat32(velocityPtr + FLOAT_SIZE, true)
  const z = dataView.getFloat32(velocityPtr + (2 * FLOAT_SIZE), true)
  asm._free(velocityPtr)
  return {x, y, z}
}

const applyForce = (world: World, eid: Eid, forceX: number, forceY: number, forceZ: number) => {
  if (!asm.applyForceToEntity(world._id, eid, forceX, forceY, forceZ)) {
    throw new Error(`Entity ${eid} fail to apply force, there may not be a collider on the entity`)
  }
}

const applyImpulse = (world: World, eid: Eid,
  impulseX: number, impulseY: number, impulseZ: number) => {
  if (!asm.applyImpulseToEntity(world._id, eid, impulseX, impulseY, impulseZ)) {
    throw new
    Error(`Entity ${eid} fail to apply impulse, there may not be a collider on the entity`)
  }
}

const applyTorque = (world: World, eid: Eid, torqueX: number, torqueY: number, torqueZ: number) => {
  if (!asm.applyTorqueToEntity(world._id, eid, torqueX, torqueY, torqueZ)) {
    throw new Error(`Entity ${eid} fail to apply torque, there may not be a collider on the entity`)
  }
}

const registerConvexShape = (world: World, vertices: Float32Array): number => {
  const verticesPtr = asm._malloc(FLOAT_SIZE * vertices.length)
  asm.HEAPF32.set(vertices, verticesPtr / FLOAT_SIZE)
  const id = asm.registerConvexShape(world._id, verticesPtr, vertices.length)
  asm._free(verticesPtr)
  if (id === 0) {
    throw new Error('Failed to register convex shape, invalid vertices?')
  }

  return id
}

const registerCompoundShape = (
  world: World, vertices: Float32Array, indices: Uint32Array
): number => {
  const verticesPtr = asm._malloc(FLOAT_SIZE * vertices.length)
  const indicesPtr = asm._malloc(UINT32_SIZE * indices.length)

  asm.HEAPF32.set(vertices, verticesPtr / FLOAT_SIZE)
  asm.HEAPU32.set(indices, indicesPtr / UINT32_SIZE)

  const id = (asm as any).registerCompoundShape(
    world._id, verticesPtr, vertices.length, indicesPtr, indices.length
  )

  asm._free(verticesPtr)
  asm._free(indicesPtr)

  if (id === 0 || id === -1) {
    throw new Error('Failed to register compound shape, invalid mesh data or decomposition failed')
  }

  return id
}

const unregisterConvexShape = (world: World, id: number) => {
  asm.unregisterConvexShape(world._id, id)
}

const enable = (world: World) => {
  const result = asm.togglePhysics(world._id, true)
  if (!result) {
    throw new Error('Failed to enable physics')
  }
}

const disable = (world: World) => {
  const result = asm.togglePhysics(world._id, false)
  if (!result) {
    throw new Error('Failed to disable physics')
  }
}

const physics = {
  enable,
  disable,
  setWorldGravity,
  getWorldGravity,
  applyForce,
  applyImpulse,
  applyTorque,
  setLinearVelocity,
  getLinearVelocity,
  setAngularVelocity,
  getAngularVelocity,
  registerConvexShape,
  registerCompoundShape,
  unregisterConvexShape,
  COLLISION_START_EVENT,
  COLLISION_END_EVENT,
  UPDATE_EVENT,
  ColliderShape,
  ColliderType,
}

export {
  physics,
  ColliderType,
  ColliderShape,
  Collider,
}
