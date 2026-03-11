import chai from 'chai'

import {
  getCircumferenceRatio, getConinessForRadii,
} from '../src/client/apps/image-targets/curved-geometry'

chai.should()
const {assert} = chai

const assertCloseEnough = (actual, expected, message = undefined) => {
  assert.closeTo(actual, expected, Math.abs(expected / 100), message)
}

describe('getCircumferenceRatio', () => {
  it('Returns correct value for a normal cone', () => {
    assertCloseEnough(getCircumferenceRatio(0, 0, 100, 50), 2)
  })
  it('Returns correct value for a fez', () => {
    assertCloseEnough(getCircumferenceRatio(0, 0, -100, 50), 0.5)
  })

  it('Returns correct value for a cylinder', () => {
    assertCloseEnough(getCircumferenceRatio(0, 0, 100, 100), 1)
  })

  it('Defaults to given values if topRadius or bottomRadius are falsy', () => {
    assertCloseEnough(getCircumferenceRatio(1, 1, 0, 100), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, 100, 0), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, undefined, 100), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, 100, undefined), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, 0, 0), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, undefined, undefined), 1)
    assertCloseEnough(getCircumferenceRatio(1, 1, NaN, 100), 1)
  })
})

describe('getConinessForRadii', () => {
  it('Returns correct value for a normal cone', () => {
    assertCloseEnough(getConinessForRadii(100, 50), 1)
  })
  it('Returns correct value for a fez', () => {
    assertCloseEnough(getConinessForRadii(-100, 50), -1)
  })

  it('Returns correct value for a cylinder', () => {
    assertCloseEnough(getConinessForRadii(100, 100), 0)
  })

  it('Limits coniness to maximum of 2', () => {
    assertCloseEnough(getConinessForRadii(100, 10), 2)
    assertCloseEnough(getConinessForRadii(100, 0), 2)
  })

  it('Limits coniness to minimum of -2', () => {
    assertCloseEnough(getConinessForRadii(-100, 10), -2)
    assertCloseEnough(getConinessForRadii(-100, 0), -2)
  })

  it('Defaults to zero if topRadius is 0', () => {
    assertCloseEnough(getConinessForRadii(0, 100), 0)
  })

  it('Defaults to zero if missing either field', () => {
    assertCloseEnough(getConinessForRadii(undefined, 100), 0)
    assertCloseEnough(getConinessForRadii(100, undefined), 0)
    assertCloseEnough(getConinessForRadii(undefined, undefined), 0)
    assertCloseEnough(getConinessForRadii(null, 100), 0)
    assertCloseEnough(getConinessForRadii(100, null), 0)
    assertCloseEnough(getConinessForRadii(null, null), 0)
    assertCloseEnough(getConinessForRadii(NaN, 100), 0, 'nanfirst')
    assertCloseEnough(getConinessForRadii(100, NaN), 0, 'nansecond')
  })
})
