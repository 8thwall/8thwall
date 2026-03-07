/* eslint-disable @typescript-eslint/no-unused-expressions */
/* eslint-disable import/no-unresolved */

// @ts-ignore
import {chai, describe, it, assert} from 'bzl/js/chai-js'

import {mat4, quat, vec2, vec3} from './math'
import type {Mat4, Quat, Vec2, Vec3} from './math'

const {expect} = chai

describe('Vector2 Methods', () => {
  it('tests clone and equals', () => {
    const a: Vec2 = vec2.xy(1, 2)
    const b = vec2.zero()
    const c = a.clone()
    const d = vec2.xy(2, 1)
    const e = vec2.from({x: 1, y: 2})
    const f = vec2.xy(0, 0)

    expect(a.equals(a, 0)).to.be.true
    expect(a.equals(b, 0)).to.be.false
    expect(a.equals(c, 0)).to.be.true
    expect(a.equals(d, 0)).to.be.false
    expect(a.equals(e, 0)).to.be.true
    expect(a.equals(f, 0)).to.be.false
    expect(b.equals(c, 0)).to.be.false
    expect(b.equals(d, 0)).to.be.false
    expect(b.equals(e, 0)).to.be.false
    expect(b.equals(f, 0)).to.be.true

    // With a high tolerance, these are equal.
    expect(a.equals(b, 2)).to.be.true
  })

  it('tests algebra operations', () => {
    const a = vec2.xy(1, 2)
    expect(a.plus(a).equals(vec2.xy(2, 4), 0)).to.be.true
    expect(a.minus(a).equals(vec2.zero(), 0)).to.be.true
    expect(a.scale(2).equals(vec2.xy(2, 4), 0)).to.be.true
    expect(a.times(vec2.xy(2, 3)).equals(vec2.xy(2, 6), 0)).to.be.true
    expect(a.divide(a).equals(vec2.one(), 0)).to.be.true
  })

  it('tests vector operations', () => {
    const a = vec2.xy(1, 2)
    const b = vec2.xy(2, 1)
    const zeroVec = vec2.xy(0, 0)
    const norm = Math.sqrt(5)
    const inorm = 1 / norm
    expect(a.dot(b)).to.equal(4)
    expect(a.cross(b)).to.equal(-3)
    expect(a.length()).to.equal(norm)
    expect(a.normalize().equals(a.scale(inorm), 0)).to.be.true
    expect(zeroVec.normalize().equals(zeroVec, 0)).to.be.true
  })

  it('tests mix', () => {
    const a = vec2.xy(1, 2)
    const b = vec2.xy(5, -2)

    expect(a.mix(b, 0).equals(a, 0)).to.be.true
    expect(a.mix(b, 0.25).equals(vec2.xy(2, 1), 1e-5)).to.be.true
    expect(a.mix(b, 0.5).equals(vec2.xy(3, 0), 1e-5)).to.be.true
    expect(a.mix(b, 0.75).equals(vec2.xy(4, -1), 1e-5)).to.be.true
    expect(a.mix(b, 1).equals(b, 0)).to.be.true
  })

  it('exposes only x, y as enumerable properties', () => {
    const a = vec2.xy(1, 2)
    assert.deepStrictEqual(Object.keys(a), ['x', 'y'])
    const spread = {...a}
    assert.deepStrictEqual(spread, {x: 1, y: 2})
  })

  it('throws errors if x, y are written to', () => {
    const a = vec2.zero()
    expect(() => {
      // @ts-expect-error
      a.x = 1
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.y = 2
    }).to.throw()
  })

  it('tests make scale', () => {
    const a = vec2.scale(2)
    const b = vec2.scale(-54321)
    expect(a.equals(vec2.xy(2, 2), 0)).to.be.true
    expect(b.equals(vec2.xy(-54321, -54321), 0)).to.be.true
  })

  it('tests distanceTo', () => {
    const a = vec2.xy(1, 2)
    const b = a.plus(vec2.xy(1, 0))
    const c = a.minus(vec2.xy(2, 0))
    const d = a.plus(vec2.xy(3, -4))
    const e = vec2.zero()
    const f = vec2.one()
    const g = vec2.scale(2)

    expect(a.distanceTo(b)).to.equal(1 + 0 + 0)
    expect(a.distanceTo(c)).to.equal(Math.sqrt(4 + 0))
    expect(a.distanceTo(d)).to.equal(Math.sqrt(9 + 16))
    expect(a.distanceTo(e)).to.equal(Math.sqrt(1 + 4))
    expect(a.distanceTo(f)).to.equal(Math.sqrt(0 + 1))
    expect(a.distanceTo(g)).to.equal(Math.sqrt(1 + 0))
  })

  it('tests set components', () => {
    const a = vec2.one()
    a.setX(0)
    expect(a.equals(vec2.xy(0, 1), 0)).to.be.true
    a.setY(2)
    expect(a.equals(vec2.xy(0, 2), 0)).to.be.true

    const b = vec2.xy(1, 2)
    expect(vec2.from(b).setX(0).equals(a, 0)).to.be.true
    expect(b.equals(a, 0)).to.be.false
  })
})

