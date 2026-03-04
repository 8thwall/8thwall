// @attr(testonly = 1)

import {assert} from '@nia/bzl/js/chai-js'

import {ecs} from './test-runtime-lib'
import type {QuatSource} from './math/quat'
import type {Eid} from '../shared/schema'
import type {Mat4, Vec3Source} from './math/math'
import type {World} from './world'
import THREE from './three'
import {radiansToDegrees} from '../shared/angle-conversion'

const formatQuatAsEuler = (quat: QuatSource) => {
  const euler = new THREE.Euler().setFromQuaternion(new THREE.Quaternion().copy(quat), 'YXZ')
  return `(pitch: ${radiansToDegrees(euler.x).toFixed(0)}, ` +
    `yaw: ${radiansToDegrees(euler.y).toFixed(0)}, ` +
    `roll: ${radiansToDegrees(euler.z).toFixed(0)})`
}

const formatVec3 = (v: Vec3Source) => (
  `(${v.x.toFixed(2)}, ${v.y.toFixed(2)}, ${v.z.toFixed(2)})`
)
const formatQuat = (q: QuatSource) => (
  `(${q.x.toFixed(2)}, ${q.y.toFixed(2)}, ${q.z.toFixed(2)}, ${q.w.toFixed(2)})`
)

const assertVec3Equal = (actual: Vec3Source, expected: Vec3Source) => {
  assert.closeTo(actual.x, expected.x, 1e-5)
  assert.closeTo(actual.y, expected.y, 1e-5)
  assert.closeTo(actual.z, expected.z, 1e-5)
}

const assertQuatEqual = (actual: QuatSource, expected: QuatSource, _message?: string) => {
  const message = _message ? ` (${_message})` : ''
  assert.closeTo(
    ecs.math.quat.from(expected).dot(actual), 1, 1e-5,
    `Quaternion: ${formatQuat(expected)} vs Actual: ${formatQuat(actual)}${message}, in euler: \
${formatQuatAsEuler(expected)} vs Actual: ${formatQuatAsEuler(actual)}`
  )
}

const assertPosition = (world: World, eid: Eid, expected: Vec3Source, _message?: string) => {
  const actual = ecs.Position.get(world, eid)
  const message = _message ? ` (${_message})` : ''
  assert.closeTo(actual.x, expected.x, 1e-5, `Position X${message}`)
  assert.closeTo(actual.y, expected.y, 1e-5, `Position Y${message}`)
  assert.closeTo(actual.z, expected.z, 1e-5, `Position Z${message}`)
}

const assertQuaternion = (world: World, eid: Eid, expected: QuatSource, message?: string) => {
  const actual = ecs.Quaternion.get(world, eid)
  assertQuatEqual(actual, expected, message)
}

const assertScale = (world: World, eid: Eid, expected: Vec3Source, _message?: string) => {
  const actual = ecs.Scale.get(world, eid)
  const message = _message ? ` (${_message})` : ''
  assert.closeTo(actual.x, expected.x, 1e-5, `Scale X${message}`)
  assert.closeTo(actual.y, expected.y, 1e-5, `Scale Y${message}`)
  assert.closeTo(actual.z, expected.z, 1e-5, `Scale Z${message}`)
}

const assertMat4Equal = (actual: Mat4, expected: Mat4) => {
  const actualData = actual.data()
  const expectedData = expected.data()

  try {
    for (let i = 0; i < 16; i++) {
      assert.closeTo(actualData[i], expectedData[i], 1e-5, `Mat4[${i}]`)
    }
  } catch (err) {
    const actualTrs = actual.decomposeTrs()
    const expectedTrs = expected.decomposeTrs()

    if (actualTrs.t.minus(expectedTrs.t).length() > 1e-5) {
      // eslint-disable-next-line no-console
      console.warn('actual translation differs from expected translation',
        formatVec3(actualTrs.t),
        formatVec3(expectedTrs.t))
    }

    if (actualTrs.r.dot(expectedTrs.r) < 0.999) {
      // eslint-disable-next-line no-console
      console.warn('actual rotation differs from expected rotation',
        formatQuat(actualTrs.r),
        formatQuat(expectedTrs.r))
    }

    if (actualTrs.s.minus(expectedTrs.s).length() > 1e-5) {
      // eslint-disable-next-line no-console
      console.warn('actual scale differs from expected scale',
        formatVec3(actualTrs.s),
        formatVec3(expectedTrs.s))
    }

    throw err
  }
}

export {
  formatVec3,
  formatQuat,
  assertVec3Equal,
  assertQuatEqual,
  assertPosition,
  assertQuaternion,
  assertScale,
  assertMat4Equal,
}
