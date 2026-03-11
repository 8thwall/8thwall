import chai from 'chai'

import {
  computePixelPointsFromRadius,
} from '../src/client/apps/image-targets/conical/unconify'

chai.should()
const {assert} = chai

const ROOT_2 = Math.sqrt(2)
const ROOT_3 = Math.sqrt(3)

const assertCloseEnough = (actual, expected, message) => {
  assert.closeTo(actual, expected, Math.abs(expected / 100), message)
}

const assertPointsEqual = (actual, expected, message) => {
  assertCloseEnough(actual.x, expected.x, `${message} x`)
  assertCloseEnough(actual.y, expected.y, `${message} y`)
}

const assertPixelPointsEqual = (actual, expected) => {
  assertPointsEqual(actual.tl, expected.tl, 'tl')
  assertPointsEqual(actual.tr, expected.tr, 'tr')
  assertPointsEqual(actual.bl, expected.bl, 'bl')
  assertPointsEqual(actual.apex, expected.apex, 'apex')
  assertCloseEnough(actual.theta, expected.theta, 'theta')
  assert.strictEqual(actual.isFez, expected.isFez, 'isFex')
}

describe('computePixelPointsFromRadius', () => {
  it('Returns correct values for a 180 degree rainbow', () => {
    // 180 degree rainbow
    assertPixelPointsEqual(computePixelPointsFromRadius(100, 50, 200), {
      tl: {x: 0, y: 100},
      tr: {x: 200, y: 100},
      bl: {x: 50, y: 100},
      apex: {x: 100, y: 100},
      isFez: false,
      theta: Math.PI,
    })
  })

  it('Returns correct values for a 90 degree rainbow', () => {
    assertPixelPointsEqual(computePixelPointsFromRadius(100, 0, 100 * ROOT_2), {
      tl: {x: 0, y: 100 - 100 / ROOT_2},
      tr: {x: 100 * ROOT_2, y: 100 - 100 / ROOT_2},
      bl: {x: 50 * ROOT_2, y: 100},
      apex: {x: 50 * ROOT_2, y: 100},
      isFez: false,
      theta: Math.PI / 2,
    })
  })

  it('Returns correct values for a 90 degree rainbow with nonzero inner radius', () => {
    assertPixelPointsEqual(computePixelPointsFromRadius(100, 50, 100 * ROOT_2), {
      tl: {x: 0, y: 100 - 100 / ROOT_2},
      tr: {x: 100 * ROOT_2, y: 100 - 100 / ROOT_2},
      bl: {x: 25 * ROOT_2, y: 100 - 25 * ROOT_2},
      apex: {x: 50 * ROOT_2, y: 100},
      isFez: false,
      theta: Math.PI / 2,
    })
  })

  it('Returns correct values for a 90 degree fez with nonzero inner radius', () => {
    assertPixelPointsEqual(computePixelPointsFromRadius(-100, 50, 100 * ROOT_2), {
      tl: {x: 0, y: 100 - 50 * ROOT_2},
      tr: {x: 100 * ROOT_2, y: 100 - 50 * ROOT_2},
      bl: {x: 25 * ROOT_2, y: 100 - 25 * ROOT_2},
      apex: {x: 50 * ROOT_2, y: 100},
      isFez: true,
      theta: Math.PI / 2,
    })
  })

  it('Returns correct values for a 120 degree fez', () => {
    assertPixelPointsEqual(computePixelPointsFromRadius(-100, 50, 100 * ROOT_3), {
      tl: {x: 0, y: 50},
      tr: {x: 100 * ROOT_3, y: 50},
      bl: {x: 25 * ROOT_3, y: 75},
      apex: {x: 50 * ROOT_3, y: 100},
      isFez: true,
      theta: 2 * Math.PI / 3,
    })
  })
})