describe('Vector3 Methods', () => {
  it('tests clone and equals', () => {
    const a: Vec3 = vec3.xyz(1, 2, 3)
    const b = vec3.zero()
    const c = a.clone()
    const d = vec3.xyz(3, 2, 1)
    const e = vec3.from({x: 1, y: 2, z: 3})
    const f = vec3.xyz(0, 0, 0)

    expect(a.equals(a, 0)).to.be.true
    expect(a.equals(b, 0)).to.be.false
    expect(a.equals(c, 0)).to.be.true
    expect(a.equals(d, 0)).to.be.false
    expect(a.equals(e, 0)).to.be.true
    expect(a.equals(f, 0)).to.be.false
    expect(b.equals(c, 0)).to.be.false
    expect(b.equals(d, 0)).to.be.false
    expect(b.equals(e, 0)).to.be.false
    expect(b.equals(f, 0)).to.be.true

    // With a high tolerance, these are equal.
    expect(a.equals(b, 3)).to.be.true
  })

  it('tests algebra operations', () => {
    const a = vec3.xyz(1, 2, 3)
    expect(a.plus(a).equals(vec3.xyz(2, 4, 6), 0)).to.be.true
    expect(a.minus(a).equals(vec3.zero(), 0)).to.be.true
    expect(a.scale(2).equals(vec3.xyz(2, 4, 6), 0)).to.be.true
    expect(a.times(vec3.xyz(2, 3, 4)).equals(vec3.xyz(2, 6, 12), 0)).to.be.true
    expect(a.divide(a).equals(vec3.one(), 0)).to.be.true
  })

  it('tests vector operations', () => {
    const a = vec3.xyz(1, 2, 3)
    const b = vec3.xyz(3, 2, 1)
    const zeroVec = vec3.xyz(0, 0, 0)
    const norm = Math.sqrt(14)
    const inorm = 1 / norm
    expect(a.dot(b)).to.equal(10)
    expect(a.cross(b).equals(vec3.xyz(-4, 8, -4), 0)).to.be.true
    expect(a.length()).to.equal(norm)
    expect(a.normalize().equals(a.scale(inorm), 0)).to.be.true
    expect(zeroVec.normalize().equals(zeroVec, 0)).to.be.true
  })

  it('tests mix', () => {
    const a = vec3.xyz(1, 2, 3)
    const b = vec3.xyz(5, -2, 11)

    expect(a.mix(b, 0).equals(a, 0)).to.be.true
    expect(a.mix(b, 0.25).equals(vec3.xyz(2, 1, 5), 1e-5)).to.be.true
    expect(a.mix(b, 0.5).equals(vec3.xyz(3, 0, 7), 1e-5)).to.be.true
    expect(a.mix(b, 0.75).equals(vec3.xyz(4, -1, 9), 1e-5)).to.be.true
    expect(a.mix(b, 1).equals(b, 0)).to.be.true
  })

  it('exposes only x, y, z as enumerable properties', () => {
    const a = vec3.xyz(1, 2, 3)
    assert.deepStrictEqual(Object.keys(a), ['x', 'y', 'z'])
    const spread = {...a}
    assert.deepStrictEqual(spread, {x: 1, y: 2, z: 3})
  })

  it('throws errors if x, y, z are written to', () => {
    const a = vec3.zero()
    expect(() => {
      // @ts-expect-error
      a.x = 1
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.y = 2
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.z = 3
    }).to.throw()
  })

  it('tests make scale', () => {
    const a = vec3.scale(2)
    const b = vec3.scale(-54321)
    expect(a.equals(vec3.xyz(2, 2, 2), 0)).to.be.true
    expect(b.equals(vec3.xyz(-54321, -54321, -54321), 0)).to.be.true
  })

  it('tests distanceTo', () => {
    const a = vec3.xyz(1, 2, 3)
    const b = a.plus(vec3.xyz(1, 0, 0))
    const c = a.minus(vec3.xyz(2, 0, 2))
    const d = a.plus(vec3.xyz(3, -4, 5))
    const e = vec3.zero()
    const f = vec3.up()
    const g = vec3.one()
    const h = vec3.scale(2)

    expect(a.distanceTo(b)).to.equal(1 + 0 + 0)
    expect(a.distanceTo(c)).to.equal(Math.sqrt(4 + 0 + 4))
    expect(a.distanceTo(d)).to.equal(Math.sqrt(9 + 16 + 25))
    expect(a.distanceTo(e)).to.equal(Math.sqrt(1 + 4 + 9))
    expect(a.distanceTo(f)).to.equal(Math.sqrt(1 + 1 + 9))
    expect(a.distanceTo(g)).to.equal(Math.sqrt(0 + 1 + 4))
    expect(a.distanceTo(h)).to.equal(Math.sqrt(1 + 0 + 1))
  })

  it('tests set components', () => {
    const a = vec3.one()
    a.setX(0)
    expect(a.equals(vec3.xyz(0, 1, 1), 0)).to.be.true
    a.setY(2)
    expect(a.equals(vec3.xyz(0, 2, 1), 0)).to.be.true
    a.setZ(-3)
    expect(a.equals(vec3.xyz(0, 2, -3), 0)).to.be.true

    const b = vec3.xyz(1, 2, -3)
    expect(vec3.from(b).setX(0).equals(a, 0)).to.be.true
    expect(b.equals(a, 0)).to.be.false
  })
})

