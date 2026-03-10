// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, assert, beforeEach, afterEach} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

type GeometryComponent = {
  component: ecs.RootAttribute<{}>
  type: string
  data?: {}
}

// TODO(christoph): For some reason even though entities start without a geometry, clearing an
// objects geometry causes a crash.
const EMPTY_GEOMETRY_TYPE = 'BufferGeometry'

const GEOMETRIES: GeometryComponent[] = [
  {component: ecs.SphereGeometry, type: 'SphereGeometry'},
  {component: ecs.BoxGeometry, type: 'BoxGeometry'},
  {component: ecs.PlaneGeometry, type: 'PlaneGeometry'},
  {component: ecs.CapsuleGeometry, type: 'CapsuleGeometry', data: {radius: 0.5, height: 1}},
  {component: ecs.ConeGeometry, type: 'ConeGeometry'},
  {component: ecs.CylinderGeometry, type: 'CylinderGeometry'},
  {component: ecs.TetrahedronGeometry, type: 'TetrahedronGeometry'},
  {component: ecs.PolyhedronGeometry, type: 'PolyhedronGeometry', data: {faces: 20}},
  {component: ecs.CircleGeometry, type: 'CircleGeometry'},
  {component: ecs.RingGeometry, type: 'RingGeometry'},
  {component: ecs.TorusGeometry, type: 'TorusGeometry'},
]

describe('Geometry Systems', () => {
  let world: ecs.World

  const assertGeometryType = (eid: ecs.Eid, type: string) => {
    const object = world.three.entityToObject.get(eid) as any
    assert.isDefined(object.geometry)
    assert.strictEqual(object.geometry.type, type)
  }

  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
    world.tick()
  })

  afterEach(() => {
    world.destroy()
  })

  GEOMETRIES.forEach((geometry) => {
    it(`Can create a ${geometry.type}`, () => {
      const eid = world.createEntity()
      geometry.component.set(world, eid, {...geometry.data})
      world.tick()
      assertGeometryType(eid, geometry.type)
    })

    it('can be removed', () => {
      const eid = world.createEntity()
      geometry.component.set(world, eid, {...geometry.data})
      world.tick()

      assertGeometryType(eid, geometry.type)
      geometry.component.remove(world, eid)
      world.tick()
      assertGeometryType(eid, EMPTY_GEOMETRY_TYPE)
    })

    GEOMETRIES
      .filter(e => e.type !== geometry.type)
      .forEach((nextGeometry) => {
        it(`Can switch from ${geometry.type} to ${nextGeometry.type}`, () => {
          const eid = world.createEntity()
          geometry.component.set(world, eid, {...geometry.data})
          world.tick()

          assertGeometryType(eid, geometry.type)
          geometry.component.remove(world, eid)
          nextGeometry.component.set(world, eid, {...nextGeometry.data})
          world.tick()
          assertGeometryType(eid, nextGeometry.type)
        })
      })
  })
})
