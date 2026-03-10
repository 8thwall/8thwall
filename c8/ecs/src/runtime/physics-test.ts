// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, beforeEach, afterEach, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'

describe('ecs.physics.registerConvexShape', () => {
  let world: World

  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  it('does not have overlap between custom shape IDs and built-in shapes', async () => {
    const customShape = ecs.physics.registerConvexShape(world, new Float32Array([
      -1, -1, -1,
      1, -1, -1,
      1, 1, -1,
      -1, 1, -1,
      -1, -1, 1,
      1, -1, 1,
    ]))

    const maxBuiltInShapeId = Math.max(...Object.values(ecs.physics.ColliderShape))

    assert.isAbove(customShape, maxBuiltInShapeId)
  })
})

describe('Physics collision events', () => {
  let world: World

  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  it('fires collision events with event constants', () => {
    const dynamicEntity = world.getEntity(world.createEntity())
    const staticEntity = world.getEntity(world.createEntity())

    let collisionStartCount = 0
    let collisionEndCount = 0

    const handleCollisionStart = () => {
      collisionStartCount++
    }

    const handleCollisionEnd = () => {
      collisionEndCount++
    }

    world.events.addListener(
      dynamicEntity.eid, ecs.physics.COLLISION_START_EVENT, handleCollisionStart
    )
    world.events.addListener(
      dynamicEntity.eid, ecs.physics.COLLISION_END_EVENT, handleCollisionEnd
    )

    // Verify the constants have correct values
    assert.equal(ecs.physics.COLLISION_START_EVENT, 'physics-collision-start')
    assert.equal(ecs.physics.COLLISION_END_EVENT, 'physics-collision-end')

    // Position entities to collide
    dynamicEntity.setLocalPosition({x: 0, y: 5, z: 0})
    staticEntity.setLocalPosition({x: 0, y: 0, z: 0})

    // Set up dynamic collider
    dynamicEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    // Set up static collider
    staticEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 10,
      height: 1,
      depth: 10,
      mass: 0,
      type: ecs.ColliderType.Static,
    })

    let time = 0
    // Run simulation until collision occurs
    for (let i = 0; i < 100; i++) {
      world.tick(time += 16)
      if (collisionStartCount > 0) break
    }

    assert.equal(collisionStartCount, 1, 'Collision start event should fire once')

    // Run more simulation to trigger collision end
    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionEndCount > 0) break
    }

    assert.equal(collisionEndCount, 1, 'Collision end event should fire once')
  })

  it('fires collision events on dynamic locked entity', () => {
    const staticFloor = world.getEntity(world.createEntity())
    const lockedBall = world.getEntity(world.createEntity())

    let collisionStartCount = 0
    let collisionEndCount = 0

    const handleCollisionStart = () => {
      collisionStartCount++
    }

    const handleCollisionEnd = () => {
      collisionEndCount++
    }

    // Assign listeners to the locked ball instead
    world.events.addListener(
      lockedBall.eid, ecs.physics.COLLISION_START_EVENT, handleCollisionStart
    )
    world.events.addListener(
      lockedBall.eid, ecs.physics.COLLISION_END_EVENT, handleCollisionEnd
    )

    // Position entities
    staticFloor.setLocalPosition({x: 0, y: 0, z: 0})

    // Set up static floor
    staticFloor.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 10,
      height: 1,
      depth: 10,
      mass: 0,
      type: ecs.ColliderType.Static,
      eventOnly: true,
    })

    // Set up locked ball as dynamic but with all axes locked
    lockedBall.set(ecs.Collider, {
      shape: ecs.ColliderShape.Sphere,
      radius: 0.5,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      lockXPosition: true,
      lockYPosition: true,
      lockZPosition: true,
      lockXAxis: true,
      lockYAxis: true,
      lockZAxis: true,
    })

    lockedBall.setLocalPosition({x: 0, y: 0.9, z: 0})
    let time = 0
    // Run simulation until collision occurs
    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionStartCount > 0) break
    }

    assert.equal(collisionStartCount, 1, 'Collision start event should fire on locked ball')

    // Move locked ball away to trigger collision end
    lockedBall.setLocalPosition({x: 0, y: 10, z: 0})

    // Run more simulation to trigger collision end
    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionEndCount > 0) break
    }

    assert.equal(collisionEndCount, 1, 'Collision end event should fire on locked ball')
  })

  it('handles collider removal while colliding', () => {
    const entity1 = world.getEntity(world.createEntity())
    const entity2 = world.getEntity(world.createEntity())

    let collisionEnterCount = 0
    let collisionExitCount = 0

    world.events.addListener(entity1.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionEnterCount++
    })
    world.events.addListener(entity1.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionExitCount++
    })

    let time = 10

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'no collisions yet')

    entity1.setLocalPosition({x: 0, y: 0, z: 0})
    entity2.setLocalPosition({x: 0, y: 0, z: 1})

    entity1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Sphere,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    entity2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'too far initially')

    entity2.setLocalPosition({x: 0, y: 0, z: 0.5})
    world.tick(time += 10)
    assert.equal(collisionEnterCount, 1, 'collision dispatched immediately on move')

    entity2.remove(ecs.Collider)
    world.tick(time += 10)
    assert.equal(collisionExitCount, 1, 'exit dispatched immediately on remove')
  })

  it('handles collider disabling while colliding', () => {
    const entity1 = world.getEntity(world.createEntity())
    const entity2 = world.getEntity(world.createEntity())

    let collisionEnterCount = 0
    let collisionExitCount = 0

    world.events.addListener(entity1.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionEnterCount++
    })
    world.events.addListener(entity1.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionExitCount++
    })

    let time = 10

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'no collisions yet')

    entity1.setLocalPosition({x: 0, y: 0, z: 0})
    entity2.setLocalPosition({x: 0, y: 0, z: 1})

    entity1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Sphere,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    entity2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'too far initially')

    entity2.setLocalPosition({x: 0, y: 0, z: 0.5})
    world.tick(time += 10)
    assert.equal(collisionEnterCount, 1, 'collision dispatched immediately on move')

    entity2.disable()
    world.tick(time += 10)
    assert.equal(collisionExitCount, 1, 'exit dispatched immediately on remove')
  })

  it('kinematic objects generate collision events with static objects', () => {
    const kinematicEntity = world.getEntity(world.createEntity())
    const staticEntity = world.getEntity(world.createEntity())

    let collisionStartCount = 0
    let collisionEndCount = 0

    world.events.addListener(kinematicEntity.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionStartCount++
    })
    world.events.addListener(kinematicEntity.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionEndCount++
    })

    // Position entities
    kinematicEntity.setLocalPosition({x: 0, y: 10, z: 0})
    staticEntity.setLocalPosition({x: 0, y: 0, z: 0})

    // Set up kinematic collider
    kinematicEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      type: ecs.ColliderType.Kinematic,
    })

    // Set up static collider
    staticEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 10,
      height: 1,
      depth: 10,
      type: ecs.ColliderType.Static,
    })

    let time = 0
    world.tick(time += 16)
    assert.equal(collisionStartCount, 0, 'no collision yet')

    // Move kinematic into collision
    kinematicEntity.setLocalPosition({x: 0, y: 0.6, z: 0})
    world.tick(time += 16)

    assert.equal(collisionStartCount, 1, 'Kinematic-Static collision start should fire')

    // Move kinematic away
    kinematicEntity.setLocalPosition({x: 0, y: 10, z: 0})
    world.tick(time += 16)

    // callback will run on the next frame
    world.tick(time += 16)

    assert.equal(collisionEndCount, 1, 'Kinematic-Static collision end should fire')
  })

  it('kinematic objects generate collision events with other kinematic objects', () => {
    const kinematic1 = world.getEntity(world.createEntity())
    const kinematic2 = world.getEntity(world.createEntity())

    let collisionStartCount1 = 0
    let collisionStartCount2 = 0
    let collisionEndCount1 = 0
    let collisionEndCount2 = 0

    world.events.addListener(kinematic1.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionStartCount1++
    })
    world.events.addListener(kinematic2.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionStartCount2++
    })

    world.events.addListener(kinematic1.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionEndCount1++
    })
    world.events.addListener(kinematic2.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionEndCount2++
    })

    // Position entities apart
    kinematic1.setLocalPosition({x: 0, y: 0, z: 0})
    kinematic2.setLocalPosition({x: 10, y: 0, z: 0})

    // Set up both as kinematic colliders
    kinematic1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      type: ecs.ColliderType.Kinematic,
    })

    kinematic2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      type: ecs.ColliderType.Kinematic,
    })

    let time = 0
    world.tick(time += 16)
    assert.equal(collisionStartCount1, 0, 'no collision yet')
    assert.equal(collisionStartCount2, 0, 'no collision yet')

    // Move kinematic2 into collision with kinematic1
    kinematic2.setLocalPosition({x: 0, y: 0, z: 0})
    world.tick(time += 16)

    assert.equal(collisionStartCount1, 1, 'Kinematic-Kinematic collision should fire on start 1')
    assert.equal(collisionStartCount2, 1, 'Kinematic-Kinematic collision should fire on start 2')

    // Move kinematic2 away to end collision
    kinematic2.setLocalPosition({x: 10, y: 0, z: 0})
    world.tick(time += 16)

    // callback will run on the next frame
    world.tick(time += 16)

    assert.equal(collisionEndCount1, 1, 'Kinematic-Kinematic collision should fire on end 1')
    assert.equal(collisionEndCount2, 1, 'Kinematic-Kinematic collision should fire on end 2')
  })

  it('kinematic objects generate collision events with other dynamic objects', () => {
    const kinematic = world.getEntity(world.createEntity())
    const dynamic = world.getEntity(world.createEntity())

    let collisionStartCount1 = 0
    let collisionStartCount2 = 0
    let collisionEndCount1 = 0
    let collisionEndCount2 = 0

    world.events.addListener(kinematic.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionStartCount1++
    })
    world.events.addListener(dynamic.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionStartCount2++
    })

    world.events.addListener(kinematic.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionEndCount1++
    })
    world.events.addListener(dynamic.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionEndCount2++
    })

    // Position entities apart
    kinematic.setLocalPosition({x: 0, y: 0, z: 0})
    dynamic.setLocalPosition({x: 10, y: 0, z: 0})

    // Set up both as kinematic colliders
    kinematic.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      type: ecs.ColliderType.Kinematic,
      eventOnly: true,
    })

    dynamic.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 1,
      height: 1,
      depth: 1,
      type: ecs.ColliderType.Dynamic,
    })

    let time = 0
    world.tick(time += 16)
    assert.equal(collisionStartCount1, 0, 'no collision yet')
    assert.equal(collisionStartCount2, 0, 'no collision yet')

    // Move Dynamic into collision with kinematic
    dynamic.setLocalPosition({x: 0, y: 0, z: 0})
    world.tick(time += 16)

    assert.equal(collisionStartCount1, 1, 'Kinematic-Dynamic collision should fire on Kinematic')
    assert.equal(collisionStartCount2, 1, 'Kinematic-Dynamic collision should fire on Dynamic')

    // Move Dynamic away to end collision
    dynamic.setLocalPosition({x: 10, y: 0, z: 0})
    world.tick(time += 16)

    // callback will run on the next frame
    world.tick(time += 16)

    assert.equal(collisionEndCount1, 1, 'Kinematic-Dynamic collision should fire on end Kinematic')
    assert.equal(collisionEndCount2, 1, 'Kinematic-Dynamic collision should fire on end Dynamic')
  })

  it('flipped cone settles at correct angle with quaternion offset', () => {
    const coneEntity = world.getEntity(world.createEntity())
    const floorEntity = world.getEntity(world.createEntity())

    let collisionStartCount = 0

    const handleCollisionStart = () => {
      collisionStartCount++
    }

    world.events.addListener(
      coneEntity.eid, ecs.physics.COLLISION_START_EVENT, handleCollisionStart
    )

    // Create static floor
    floorEntity.setLocalPosition({x: 0, y: -1, z: 0})
    floorEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      width: 20,
      height: 1,
      depth: 20,
      mass: 0,
      type: ecs.ColliderType.Static,
    })

    const coneRadius = 0.5
    const coneHeight = 4

    coneEntity.setLocalPosition({x: 0.1, y: 5, z: 0})

    // Add small perturbation to make it unstable
    coneEntity.setWorldQuaternion(ecs.math.quat.xDegrees(5))

    const quaternionFlipped = ecs.math.quat.xDegrees(180)

    coneEntity.set(ecs.Collider, {
      shape: ecs.ColliderShape.Cone,
      radius: coneRadius,
      height: coneHeight,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      offsetQuaternionX: quaternionFlipped.x,
      offsetQuaternionY: quaternionFlipped.y,
      offsetQuaternionZ: quaternionFlipped.z,
      offsetQuaternionW: quaternionFlipped.w,
      friction: 0.8,
      restitution: 0.1,
      angularDamping: 0.3,
      linearDamping: 0.1,
    })

    let time = 0

    for (let i = 0; i < 300; i++) {
      world.tick(time += 16)
      if (collisionStartCount > 0) break
    }

    assert.equal(collisionStartCount, 1, 'Flipped cone should collide with floor')

    for (let i = 0; i < 1000; i++) {
      world.tick(time += 16)
    }

    const finalPosition = coneEntity.get(ecs.Position)

    // Calculate the cone's local Y axis in world space
    const yAxis = ecs.math.vec3.xyz(0, 1, 0)
    coneEntity.getWorldQuaternion().timesVec(yAxis, yAxis)

    // Calculate the expected settling angle for the cone
    const coneAngle = Math.atan2(coneRadius, coneHeight)
    const expectedY = Math.sin(coneAngle)

    const angleDegrees = (coneAngle * 180) / Math.PI
    assert.isBelow(Math.abs(yAxis.y - expectedY), 0.02,
      `${yAxis.y} should be near ${expectedY} (expected angle: ${angleDegrees.toFixed(1)}°)`)

    // Also verify the cone has moved from its original position due to tipping
    assert.notEqual(finalPosition.x, 0.1, 'Cone should have moved horizontally when tipping')
  })

  it('rotated cylinders detect collision start and end with quaternion offset', () => {
    const cylinder1 = world.getEntity(world.createEntity())
    const cylinder2 = world.getEntity(world.createEntity())

    let collisionStartCount = 0
    let collisionEndCount = 0

    const handleCollisionStart = () => {
      collisionStartCount++
    }

    const handleCollisionEnd = () => {
      collisionEndCount++
    }

    world.events.addListener(
      cylinder1.eid, ecs.physics.COLLISION_START_EVENT, handleCollisionStart
    )
    world.events.addListener(
      cylinder1.eid, ecs.physics.COLLISION_END_EVENT, handleCollisionEnd
    )

    cylinder1.setLocalPosition({x: -2, y: 0, z: 0})
    cylinder2.setLocalPosition({x: 2, y: 0, z: 0})

    const quaternion90Z = ecs.math.quat.zDegrees(90)

    cylinder1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Cylinder,
      radius: 0.2,
      height: 4.5,
      mass: 0,
      type: ecs.ColliderType.Kinematic,
      eventOnly: true,
      offsetQuaternionX: quaternion90Z.x,
      offsetQuaternionY: quaternion90Z.y,
      offsetQuaternionZ: quaternion90Z.z,
      offsetQuaternionW: quaternion90Z.w,
    })

    cylinder2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Cylinder,
      radius: 0.2,
      height: 4.5,
      mass: 0,
      type: ecs.ColliderType.Kinematic,
      eventOnly: true,
      offsetQuaternionX: quaternion90Z.x,
      offsetQuaternionY: quaternion90Z.y,
      offsetQuaternionZ: quaternion90Z.z,
      offsetQuaternionW: quaternion90Z.w,
    })

    let time = 0

    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionStartCount > 0) break
    }

    assert.equal(collisionStartCount, 1, 'Horizontally rotated cylinders should touch')

    const noQuaternion = ecs.math.quat.zero()
    cylinder1.set(ecs.Collider, {
      offsetQuaternionX: noQuaternion.x,
      offsetQuaternionY: noQuaternion.y,
      offsetQuaternionZ: noQuaternion.z,
      offsetQuaternionW: noQuaternion.w,
    })

    cylinder2.set(ecs.Collider, {
      offsetQuaternionX: noQuaternion.x,
      offsetQuaternionY: noQuaternion.y,
      offsetQuaternionZ: noQuaternion.z,
      offsetQuaternionW: noQuaternion.w,
    })

    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionEndCount > 0) break
    }

    assert.equal(collisionEndCount, 1, 'Cylinders should generate collision end when made parallel')
  })

  it('planes intersect with position and quaternion offset', () => {
    const plane1 = world.getEntity(world.createEntity())
    const plane2 = world.getEntity(world.createEntity())

    let collisionStartCount = 0
    let collisionEndCount = 0

    const handleCollisionStart = () => {
      collisionStartCount++
    }

    const handleCollisionEnd = () => {
      collisionEndCount++
    }

    world.events.addListener(
      plane1.eid, ecs.physics.COLLISION_START_EVENT, handleCollisionStart
    )
    world.events.addListener(
      plane1.eid, ecs.physics.COLLISION_END_EVENT, handleCollisionEnd
    )

    // Position planes 10 units apart on Z axis - impossible to touch without offsets
    plane1.setLocalPosition({x: 0, y: 0, z: 0})
    plane2.setLocalPosition({x: 0, y: 0, z: 10})

    // Create 4x4 plane shapes
    plane1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Plane,
      width: 4,
      height: 4,
      mass: 0,
      type: ecs.ColliderType.Kinematic,
      eventOnly: true,
    })

    const quaternion90Y = ecs.math.quat.yDegrees(90)

    plane2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Plane,
      width: 4,
      height: 4,
      mass: 0,
      type: ecs.ColliderType.Kinematic,
      eventOnly: true,
      offsetZ: -9,
      offsetQuaternionX: quaternion90Y.x,
      offsetQuaternionY: quaternion90Y.y,
      offsetQuaternionZ: quaternion90Y.z,
      offsetQuaternionW: quaternion90Y.w,
    })

    let time = 0

    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionStartCount > 0) break
    }

    assert.equal(collisionStartCount, 1, 'Planes should intersect with offsets')

    // Remove the quaternion offset to separate them again
    const noQuaternion = ecs.math.quat.zero()
    plane2.set(ecs.Collider, {
      offsetQuaternionX: noQuaternion.x,
      offsetQuaternionY: noQuaternion.y,
      offsetQuaternionZ: noQuaternion.z,
      offsetQuaternionW: noQuaternion.w,
    })

    for (let i = 0; i < 60; i++) {
      world.tick(time += 16)
      if (collisionEndCount > 0) break
    }

    assert.equal(collisionEndCount, 1, 'Planes should generate collision end when separated')
  })
})