describe('Quaternion Methods', () => {
  it('tests clone and equals', () => {
    const a: Quat = quat.xyzw(0, 1, 0, 0)
    const b = quat.zero()
    const c = a.clone()
    const d = quat.xyzw(1, 0, 0, 0)
    const e = quat.from({x: 0, y: 1, z: 0, w: 0})
    const f = quat.xyzw(0, 0, 0, 1)

    expect(a.equals(a, 0)).to.be.true
    expect(a.equals(b, 0)).to.be.false
    expect(a.equals(c, 0)).to.be.true
    expect(a.equals(d, 0)).to.be.false
    expect(a.equals(e, 0)).to.be.true
    expect(a.equals(f, 0)).to.be.false
    expect(b.equals(c, 0)).to.be.false
    expect(b.equals(d, 0)).to.be.false
    expect(b.equals(e, 0)).to.be.false
    expect(b.equals(f, 0)).to.be.true

    // With a high tolerance, these are equal.
    expect(a.equals(b, 1)).to.be.true
  })

  it('tests pitchYawRoll', () => {
    // Pitch
    expect(
      quat.xDegrees(90).equals(quat.xyzw(0.7071067811865476, 0, 0, 0.7071067811865476), 1e-5)
    ).to.be.true
    expect(
      quat.xRadians(
        0.5 * Math.PI
      ).equals(quat.xyzw(0.7071067811865476, 0, 0, 0.7071067811865476),
        1e-5)
    ).to.be.true
    expect(
      quat.xDegrees(90).pitchYawRollDegrees().equals(vec3.xyz(90, 0, 0), 1e-5)
    ).to.be.true
    expect(
      quat.xRadians(Math.PI * 0.5)
        .pitchYawRollRadians()
        .equals(vec3.xyz(0.5 * Math.PI, 0, 0), 1e-5)
    ).to.be.true

    // Yaw
    expect(
      quat.yDegrees(90).equals(quat.xyzw(0, 0.7071067811865476, 0, 0.7071067811865476), 1e-5)
    ).to.be.true
    expect(
      quat.yRadians(0.5 * Math.PI)
        .equals(quat.xyzw(0, 0.7071067811865476, 0, 0.7071067811865476), 1e-5)
    ).to.be.true
    expect(
      quat.yDegrees(90).pitchYawRollDegrees().equals(vec3.xyz(0, 90, 0), 1e-5)
    ).to.be.true
    expect(
      quat.yRadians(0.5 * Math.PI)
        .pitchYawRollRadians()
        .equals(vec3.xyz(0, 0.5 * Math.PI, 0), 1e-5)
    ).to.be.true

    // Roll
    expect(
      quat.zDegrees(90).equals(quat.xyzw(0, 0, 0.7071067811865476, 0.7071067811865476), 1e-5)
    ).to.be.true
    expect(
      quat.zRadians(0.5 * Math.PI)
        .equals(quat.xyzw(0, 0, 0.7071067811865476, 0.7071067811865476), 1e-5)
    ).to.be.true
    expect(
      quat.zDegrees(90).pitchYawRollDegrees().equals(vec3.xyz(0, 0, 90), 1e-5)
    ).to.be.true
    expect(
      quat.zRadians(0.5 * Math.PI)
        .pitchYawRollRadians()
        .equals(vec3.xyz(0, 0, 0.5 * Math.PI), 1e-5)
    ).to.be.true

    // Combined
    expect(
      quat.pitchYawRollDegrees(vec3.xyz(30, -20, 10))
        .pitchYawRollDegrees()
        .equals(vec3.xyz(30, -20, 10), 1e-5)
    ).to.be.true
    expect(
      quat.pitchYawRollRadians(vec3.xyz(0.3, -0.2, 0.1))
        .pitchYawRollRadians()
        .equals(vec3.xyz(0.3, -0.2, 0.1), 1e-5)
    ).to.be.true

    // Extreme angles
    expect(
      quat.pitchYawRollDegrees(vec3.xyz(-89.9, -179.9, 179.9))
        .pitchYawRollDegrees()
        .equals(vec3.xyz(-89.9, -179.9, 179.9), 1e-4)
    ).to.be.true

    // Identity
    expect(quat.pitchYawRollDegrees(vec3.zero()).equals(quat.zero(), 0)).to.be.true
    expect(quat.zero().pitchYawRollDegrees().equals(vec3.zero(), 0)).to.be.true

    // Ambiguous Roll
    expect(
      quat.pitchYawRollDegrees(vec3.xyz(90, 15, 30))
        .pitchYawRollDegrees()
        .equals(vec3.xyz(90, -15, 0), 1e-5)
    ).to.be.true

    // quaternion sampled from the unit sphere.
    expect(
      quat.pitchYawRollDegrees(
        quat.xyzw(0.43316012270641296, 0.1292451970381261, -0.728792304188662, 0.5143245711561157)
          .pitchYawRollDegrees()
      ).equals(
        quat.xyzw(0.43316012270641296, 0.1292451970381261, -0.728792304188662, 0.5143245711561157),
        1e-5
      )
    ).to.be.true
  })

  it('tests axisAngle', () => {
    const kPi = Math.PI
    const kHalfSqrt2 = 0.5 * Math.SQRT2

    // 90 degrees rotation.
    expect(
      quat.axisAngle(vec3.xyz(kPi / 2, 0, 0))
        .equals(quat.xyzw(kHalfSqrt2, 0, 0, kHalfSqrt2), 1e-5)
    ).to.be.true

    // Tiny rotation.
    const theta = 1e-20
    expect(
      quat.axisAngle(vec3.xyz(theta, 0, 0))
        .equals(quat.xyzw(Math.sin(theta / 2), 0, 0, Math.cos(theta / 2)), 1e-5)
    ).to.be.true

    // Identity.
    expect(quat.axisAngle(vec3.xyz(0, 0, 0)).equals(quat.zero(), 1e-5)).to.be.true

    // Conversion of arbitrary quaternions to axis angle and back again.
    expect(
      quat.axisAngle(quat.xyzw(0.5, 0.3, 0.5, 0.2).normalize().axisAngle())
        .equals(quat.xyzw(0.5, 0.3, 0.5, 0.2).normalize(), 1e-5)
    ).to.be.true
    expect(
      quat.axisAngle(quat.xyzw(0.3, 0.9, 0.6, 0.4).normalize().axisAngle())
        .equals(quat.xyzw(0.3, 0.9, 0.6, 0.4).normalize(), 1e-5)
    ).to.be.true
    expect(
      quat.axisAngle(quat.xyzw(0.8, 0.0, 0.2, 0.8).normalize().axisAngle())
        .equals(quat.xyzw(0.8, 0.0, 0.2, 0.8).normalize(), 1e-5)
    ).to.be.true
    expect(
      quat.axisAngle(quat.xyzw(0.7, 0.1, 0.9, 0.1).normalize().axisAngle())
        .equals(quat.xyzw(0.7, 0.1, 0.9, 0.1).normalize(), 1e-5)
    ).to.be.true
    expect(
      quat.axisAngle(quat.xyzw(0.4, 0.2, 0.0, 0.9).normalize().axisAngle())
        .equals(quat.xyzw(0.4, 0.2, 0.0, 0.9).normalize(), 1e-5)
    ).to.be.true
  })

  it('tests delta', () => {
    const q1 = quat.xyzw(
      0.43316012270641296, 0.1292451970381261, -0.728792304188662, 0.5143245711561157
    )
    const q2 = quat.xyzw(
      0.60921278119571, -0.12202887220438137, -0.1194194353954246, 0.7744079932607568
    )
    const delta = q1.delta(q2)
    const q3 = delta.times(q1)
    expect(q3.equals(q2, 1e-5)).to.be.true
  })

  it('tests slerp', () => {
    const q1 = quat.xyzw(
      0.43316012270641296, 0.1292451970381261, -0.728792304188662, 0.5143245711561157
    )
    const q2 = quat.xyzw(
      0.60921278119571, -0.12202887220438137, -0.1194194353954246, 0.7744079932607568
    )
    const i0 = q1.slerp(q2, 0)     // 0%
    const i1 = q1.slerp(q2, 0.2)   // 20%
    const i2 = i1.slerp(q2, 0.25)  // 25% of 80% = 20% + 20% = 40%
    const i3 = q1.slerp(q2, 0.4)   // 40%
    const i4 = q1.slerp(q2, 1)     // 100%

    expect(i0.equals(q1, 1e-5)).to.be.true
    expect(i2.equals(i3, 1e-5)).to.be.true
    expect(i4.equals(q2, 1e-5)).to.be.true

    expect(quat.zero().slerp(quat.xDegrees(90), 0.5).equals(quat.xDegrees(45), 1e-5)).to.be.true
    expect(quat.zero().slerp(quat.xDegrees(-90), 0.5).equals(quat.xDegrees(-45), 1e-5)).to.be.true
    expect(quat.zero().slerp(quat.yDegrees(90), 0.5).equals(quat.yDegrees(45), 1e-5)).to.be.true
    expect(quat.zero().slerp(quat.yDegrees(-90), 0.5).equals(quat.yDegrees(-45), 1e-5)).to.be.true
    expect(quat.zero().slerp(quat.zDegrees(90), 0.5).equals(quat.zDegrees(45), 1e-5)).to.be.true
    expect(quat.zero().slerp(quat.zDegrees(-90), 0.5).equals(quat.zDegrees(-45), 1e-5)).to.be.true
    expect(
      quat.xDegrees(5).slerp(quat.xDegrees(6), 0.5).equals(quat.xDegrees(5.5), 1e-5)
    ).to.be.true
    expect(
      quat.yDegrees(5).slerp(quat.yDegrees(6), 0.5).equals(quat.yDegrees(5.5), 1e-5)
    ).to.be.true
    expect(
      quat.zDegrees(5).slerp(quat.zDegrees(6), 0.5).equals(quat.zDegrees(5.5), 1e-5)
    ).to.be.true

    expect(
      quat.zDegrees(150).slerp(quat.zDegrees(-150), 0.5).equals(quat.zDegrees(180), 1e-5)
    ).to.be.true
  })

  it('tests rotateToward', () => {
    const q1 = quat.xDegrees(0)
    const q2 = quat.xDegrees(90)
    const q3 = quat.xDegrees(-90)

    expect(q1.rotateToward(q2, 20 * (Math.PI / 180)).equals(quat.xDegrees(20), 1e-5)).to.be.true
    expect(q1.rotateToward(q3, 20 * (Math.PI / 180)).equals(quat.xDegrees(-20), 1e-5)).to.be.true
  })

  it('tests degrees and radians', () => {
    const q1 = quat.zero()
    const qSmallDiff = quat.zDegrees(0.1)
    const q2 = quat.zDegrees(90)
    const q3 = quat.zDegrees(180)
    const q4 = quat.zDegrees(270)
    const q2Inv = q2.inv()
    const q2Negated = q2.negate()
    expect(q1.degreesTo(qSmallDiff)).to.be.closeTo(0.1, 1e-5)
    expect(q1.degreesTo(q2)).to.be.closeTo(90, 1e-5)
    expect(q1.degreesTo(q3)).to.be.closeTo(180, 1e-5)
    expect(q1.degreesTo(q4)).to.be.closeTo(90, 1e-5)
    expect(q2.degreesTo(q2Inv)).to.be.closeTo(180, 1e-5)
    expect(q2.degreesTo(q2Negated)).to.be.closeTo(0, 1e-5)

    expect(q1.radiansTo(qSmallDiff)).to.be.closeTo(0.1 * (Math.PI / 180), 1e-5)
    expect(q1.radiansTo(q2)).to.be.closeTo(Math.PI / 2, 1e-5)
    expect(q1.radiansTo(q3)).to.be.closeTo(Math.PI, 1e-5)
    expect(q1.radiansTo(q4)).to.be.closeTo(Math.PI / 2, 1e-5)
    expect(q2.radiansTo(q2Inv)).to.be.closeTo(Math.PI, 1e-5)
    expect(q2.radiansTo(q2Negated)).to.be.closeTo(0, 1e-5)

    const q5 = quat.xyzw(0.238495, 0.0100453, 0.0543614, 0.969569).normalize()
    expect(q5.degreesTo(q5)).to.be.closeTo(0, 1e-5)
    expect(q5.radiansTo(q5)).to.be.closeTo(0, 1e-5)

    for (let i = 0; i < 180; ++i) {
      expect(q1.degreesTo(quat.xDegrees(i))).to.be.closeTo(i, 1e-5)
      expect(q1.degreesTo(quat.yDegrees(i))).to.be.closeTo(i, 1e-5)
      expect(q1.degreesTo(quat.zDegrees(i))).to.be.closeTo(i, 1e-5)
      expect(q1.radiansTo(quat.xDegrees(i))).to.be.closeTo(i * (Math.PI / 180), 1e-5)
      expect(q1.radiansTo(quat.yDegrees(i))).to.be.closeTo(i * (Math.PI / 180), 1e-5)
      expect(q1.radiansTo(quat.zDegrees(i))).to.be.closeTo(i * (Math.PI / 180), 1e-5)

      const q = quat.yDegrees(i).times(q5)
      expect(q.degreesTo(q5)).to.be.closeTo(i, 1e-5)
      expect(q.radiansTo(q5)).to.be.closeTo(i * (Math.PI / 180), 1e-5)
    }
  })

  it('tests vector operations', () => {
    const a = quat.zero()
    const b = quat.xyzw(0, 1, 0, 0)

    expect(a.times(b).equals(b, 0)).to.be.true
    expect(a.negate().equals(quat.xyzw(0, 0, 0, -1), 0)).to.be.true
    expect(b.negate().equals(quat.xyzw(0, -1, 0, 0), 0)).to.be.true
    expect(a.dot(b)).to.equal(0)
    expect(a.dot(a)).to.equal(1)
    expect(b.dot(b)).to.equal(1)
    expect(a.conjugate().equals(a, 0)).to.be.true
    expect(b.conjugate().equals(b.negate(), 0)).to.be.true
    expect(quat.xyzw(0, 2, 0, 0).normalize().equals(b, 0)).to.be.true
    expect(a.plus(b).normalize().equals(quat.yDegrees(90), 1e-5)).to.be.true
  })

  it('tests lookAt', () => {
    const q1 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(0, 0, 0), vec3.up())
    const q2 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(0, 0, 0), vec3.xyz(0, -1, 0))
    const q3 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(0, 0, 0), vec3.xyz(1, 0, 0))
    const q4 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(0, 0, 0), vec3.xyz(-1, 0, 0))
    const q5 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(1, 0, -1), vec3.up())
    const q6 = quat.lookAt(vec3.xyz(0, 0, -1), vec3.xyz(-1, 0, -1), vec3.up())
    const q7 = quat.lookAt(vec3.xyz(0, 1, -2), vec3.xyz(0, 0, -1), vec3.up())
    const q8 = quat.lookAt(vec3.xyz(0, 0, -2), vec3.xyz(0, 1, -1), vec3.up())

    expect(q1.equals(quat.zero(), 1e-5)).to.be.true
    expect(q2.equals(quat.zDegrees(180), 1e-5)).to.be.true
    expect(q3.equals(quat.zDegrees(-90), 1e-5)).to.be.true
    expect(q4.equals(quat.zDegrees(90), 1e-5)).to.be.true
    expect(q5.equals(quat.yDegrees(90), 1e-5)).to.be.true
    expect(q6.equals(quat.yDegrees(-90), 1e-5)).to.be.true
    expect(q7.equals(quat.xDegrees(45), 1e-5)).to.be.true
    expect(q8.equals(quat.xDegrees(-45), 1e-5)).to.be.true
  })

  it('tests timesVec', () => {
    const q = quat.xyzw(1, 2, 3, 4).normalize()
    const m = mat4.r(q)

    const expected = m.timesVec(vec3.xyz(5, -6, 7))

    expect(q.timesVec(vec3.xyz(5, -6, 7)).equals(expected, 1e-5)).to.be.true
  })

  it('exposes only x, y, z, and w as enumerable properties', () => {
    const a = quat.xyzw(1, 2, 3, 4)
    assert.deepStrictEqual(Object.keys(a), ['x', 'y', 'z', 'w'])
    const spread = {...a}
    assert.deepStrictEqual(spread, {x: 1, y: 2, z: 3, w: 4})
  })

  it('throws errors if x, y, z, or w are written to', () => {
    const a = quat.zero()
    expect(() => {
      // @ts-expect-error
      a.x = 1
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.y = 2
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.z = 3
    }).to.throw()
    expect(() => {
      // @ts-expect-error
      a.w = 4
    }).to.throw()
  })
})

