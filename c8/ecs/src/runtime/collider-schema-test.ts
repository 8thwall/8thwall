// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('Collider Test', () => {
  before(async () => {
    await ecs.ready()
  })
  it('Has the correct defaults', async () => {
    const world = ecs.createWorld(...initThree())

    const entity = world.createEntity()

    ecs.Collider.reset(world, entity)

    const data = {...ecs.Collider.get(world, entity)}

    assert.deepStrictEqual(data, {
      width: 0,
      height: 0,
      depth: 0,
      radius: 0,
      offsetX: 0,
      offsetY: 0,
      offsetZ: 0,
      offsetQuaternionX: 0,
      offsetQuaternionY: 0,
      offsetQuaternionZ: 0,
      offsetQuaternionW: 1,
      type: 0,
      shape: 0,
      eventOnly: false,
      lockXAxis: false,
      lockYAxis: false,
      lockZAxis: false,
      lockXPosition: false,
      lockYPosition: false,
      lockZPosition: false,
      highPrecision: false,
      mass: 0,
      linearDamping: 0,
      angularDamping: 0,
      friction: 0.5,
      restitution: 0,
      gravityFactor: 1,
      simplificationMode: 'convex',
    }, 'Collider defaults should be correct')
  })
})