describe('Matrix4 Methods', () => {
  it('tests clone and equals', () => {
    const a: Mat4 = mat4.t(vec3.xyz(1, 2, 3))
    const b = mat4.i()
    const c = a.clone()
    const d = mat4.t(vec3.xyz(3, 2, 1))
    const e = mat4.rows([
      [1, 0, 0, 1],
      [0, 1, 0, 2],
      [0, 0, 1, 3],
      [0, 0, 0, 1],
    ])
    const f = mat4.of([1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1])

    expect(a.equals(a, 0)).to.be.true
    expect(a.equals(b, 0)).to.be.false
    expect(a.equals(c, 0)).to.be.true
    expect(a.inv().equals(c.inv(), 0)).to.be.true  // Cloned Inverse
    expect(a.equals(d, 0)).to.be.false
    expect(a.equals(e, 0)).to.be.true
    expect(a.equals(f, 0)).to.be.false
    expect(a.inv().equals(e.inv(), 0)).to.be.true  // Computed Inverse
    expect(b.equals(c, 0)).to.be.false
    expect(b.equals(d, 0)).to.be.false
    expect(b.equals(e, 0)).to.be.false
    expect(b.equals(f, 0)).to.be.true
    expect(b.inv().equals(b.inv(), 0)).to.be.true  // Computed Inverse

    // With a high tolerance, these are equal.
    expect(a.equals(b, 3)).to.be.true
  })

  it('tests inverse', () => {
    // Identity
    const a = mat4.i()
    expect(a.equals(a.inv(), 1e-5)).to.be.true

    // Translation
    const b = mat4.t(vec3.xyz(1, 2, 3))
    expect(a.equals(b, 1e-5)).to.be.false
    expect(b.equals(b.inv(), 1e-5)).to.be.false
    expect(a.equals(b.inv(), 1e-5)).to.be.false
    expect(a.equals(b.times(b.inv()), 1e-5)).to.be.true
    expect(a.equals(b.inv().times(b), 1e-5)).to.be.true

    // Rotation
    const c = mat4.r(quat.xyzw(1, 2, 3, 4).normalize())
    expect(a.equals(c, 1e-5)).to.be.false
    expect(c.equals(c.inv(), 1e-5)).to.be.false
    expect(a.equals(c.inv(), 1e-5)).to.be.false
    expect(a.equals(c.times(c.inv()), 1e-5)).to.be.true
    expect(a.equals(c.inv().times(c), 1e-5)).to.be.true

    // Scale
    const d = mat4.s(vec3.xyz(4, 5, 6))
    expect(a.equals(d, 1e-5)).to.be.false
    expect(d.equals(d.inv(), 1e-5)).to.be.false
    expect(a.equals(d.inv(), 1e-5)).to.be.false
    expect(a.equals(d.times(d.inv()), 1e-5)).to.be.true
    expect(a.equals(d.inv().times(d), 1e-5)).to.be.true

    // Translation + Rotation
    const e = mat4.tr(vec3.xyz(1, 2, 3), quat.xyzw(1, 2, 3, 4).normalize())
    expect(a.equals(e, 1e-5)).to.be.false
    expect(e.equals(e.inv(), 1e-5)).to.be.false
    expect(a.equals(e.inv(), 1e-5)).to.be.false
    expect(a.equals(e.times(e.inv()), 1e-5)).to.be.true
    expect(a.equals(e.inv().times(e), 1e-5)).to.be.true
    expect(a.equals(e.times(c.inv().times(b.inv())), 1e-5)).to.be.true

    // Translation + Rotation + Scale
    const f = mat4.trs(vec3.xyz(1, 2, 3), quat.xyzw(1, 2, 3, 4).normalize(), vec3.xyz(4, 5, 6))
    expect(a.equals(f, 1e-5)).to.be.false
    expect(f.equals(f.inv(), 1e-5)).to.be.false
    expect(a.equals(f.inv(), 1e-5)).to.be.false
    expect(a.equals(f.times(f.inv()), 1e-5)).to.be.true
    expect(a.equals(f.inv().times(f), 1e-5)).to.be.true
    expect(a.equals(f.times(d.inv().times(c.inv()).times(b.inv())), 1e-5)).to.be.true

    // Non-invertible
    const g = mat4.of([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
    expect(() => g.inv()).to.throw()

    // Computed
    const h = mat4.rows([
      [2, 0, 0, 4],
      [0, 2, 0, -4],
      [0, 0, 1, 8],
      [0, 0, 0, 1],
    ])

    const hi = mat4.rows([
      [0.5, 0, 0, -2],
      [0, 0.5, 0, 2],
      [0, 0, 1, -8],
      [0, 0, 0, 1],
    ])
    expect(h.inv().equals(hi, 1e-5)).to.be.true
  })

  it('tests rotation', () => {
    const x90 = mat4.r(quat.xDegrees(90))
    expect(x90.equals(mat4.rows([
      [1, 0, 0, 0],
      [0, 0, -1, 0],
      [0, 1, 0, 0],
      [0, 0, 0, 1],
    ]), 1e-5)).to.be.true
    expect(x90.decomposeTrs().r.equals(quat.xDegrees(90), 1e-5)).to.be.true
    expect(x90.inv().times(x90).equals(mat4.i(), 1e-5)).to.be.true
    expect(x90.times(x90.inv()).equals(mat4.i(), 1e-5)).to.be.true

    const y90 = mat4.r(quat.yDegrees(90))
    expect(y90.equals(mat4.rows([
      [0, 0, 1, 0],
      [0, 1, 0, 0],
      [-1, 0, 0, 0],
      [0, 0, 0, 1],
    ]), 1e-5)).to.be.true
    expect(y90.decomposeTrs().r.equals(quat.yDegrees(90), 1e-5)).to.be.true
    expect(y90.inv().times(y90).equals(mat4.i(), 1e-5)).to.be.true
    expect(y90.times(y90.inv()).equals(mat4.i(), 1e-5)).to.be.true

    const z90 = mat4.r(quat.zDegrees(90))
    expect(z90.equals(mat4.rows([
      [0, -1, 0, 0],
      [1, 0, 0, 0],
      [0, 0, 1, 0],
      [0, 0, 0, 1],
    ]), 1e-5)).to.be.true
    expect(z90.decomposeTrs().r.equals(quat.zDegrees(90), 1e-5)).to.be.true
    expect(z90.inv().times(z90).equals(mat4.i(), 1e-5)).to.be.true
    expect(z90.times(z90.inv()).equals(mat4.i(), 1e-5)).to.be.true

    const r = quat.pitchYawRollDegrees(vec3.xyz(30, 45, 60))
    const m = mat4.r(r)
    expect(m.transpose().equals(m.inv(), 1e-5)).to.be.true
    expect(m.inv().times(m).equals(mat4.i(), 1e-5)).to.be.true
    expect(m.times(m.inv()).equals(mat4.i(), 1e-5)).to.be.true
    expect(m.decomposeTrs().r.equals(r, 1e-5)).to.be.true

    const ms = mat4.trs(vec3.xyz(1, 5, 10), r, vec3.xyz(2, 3, 4))
    expect(ms.decomposeTrs().r.equals(r, 1e-5)).to.be.true
  })

  it('tests transpose', () => {
    // Non-invertible.
    const a = mat4.rows([
      [1, 2, 3, 4],
      [5, 6, 7, 8],
      [9, 10, 11, 12],
      [13, 14, 15, 16],
    ])
    const b = mat4.rows([
      [1, 5, 9, 13],
      [2, 6, 10, 14],
      [3, 7, 11, 15],
      [4, 8, 12, 16],
    ])
    expect(a.transpose().equals(b, 1e-5)).to.be.true
    expect(() => a.inv()).to.throw()
    expect(() => a.transpose().inv()).to.throw()

    // Invertible.
    const c = mat4.rows([
      [1.0, -1.0, 1.0, 1.0],
      [1.0, 1.0, -1.0, 1.0],
      [-1.0, 1.0, 1.0, 1.0],
      [1.0, 1.0, 1.0, -1.0],
    ])
    const d = mat4.rows([
      [1.0, 1.0, -1.0, 1.0],
      [-1.0, 1.0, 1.0, 1.0],
      [1.0, -1.0, 1.0, 1.0],
      [1.0, 1.0, 1.0, -1.0],
    ])
    expect(c.transpose().equals(d, 1e-5)).to.be.true
    expect(c.transpose().inv().equals(d.inv(), 1e-5)).to.be.true
  })

  it('tests vector multiply', () => {
    const m1 = mat4.rows([
      [-1, 2, 3, 4],
      [5, -6, 7, 8],
      [9, -8, 7, 6],
      [5, 4, -3, 1],
    ])

    const p1 = [
      vec3.xyz(1, 2, 3),
      vec3.xyz(2, 5, 7),
    ]

    const ep1 = [
      vec3.xyz(16 / 5, 22 / 5, 20 / 5),
      vec3.xyz(33 / 10, 37 / 10, 33 / 10),
    ]

    for (let i = 0; i < p1.length; ++i) {
      expect(m1.timesVec(p1[i]).equals(ep1[i], 1e-5)).to.be.true
    }

    const m2 = mat4.rows([
      [0.74546921, 0.71952996, 0.76345238, 0.71337735],
      [0.78034825, 0.54154092, 0.55100812, 0.16757722],
      [0.49670883, 0.81379888, 0.13691469, 0.866180416],
      [0.5860506, 0.5669385, 0.05191393, 0.3886459],
    ])

    const p2 = [
      vec3.xyz(0.54342354, 0.65875207, 0.89720247),
      vec3.xyz(0.51389245, 0.97631843, 0.48467117),
      vec3.xyz(0.25009358, 0.71512308, 0.32409540),
      vec3.xyz(0.99413048, 0.50010172, 0.12262631),
      vec3.xyz(0.45707725, 0.08260504, 0.16636445),
      vec3.xyz(0.56608858, 0.70367859, 0.32572620),
      vec3.xyz(0.04110697, 0.74535412, 0.96589189),
      vec3.xyz(0.01032942, 0.84072808, 0.30515309),
      vec3.xyz(0.87960067, 0.85525782, 0.49250595),
      vec3.xyz(0.53983367, 0.94510150, 0.06532676),
      vec3.xyz(0.00458787, 0.62535925, 0.96895621),
      vec3.xyz(0.26766250, 0.34012995, 0.41140411),
      vec3.xyz(0.96016821, 0.45234735, 0.98342685),
      vec3.xyz(0.19080576, 0.46184871, 0.45606579),
      vec3.xyz(0.10908995, 0.39252267, 0.90102594),
      vec3.xyz(0.84180821, 0.80859068, 0.22400568),
      vec3.xyz(0.56711724, 0.14736189, 0.71439800),
    ]

    const ep2 = [
      vec3.xyz(2.02050135, 1.27997144, 1.59251709),
      vec3.xyz(1.70989756, 1.07558602, 1.56274480),
      vec3.xyz(1.73561530, 0.96983261, 1.68855957),
      vec3.xyz(1.51284871, 1.01632577, 1.41438048),
      vec3.xyz(1.74240001, 0.92791128, 1.66185595),
      vec3.xyz(1.66368799, 1.02958608, 1.55300676),
      vec3.xyz(2.27877210, 1.28240916, 1.83569035),
      vec3.xyz(1.75722234, 0.90068103, 1.80039062),
      vec3.xyz(1.66867636, 1.12294864, 1.46087231),
      vec3.xyz(1.48342509, 0.91353682, 1.53701511),
      vec3.xyz(2.39458522, 1.31091136, 1.89661333),
      vec3.xyz(1.93725495, 1.03636756, 1.75365977),
      vec3.xyz(1.99023781, 1.35335458, 1.46630300),
      vec3.xyz(1.95438614, 1.04057893, 1.78025241),
      vec3.xyz(2.44500192, 1.33225960, 1.88832688),
      vec3.xyz(1.54858212, 1.02496702, 1.45928842),
      vec3.xyz(2.12393776, 1.28745165, 1.62255944),
    ]

    for (let i = 0; i < p2.length; ++i) {
      expect(m2.timesVec(p2[i]).equals(ep2[i], 1e-5)).to.be.true
    }
  })

  it('tests point translation', () => {
    const pt = vec3.xyz(2, 5, 7)
    const m = mat4.t(vec3.xyz(1, -7, -2))
    expect(m.timesVec(pt).equals(vec3.xyz(3, -2, 5), 1e-5)).to.be.true
    expect(m.inv().timesVec(pt).equals(vec3.xyz(1, 12, 9), 1e-5)).to.be.true
  })

  it('tests scale', () => {
    const m = mat4.rows([
      [-1, 2, 3, 4],
      [5, -6, 7, 8],
      [9, -8, 7, 6],
      [5, 4, -3, 1],
    ])

    const m2 = mat4.rows([
      [-0.5, 1, 1.5, 2],
      [2.5, -3, 3.5, 4],
      [4.5, -4, 3.5, 3],
      [2.5, 2, -1.5, 0.5],
    ])

    expect(() => m.inv()).to.not.throw()
    expect(m.scale(0.5).equals(m2, 1e-5)).to.be.true
    expect(m2.scale(2).equals(m, 1e-5)).to.be.true

    expect(m.inv().scale(0.25).times(m.scale(4)).equals(mat4.i(), 1e-5)).to.be.true
    expect(m.scale(4).inv().times(m.scale(4)).equals(mat4.i(), 1e-5)).to.be.true
  })

  it('tests determinant', () => {
    expect(mat4.i().determinant()).to.be.closeTo(1, 1e-5)
    expect(
      mat4.r(quat.pitchYawRollDegrees(vec3.xyz(10, -20, 30))).determinant()
    ).to.be.closeTo(1, 1e-5)

    const m1 = mat4.rows([
      [-1.0, 2.00, 3.000, -4.00],
      [5.00, -6.0, -7.00, 8.000],
      [-9.0, 10.0, -11.0, 12.00],
      [13.0, -14.0, 15.0, -16.0],
    ])

    expect(m1.determinant()).to.be.closeTo(64, 1e-5)
    expect(m1.inv().determinant()).to.be.closeTo(1 / 64, 1e-5)

    const m2 = mat4.rows([
      [-0.92, -2.12, -1.58, 0.870],
      [0.070, 0.000, -0.67, -0.81],
      [-0.53, 1.340, -0.18, -0.39],
      [1.330, 0.840, 0.890, 1.060],
    ])

    expect(m2.determinant()).to.be.closeTo(-4.1393304, 1e-5)
    expect(m2.inv().determinant()).to.be.closeTo(1 / -4.1393304, 1e-5)
  })

  it('tests point scale', () => {
    const pt0 = vec3.xyz(2, 3, 5)
    const pex = vec3.xyz(8, 3, 5)
    const pey = vec3.xyz(2, 12, 5)
    const pez = vec3.xyz(2, 3, 20)

    const px = mat4.s(vec3.xyz(4, 1, 1)).timesVec(pt0)
    const px0 = mat4.s(vec3.xyz(4, 1, 1)).inv().timesVec(px)
    expect(px.equals(pex, 1e-5)).to.be.true
    expect(px0.equals(pt0, 1e-5)).to.be.true

    const py = mat4.s(vec3.xyz(1, 4, 1)).timesVec(pt0)
    const py0 = mat4.s(vec3.xyz(1, 4, 1)).inv().timesVec(py)
    expect(py.equals(pey, 1e-5)).to.be.true
    expect(py0.equals(pt0, 1e-5)).to.be.true

    const pz = mat4.s(vec3.xyz(1, 1, 4)).timesVec(pt0)
    const pz0 = mat4.s(vec3.xyz(1, 1, 4)).inv().timesVec(pz)
    expect(pz.equals(pez, 1e-5)).to.be.true
    expect(pz0.equals(pt0, 1e-5)).to.be.true
  })

  it('tests times', () => {
    const a = mat4.t(vec3.xyz(1, 2, 3))
    const b = mat4.r(quat.xyzw(1, 2, 3, 4).normalize())

    const expected = mat4.tr(vec3.xyz(1, 2, 3), quat.xyzw(1, 2, 3, 4).normalize())
    expect(a.times(b).equals(expected, 1e-5)).to.be.true
    expect(b.clone().setPremultiply(a).equals(expected, 1e-5)).to.be.true
  })

  it('tests lookAt', () => {
    const m = mat4.trs(vec3.xyz(1, 2, 3), quat.pitchYawRollDegrees(vec3.xyz(10, 7, 30)), vec3.one())
    const m2 = m.lookAt(vec3.xyz(-3, 1, 2), vec3.up())

    const projectedCenter = m2.inv().timesVec(vec3.xyz(-3, 1, 2))
    expect(projectedCenter.x).to.be.closeTo(0, 1e-5)
    expect(projectedCenter.y).to.be.closeTo(0, 1e-5)
    expect(projectedCenter.z).to.be.greaterThan(0, 1e-5)  // z+ is forward for lookAt.

    const projectedUp = m2.inv().timesVec(vec3.xyz(-3, 2, 2))
    expect(projectedUp.x).to.be.closeTo(0, 1e-5)
    expect(projectedUp.y).to.be.greaterThan(0)
    expect(projectedUp.z).to.be.greaterThan(0)  // z+ is forward for lookAt.
  })

  it('tests decomposeTrs', () => {
    const m = mat4.trs(
      vec3.xyz(1, -2, 3),
      quat.pitchYawRollDegrees(vec3.xyz(10, -70, 30)),
      vec3.xyz(4, 5, 6)
    )
    const {t, r, s} = m.decomposeTrs()
    expect(t.equals(vec3.xyz(1, -2, 3), 1e-5)).to.be.true
    expect(r.equals(quat.pitchYawRollDegrees(vec3.xyz(10, -70, 30)), 1e-5)).to.be.true
    expect(s.equals(vec3.xyz(4, 5, 6), 1e-5)).to.be.true
  })

  it('tests t,s same as decomposeTrs', () => {
    const m = mat4.trs(
      vec3.xyz(1, -2, 3),
      quat.pitchYawRollDegrees(vec3.xyz(10, -70, 30)),
      vec3.xyz(4, 5, 6)
    )

    const t = vec3.zero()
    const s = vec3.zero()

    m.decomposeS(s)
    m.decomposeT(t)

    expect(t.equals(vec3.xyz(1, -2, 3), 1e-5)).to.be.true
    expect(s.equals(vec3.xyz(4, 5, 6), 1e-5)).to.be.true
  })

  it('tests t,r,s same as decomposeTrs', () => {
    const m = mat4.trs(
      vec3.xyz(1, -2, 3),
      quat.pitchYawRollDegrees(vec3.xyz(10, -70, 30)),
      vec3.xyz(4, 5, 6)
    )

    const t = vec3.zero()
    const r = quat.zero()
    const s = vec3.zero()

    // Note that we decompose in a different order here but the right combined order is always TRS
    m.decomposeR(r)
    m.decomposeS(s)
    m.decomposeT(t)

    expect(t.equals(vec3.xyz(1, -2, 3), 1e-5)).to.be.true
    expect(r.equals(quat.pitchYawRollDegrees(vec3.xyz(10, -70, 30)), 1e-5)).to.be.true
    expect(s.equals(vec3.xyz(4, 5, 6), 1e-5)).to.be.true
  })
})
