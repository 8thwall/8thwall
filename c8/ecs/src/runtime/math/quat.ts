import {fill4x1, fill4x4, rotation4x4ToQuat} from './algorithms'
import type {Vec3, Vec3Source} from './vec3'
import {vec3} from './vec3'

/// /////////////////// Type interfaces //////////////////////

/**
 * Interface representing any object that has x, y, z, and w properties and hence can be used as a
 * data source to create a Quat. In addition, QuatSource can be used as an argument to Quat
 * algorithms, meaning that any object with `{x: number, y: number, z: number, w: number}`
 * properties can be used.
 */
interface QuatSource {
  /// //////////////////////////////// Properties ////////////////////////////////
  /**
   * Access the x component of the quaternion.
   */
  readonly x: number
  /**
   * Access the y component of the quaternion.
   */
  readonly y: number
  /**
   * Access the z component of the quaternion.
   */
  readonly z: number
  /**
   * Access the w component of the quaternion.
   */
  readonly w: number
}

/**
 * Interface representing a quaternion.  A quaternion is represented by (x, y, z, w) coordinates,
 * and represents a 3D rotation. Quaternions can be converted to and from 4x4 rotation matrices with
 * the interfaces in `Mat4`. `Quat` objects are created with the `ecs.math.quat` `QuatFactory`, or
 * through operations on other `Quat` objects.
 */
interface Quat extends QuatSource {
  /// //////////////////////////////// Immutable API ////////////////////////////////
  /**
   * Convert the quaternion to an axis-angle representation.  The direction of the vector gives the
   * axis of rotation, and the magnitude of the vector gives the angle, in radians. If `target` is
   * supplied, the result will be stored in `target` and `target` will be returned. Otherwise, a new
   * Vec3 will be created and returned.
   *
   * API Type: Immutable API.
   *
   * @param target optional vector to store the result in.
   * @returns target if supplied, otherwise a new Vec3.
   */
  axisAngle: (target?: Vec3) => Vec3
  /**
   * Create a new quaternion with the same components as this quaternion.
   *
   * API Type: Immutable API.
   *
   * @returns a new quaternion with the same components as this quaternion.
   */
  clone: () => Quat
  /**
   * Return the rotational conjugate of this quaternion. The conjugate of a quaternion represents
   * the same rotation in the opposite direction about the rotational axis.
   *
   * API Type: Immutable API.
   *
   * @returns a new quaternion representing the rotational conjugate of this quaternion.
   */
  conjugate: () => Quat
  /**
   * Access the quaternion as an array of [x, y, z, w].
   *
   * API Type: Immutable API.
   *
   * @returns an array of [x, y, z, w].
   */
  data: () => number[]
  /**
   * Angle between two quaternions, in degrees.
   *
   * API Type: Immutable API.
   *
   * @param target quaternion to compute the angle to.
   * @returns the angle between this quaternion and the target quaternion, in degrees.
   */
  degreesTo: (target: QuatSource) => number
  /**
   * Compute the quaternion required to rotate this quaternion to the target quaternion.
   *
   * API Type: Immutable API.
   *
   * @param target quaternion to rotate towards.
   * @returns the quaternion required to rotate this quaternion to the target quaternion.
   */
  delta: (target: QuatSource) => Quat
  /**
   * Compute the dot product of this quaternion with another quaternion.
   *
   * API Type: Immutable API.
   *
   * @param q quaternion to compute the dot product with.
   * @returns the dot product of this quaternion with the target quaternion.
   */
  dot: (q: QuatSource) => number
  /**
   * Check whether two quaternions are equal, with a specified floating point tolerance.
   *
   * API Type: Immutable API.
   *
   * @param v quaternion to compare to.
   * @param tolerance used to judge near equality.
   * @returns true if quaternions components are each equal within the specified tolerance, false
   *   otherwise.
   */
  equals: (q: QuatSource, tolerance: number) => boolean
  /**
   * Compute the quaternion which multiplies this quaternion to get a zero rotation quaternion.
   *
   * API Type: Immutable API.
   *
   * @returns the inverse of this quaternion.
   */
  inv: () => Quat
  /**
   * Negate all components of this quaternion. The result is a quaternion representing the same
   * rotation as this quaternion.
   *
   * API Type: Immutable API.
   *
   * @returns the negated quaternion.
   */
  negate: () => Quat
  /**
   * Get the normalized version of this quaternion with a length of 1.
   *
   * API Type: Immutable API.
   *
   * @returns the normalized quaternion.
   */
  normalize: () => Quat
  /**
   * Convert the quaternion to pitch, yaw, and roll angles in radians.
   *
   * API Type: Immutable API.
   *
   * @param target optional vector to store the result in.
   * @returns target if supplied, otherwise a new Vec3.
   */
  pitchYawRollRadians: (target?: Vec3) => Vec3
  /**
   * Convert the quaternion to pitch, yaw, and roll angles in degrees.
   *
   * API Type: Immutable API.
   *
   * @param target optional vector to store the result in.
   * @returns target if supplied, otherwise a new Vec3.
   */
  pitchYawRollDegrees: (target?: Vec3) => Vec3
  /**
   * Add two quaternions together.
   *
   * API Type: Immutable API.
   *
   * @param q quaternion to add.
   * @returns the sum of this quaternion and the target quaternion.
   */
  plus: (q: QuatSource) => Quat
  /**
   * Angle between two quaternions, in radians.
   *
   * API Type: Immutable API.
   *
   * @param target quaternion to compute the angle to.
   * @returns the angle between this quaternion and the target quaternion, in radians.
   */
  radiansTo: (target: QuatSource) => number
  /**
   * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
   * the target.
   *
   * API Type: Immutable API.
   *
   * @param target quaternion to rotate towards.
   * @param radians number of radians to rotate.
   * @returns the rotated quaternion.
   */
  rotateToward: (target: QuatSource, radians: number) => Quat
  /**
   * Spherical interpolation between two quaternions given a provided interpolation value. If the
   * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
   * 1, then it will return the target quaternion.
   *
   * API Type: Immutable API.
   *
   * @param target quaternion to interpolate towards.
   * @param t factor to interpolate; should be between in 0 to 1, inclusive.
   * @returns the interpolated quaternion.
   */
  slerp: (target: QuatSource, t: number) => Quat
  /**
   * Multiply two quaternions together.
   *
   * API Type: Immutable API.
   *
   * @param q quaternion to multiply.
   * @returns the product of this quaternion and the target quaternion.
   */
  times: (q: QuatSource) => Quat
  /**
   * Multiply the quaternion by a vector. This is equivalent to converting the quaternion to a
   * rotation matrix and multiplying the matrix by the vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to multiply.
   * @param target optional vector to store the result in.
   * @returns target if supplied, otherwise a new Vec3.
   */
  timesVec: (v: Vec3Source, target?: Vec3) => Vec3
  /// //////////////////////////////// Mutable API ////////////////////////////////
  /**
   * Set this quaternion to its rotational conjugate. The conjugate of a quaternion represents the
   * same rotation in the opposite direction about the rotational axis. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this quaternion for chaining.
   */
  setConjugate: () => Quat
  /**
   * Compute the quaternion required to rotate this quaternion to the target quaternion. Store the
   * result in this Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param target quaternion to rotate towards.
   * @returns this quaternion for chaining.
   */
  setDelta: (target: QuatSource) => Quat
  /**
   * Set this quaternion to the value in another quaternion. Store the result in this Quat and
   * return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param q quaternion to set from.
   * @returns this quaternion for chaining.
   */
  setFrom: (q: QuatSource) => Quat
  /**
   * Set this to the quaternion which multiplies this quaternion to get a zero rotation quaternion.
   * Store the result in this Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this quaternion for chaining.
   */
  setInv: () => Quat
  /**
   * Negate all components of this quaternion. The result is a quaternion representing the same
   * rotation as this quaternion. Store the result in this Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this quaternion for chaining.
   */
  setNegate: () => Quat
  /**
   * Get the normalized version of this quaternion with a length of 1. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this quaternion for chaining.
   */
  setNormalize: () => Quat
  /**
   * Add this quaternion to another quaternion. Store the result in this Quat and return this
   * Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param q quaternion to add.
   * @returns this quaternion for chaining.
   */
  setPlus: (q: QuatSource) => Quat
  /**
   * Set this quaternion to the result of q times this quaternion. Store the result in this Quat
   * and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param q quaternion to premultiply.
   * @returns this quaternion for chaining.
   */
  setPremultiply: (q: QuatSource) => Quat
  /**
   * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
   * the target. Store the result in this Quat and return this Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param target quaternion to rotate towards.
   * @param radians number of radians to rotate.
   * @returns this quaternion for chaining.
   */
  setRotateToward: (target: QuatSource, radians: number) => Quat
  /**
   * Spherical interpolation between two quaternions given a provided interpolation value. If the
   * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
   * 1, then it will return the target quaternion. Store the result in this Quat and return this
   * Quat for chaining.
   *
   * API Type: Mutable API.
   *
   * @param target quaternion to interpolate towards.
   * @param t factor to interpolate; should be between in 0 to 1, inclusive.
   * @returns this quaternion for chaining.
   */
  setSlerp: (target: QuatSource, t: number) => Quat
  /**
   * Multiply two quaternions together. Store the result in this Quat and return this Quat for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param q quaternion to multiply.
   * @returns this quaternion for chaining.
   */
  setTimes: (q: QuatSource) => Quat
  /// //////////////////////////////// Set API ////////////////////////////////
  /**
   * Set the quaternion to the specified x, y, z and w values. Store the result in this Quat and
   * return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param x x component of the quaternion.
   * @param y y component of the quaternion.
   * @param z z component of the quaternion.
   * @param w w component of the quaternion.
   * @returns this quaternion for chaining.
   */
  setXyzw: (x: number, y: number, z: number, w: number) => Quat
  /**
   * Set a Quat from an axis-angle representation. The direction of the vector gives the axis of
   * rotation, and the magnitude of the vector gives the angle, in radians. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param aa vector containing the axis-angle representation of the rotation.
   * @returns this quaternion for chaining.
   */
  makeAxisAngle: (aa: Vec3Source) => Quat
  /**
   * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in radians. Store the
   * result in this Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param v vector containing the pitch, yaw, and roll angles in radians.
   * @returns this quaternion for chaining.
   */
  makePitchYawRollRadians: (v: Vec3Source) => Quat
  /**
   * Set the quaternion to a rotation that would cause the eye to look at the target with the given
   * up vector. Store the result in this Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param eye vector where the eye is located.
   * @param target vector where the target is located.
   * @param up vector representing the up direction from the eye's perspective.
   * @returns this quaternion for chaining.
   */
  makeLookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat
  /**
   * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in degrees. Store the
   * result in this Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param v vector containing the pitch, yaw, and roll angles in degrees.
   * @returns this quaternion for chaining.
   */
  makePitchYawRollDegrees: (v: Vec3Source) => Quat
  /**
   * Set the quaternion to a rotation about the x-axis (pitch) in degrees. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param degrees the angle of rotation in degrees.
   * @returns this quaternion for chaining.
   */
  makeXDegrees: (degrees: number) => Quat
  /**
   * Set the quaternion to a rotation about the x-axis (pitch) in radians. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param radians the angle of rotation in radians.
   * @returns this quaternion for chaining.
   */
  makeXRadians: (radians: number) => Quat
  /**
   * Set the quaternion to a rotation about the y-axis (yaw) in degrees. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param degrees the angle of rotation in degrees.
   * @returns this quaternion for chaining.
   */
  makeYDegrees: (degrees: number) => Quat
  /**
   * Set the quaternion to a rotation about the y-axis (yaw) in radians. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param radians the angle of rotation in radians.
   * @returns this quaternion for chaining.
   */
  makeYRadians: (radians: number) => Quat
  /**
   * Set the quaternion to a rotation about the z-axis (roll) in degrees. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param degrees the angle of rotation in degrees.
   * @returns this quaternion for chaining.
   */
  makeZDegrees: (degrees: number) => Quat
  /**
   * Set the quaternion to a rotation about the z-axis (roll) in radians. Store the result in this
   * Quat and return this Quat for chaining.
   *
   * API Type: Set API.
   *
   * @param radians the angle of rotation in radians.
   * @returns this quaternion for chaining.
   */
  makeZRadians: (radians: number) => Quat
  /**
   * Set the quaternion to a zero rotation. Store the result in this Quat and return this Quat for
   * chaining.
   *
   * API Type: Set API.
   *
   * @returns this quaternion for chaining.
   */
  makeZero: () => Quat
}

/**
 * Factory for Quat. Quat objects are created with the `ecs.math.quat` QuatFactory.
 */
interface QuatFactory {
  /**
   * Create a Quat from an axis-angle representation. The direction of the `aa` vector gives the
   * axis of rotation, and the magnitude of the vector gives the angle, in radians. For example,
   * `quat.axisAngle(vec3.up().scale(Math.PI / 2))` represents a 90-degree rotation about the
   * y-axis, and is equivalent to `quat.yDegrees(90)`. If `target` is supplied, the result will be
   * stored in `target` and `target` will be returned. Otherwise, a new Quat will be created and
   * returned.
   *
   * API Type: Factory API.
   *
   * @param aa vector containing the axis-angle representation of the rotation.
   * @param target optional quaternion to store the result in.
   * @returns target if supplied, otherwise a new Quat.
   */
  axisAngle: (aa: Vec3Source, target?: Quat) => Quat
  /**
   * Create a Quat from an object with x, y, z, w properties.
   *
   * API Type: Factory API.
   *
   * @param source object with x, y, z, w properties.
   * @returns a new Quat with the same components as the source object.
   */
  from: (source: QuatSource) => Quat
  /**
   * Create a Quat representing the rotation required for an object positioned at `eye` to look at
   * an object positioned at `target`, with the given `up` vector.
   *
   * API Type: Factory API.
   *
   * @param eye vector where the eye is located.
   * @param target vector where the target is located.
   * @param up vector representing the up direction from the eye's perspective.
   * @returns a new Quat representing the rotation required for an object positioned at `eye` to
   *  look at an object positioned at `target`, with the given `up` vector.
   */
  lookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat
  /**
   * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
   * angles. Rotation is specified in degrees.
   *
   * API Type: Factory API.
   *
   * @param v vector containing the pitch, yaw, and roll angles in degrees.
   * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
   *   degrees.
   */
  pitchYawRollDegrees: (v: Vec3Source) => Quat
  /**
   * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
   * angles. Rotation is specified in radians.
   *
   * API Type: Factory API.
   *
   * @param v rotation specified in radians.
   * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
   *   radians.
   */
  pitchYawRollRadians: (v: Vec3Source) => Quat
  /**
   * Create a Quat which represents a rotation about the x-axis. Rotation is specified in degrees.
   *
   * API Type: Factory API.
   *
   * @param degrees to rotate.
   * @returns a new Quat representing the rotation.
   */
  xDegrees: (degrees: number) => Quat
  /**
   * Create a Quat which represents a rotation about the x-axis. Rotation is specified in radians.
   *
   * API Type: Factory API.
   *
   * @param radians to rotate.
   * @returns a new Quat representing the rotation.
   */
  xRadians: (radians: number) => Quat
  /**
   * Create a Quat from the specified x, y, z, and w values.
   *
   * API Type: Factory API.
   *
   * @param x component of the quaternion.
   * @param y component of the quaternion.
   * @param z component of the quaternion.
   * @param w component of the quaternion.
   * @returns a new Quat with the specified components.
   */
  xyzw: (x: number, y: number, z: number, w: number) => Quat
  /**
   * Create a Quat which represents a rotation about the y-axis. Rotation is specified in degrees.
   *
   * API Type: Factory API.
   *
   * @param degrees to rotate.
   * @returns a new Quat representing the rotation.
   */
  yDegrees: (degrees: number) => Quat
  /**
   * Create a Quat which represents a rotation about the y-axis. Rotation is specified in radians.
   *
   * API Type: Factory API.
   *
   * @param radians to rotate.
   * @returns a new Quat representing the rotation.
   */
  yRadians: (radians: number) => Quat
  /**
   * Create a Quat which represents a rotation about the z-axis. Rotation is specified in degrees.
   *
   * API Type: Factory API.
   *
   * @param degrees to rotate.
   * @returns a new Quat representing the rotation.
   */
  zDegrees: (degrees: number) => Quat
  /**
   * Create a Quat which represents a rotation about the z-axis. Rotation is specified in radians.
   *
   * API Type: Factory API.
   *
   * @param radians to rotate.
   * @returns a new Quat representing the rotation.
   */
  zRadians: (radians: number) => Quat
  /**
   * Create a Quat which represents a zero rotation.
   *
   * API Type: Factory API.
   *
   * @returns a new Quat representing a zero rotation.
   */
  zero: () => Quat
}

/// /////////////////// Unexported Internal Factories //////////////////////

const DEG_TO_RAD = Math.PI / 180
const RAD_TO_DEG = 180 / Math.PI

const clip = (value: number, min: number, max: number): number => (
  Math.min(Math.max(value, min), max)
)

const SCRATCH_Q: QuatImpl[] = []  // These are allocated after the definition of factory methods.
const SCRATCH_V: [Vec3, Vec3, Vec3] = [vec3.zero(), vec3.zero(), vec3.zero()]

const SCRATCH_4: [number, number, number, number] = [0, 0, 0, 0]
const SCRATCH_4X4: number[] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

class QuatImpl implements Quat {
  private _data: number[]

  public readonly x!: number

  public readonly y!: number

  public readonly z!: number

  public readonly w!: number

  constructor() {
    this._data = [0, 0, 0, 1]
    Object.defineProperties(this, {
      _data: {enumerable: false},
      x: {enumerable: true, get: () => this._data[0]},
      y: {enumerable: true, get: () => this._data[1]},
      z: {enumerable: true, get: () => this._data[2]},
      w: {enumerable: true, get: () => this._data[3]},
    })
  }

  setXyzw(x: number, y: number, z: number, w: number): Quat {
    this._data[0] = x
    this._data[1] = y
    this._data[2] = z
    this._data[3] = w
    return this
  }

  setFrom(q: QuatSource): Quat {
    return this.setXyzw(q.x, q.y, q.z, q.w)
  }

  clone(): Quat {
    return new QuatImpl().setFrom(this)
  }

  makeXRadians(radians: number): Quat {
    const hr = radians * 0.5
    return this.setXyzw(Math.sin(hr), 0, 0, Math.cos(hr))
  }

  makeXDegrees(degrees: number): Quat {
    return this.makeXRadians(degrees * DEG_TO_RAD)
  }

  makeYRadians(radians: number): Quat {
    const hr = radians * 0.5
    return this.setXyzw(0, Math.sin(hr), 0, Math.cos(hr))
  }

  makeYDegrees(degrees: number): Quat {
    return this.makeYRadians(degrees * DEG_TO_RAD)
  }

  makeZRadians(radians: number): Quat {
    const hr = radians * 0.5
    return this.setXyzw(0, 0, Math.sin(hr), Math.cos(hr))
  }

  makeZDegrees(degrees: number): Quat {
    return this.makeZRadians(degrees * DEG_TO_RAD)
  }

  pitchYawRollRadians(target: Vec3 = vec3.zero()): Vec3 {
    // Convert quaternion to y-x-z euler angles.
    const wx = this.w * this.x
    const wy = this.w * this.y
    const wz = this.w * this.z
    const xx = this.x * this.x
    const xy = this.x * this.y
    const xz = this.x * this.z
    const yy = this.y * this.y
    const yz = this.y * this.z
    const zz = this.z * this.z

    const m00 = 1.0 - 2.0 * (yy + zz)
    const m02 = 2.0 * (xz + wy)
    const m10 = 2.0 * (xy + wz)
    const m11 = 1.0 - 2.0 * (xx + zz)
    const m12 = 2.0 * (yz - wx)
    const m20 = 2.0 * (xz - wy)
    const m22 = 1.0 - 2.0 * (xx + yy)

    const vx = Math.asin(Math.max(-1.0, Math.min(-m12, 1.0)))
    // If pitch is 1 or -1, the camera is facing straight up or straight down. In this plane, yaw
    // and roll are interchangeable and ambiguous. In this case, we resolve ties in favor of
    // yaw-only motion (no roll).
    const fullPitch = Math.abs(m12) < (1.0 - 1e-7)
    const vy = fullPitch ? Math.atan2(m02, m22) : Math.atan2(-m20, m00)
    const vz = fullPitch ? Math.atan2(m10, m11) : 0.0
    return target.setXyz(vx, vy, vz)
  }

  pitchYawRollDegrees(out: Vec3 = vec3.zero()): Vec3 {
    return this.pitchYawRollRadians(out).setScale(RAD_TO_DEG)
  }

  axisAngle(target: Vec3 = vec3.zero()): Vec3 {
    // For quaternions representing non-zero rotation, the conversion is numerically stable.
    const sinSquared = this.x * this.x + this.y * this.y + this.z * this.z
    if (sinSquared > 0) {
      const sinTheta = Math.sqrt(sinSquared)
      const k = (2 * Math.atan2(sinTheta, this.w)) / sinTheta
      return target.setXyz(this.x * k, this.y * k, this.z * k)
    }
    // If sinSquared is 0, then we will get NaNs when dividing by sinTheta.  By approximating with a
    // Taylor series, and truncating at one term, the value will be computed correctly.
    return target.setXyz(this.x * 2.0, this.y * 2.0, this.z * 2.0)
  }

  setConjugate(): Quat {
    this._data[0] = -this._data[0]
    this._data[1] = -this._data[1]
    this._data[2] = -this._data[2]
    return this
  }

  conjugate(): Quat {
    return this.clone().setConjugate()
  }

  squaredNorm(): number {
    return this.x * this.x + this.y * this.y + this.z * this.z + this.w * this.w
  }

  setNegate(): Quat {
    this._data[0] = -this._data[0]
    this._data[1] = -this._data[1]
    this._data[2] = -this._data[2]
    this._data[3] = -this._data[3]
    return this
  }

  negate(): Quat {
    return this.clone().setNegate()
  }

  norm(): number {
    return Math.sqrt(this.squaredNorm())
  }

  setNormalize(): Quat {
    const n = this.norm()
    const invN = n > 0 ? 1 / n : 1
    this._data[0] *= invN
    this._data[1] *= invN
    this._data[2] *= invN
    this._data[3] *= invN
    return this
  }

  normalize(): Quat {
    return this.clone().setNormalize()
  }

  setInv(): Quat {
    return this.setConjugate().setNormalize()
  }

  inv(): Quat {
    return this.clone().setInv()
  }

  setPlus(q: QuatSource): Quat {
    this._data[0] += q.x
    this._data[1] += q.y
    this._data[2] += q.z
    this._data[3] += q.w
    return this.setNormalize()
  }

  plus(q: QuatSource): Quat {
    return this.clone().setPlus(q)
  }

  setTimes(q: QuatSource): Quat {
    return this.setXyzw(
      this.w * q.x + this.x * q.w + this.y * q.z - this.z * q.y,
      this.w * q.y - this.x * q.z + this.y * q.w + this.z * q.x,
      this.w * q.z + this.x * q.y - this.y * q.x + this.z * q.w,
      this.w * q.w - this.x * q.x - this.y * q.y - this.z * q.z
    ).setNormalize()
  }

  times(q: QuatSource): Quat {
    return this.clone().setTimes(q)
  }

  timesVec(v: Vec3Source, target: Vec3 = vec3.zero()): Vec3 {
    const {x, y, z, w} = this
    const x2 = x + x
    const y2 = y + y
    const z2 = z + z
    const wx2 = w * x2
    const wy2 = w * y2
    const wz2 = w * z2
    const xx2 = x * x2
    const xy2 = x * y2
    const xz2 = x * z2
    const yy2 = y * y2
    const yz2 = y * z2
    const zz2 = z * z2
    return target.setXyz(
      v.x * (1 - (yy2 + zz2)) + v.y * (xy2 - wz2) + v.z * (xz2 + wy2),
      v.x * (xy2 + wz2) + v.y * (1 - (xx2 + zz2)) + v.z * (yz2 - wx2),
      v.x * (xz2 - wy2) + v.y * (yz2 + wx2) + v.z * (1 - (xx2 + yy2))
    )
  }

  setPremultiply(q: QuatSource): Quat {
    return this.setXyzw(
      q.w * this.x + q.x * this.w + q.y * this.z - q.z * this.y,
      q.w * this.y - q.x * this.z + q.y * this.w + q.z * this.x,
      q.w * this.z + q.x * this.y - q.y * this.x + q.z * this.w,
      q.w * this.w - q.x * this.x - q.y * this.y - q.z * this.z
    ).setNormalize()
  }

  dot(q: QuatSource): number {
    return this.x * q.x + this.y * q.y + this.z * q.z + this.w * q.w
  }

  radiansTo(q: QuatSource): number {
    return Math.acos(2 * (clip(this.dot(q), -1, 1) ** 2) - 1)
  }

  degreesTo(q: QuatSource): number {
    return this.radiansTo(q) * RAD_TO_DEG
  }

  setDelta(target: QuatSource): Quat {
    return this.setInv().setPremultiply(target)
  }

  delta(target: QuatSource): Quat {
    return this.clone().setDelta(target)
  }

  setSlerp(target: QuatSource, t: number): Quat {
    this.setNormalize()
    const a = this
    const b = SCRATCH_Q[0].setFrom(target).setNormalize()

    let dp = a.dot(b)
    if (dp < 0.0) {
      b.setNegate()
      dp *= -1.0
    }

    if (dp > 0.995) {
      return this.setXyzw(
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        a.w + t * (b.w - a.w)
      ).setNormalize()
    }

    const sqrSinHalfTheta = 1.0 - dp * dp
    const sinHalfTheta = Math.sqrt(sqrSinHalfTheta)
    const r0 = Math.atan2(sinHalfTheta, dp)
    const sr0 = Math.sin(r0)
    const r = t * r0
    const sr = Math.sin(r)
    const ta = Math.cos(r) - (dp * sr) / sr0
    const tb = sr / sr0
    return this.setXyzw(
      ta * a.x + tb * b.x,
      ta * a.y + tb * b.y,
      ta * a.z + tb * b.z,
      ta * a.w + tb * b.w
    ).setNormalize()
  }

  slerp(target: QuatSource, t: number): Quat {
    return this.clone().setSlerp(target, t)
  }

  setRotateToward(target: QuatSource, rads: number): Quat {
    const total = this.radiansTo(target)
    const deltaRads = clip(0, rads, total)
    return this.setSlerp(target, deltaRads / total)
  }

  rotateToward(target: QuatSource, rads: number): Quat {
    return this.clone().setRotateToward(target, rads)
  }

  equals(q: QuatSource, tolerance: number): boolean {
    return (
      Math.abs(this.x - q.x) <= tolerance &&
      Math.abs(this.y - q.y) <= tolerance &&
      Math.abs(this.z - q.z) <= tolerance &&
      Math.abs(this.w - q.w) <= tolerance
    )
  }

  // Create a Quat from an axis-angle representation.
  makeAxisAngle(aa: Vec3Source): Quat {
    const a0 = aa.x
    const a1 = aa.y
    const a2 = aa.z
    const thetaSquared = a0 * a0 + a1 * a1 + a2 * a2
    // For points not at the origin, the full conversion is numerically stable.
    if (thetaSquared > 0.0) {
      const theta = Math.sqrt(thetaSquared)
      const halfTheta = theta * 0.5
      const k = Math.sin(halfTheta) / theta
      return this.setXyzw(a0 * k, a1 * k, a2 * k, Math.cos(halfTheta))
    }

    // If thetaSquared is 0, then we will get NaNs when dividing by theta.  By approximating with a
    // Taylor series, and truncating at one term, the value will be computed correctly.
    const k = 0.5
    return this.setXyzw(a0 * k, a1 * k, a2 * k, 1.0)
  }

  makeLookAt(eye: Vec3Source, target: Vec3Source, up: Vec3Source): Quat {
    const dir = SCRATCH_V[0].setFrom(target).setMinus(eye)

    if (dir.length() === 0) {
      dir.setXyz(0, 0, 1)  // eye and target are in the same position
    }

    dir.setNormalize()
    const cross = SCRATCH_V[1].setFrom(up).setCross(dir)
    if (cross.length() === 0) {
      // up and z are parallel
      if (Math.abs(up.z) === 1) {
        dir.setPlus(SCRATCH_V[2].setXyz(0.0001, 0, 0)).setNormalize()
      } else {
        dir.setPlus(SCRATCH_V[2].setXyz(0, 0, 0.0001)).setNormalize()
      }
      cross.setFrom(up).setCross(dir)
    }
    cross.setNormalize()
    const cross2 = SCRATCH_V[2].setFrom(dir).setCross(cross)

    const q = fill4x1(SCRATCH_4, 0, 0, 0, 1)
    rotation4x4ToQuat(fill4x4(
      SCRATCH_4X4,
      cross.x, cross.y, cross.z, 0,
      cross2.x, cross2.y, cross2.z, 0,
      dir.x, dir.y, dir.z, 0,
      0, 0, 0, 1
    ), q)
    return this.setXyzw(q[0], q[1], q[2], q[3])
  }

  // Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ euler
  // angles.
  // The following formulas are generated with
  /*
  from sympy import Quaternion, symbols, ccode, sin, cos
  sinHalfX = symbols('sinHalfX')
  sinHalfY = symbols('sinHalfY')
  sinHalfZ = symbols('sinHalfZ')
  cosHalfX = symbols('cosHalfX')
  cosHalfY = symbols('cosHalfY')
  cosHalfZ = symbols('cosHalfZ')
  NOTE(dat): Quaternion is w,x,y,z. Niantic version is x,y,z,w.
  Compare these to our implementation of makeYRadians
  def yQuat(sinR, cosR):
    return Quaternion( cosR, 0, sinR, 0)

  def zQuat(sinR, cosR):
    return Quaternion( cosR, 0, 0, sinR,)

  def xQuat(sinR, cosR):
    return Quaternion( cosR, sinR, 0, 0, )

  output_q = yQuat(sinHalfY, cosHalfY)
    .mul(xQuat(sinHalfX, cosHalfX))
    .mul(zQuat(sinHalfZ, cosHalfZ))
  print(ccode(output_q.b), ',')
  print(ccode(output_q.c), ',')
  print(ccode(output_q.d), ',')
  print(ccode(output_q.a))
  */
  makePitchYawRollRadians(v: Vec3Source): Quat {
    const halfX = v.x * 0.5
    const halfY = v.y * 0.5
    const halfZ = v.z * 0.5
    const cosHalfX = Math.cos(halfX)
    const cosHalfY = Math.cos(halfY)
    const cosHalfZ = Math.cos(halfZ)
    const sinHalfX = Math.sin(halfX)
    const sinHalfY = Math.sin(halfY)
    const sinHalfZ = Math.sin(halfZ)

    return this.setXyzw(
      cosHalfX * sinHalfY * sinHalfZ + cosHalfY * cosHalfZ * sinHalfX,
      cosHalfX * cosHalfZ * sinHalfY - cosHalfY * sinHalfX * sinHalfZ,
      cosHalfX * cosHalfY * sinHalfZ - cosHalfZ * sinHalfX * sinHalfY,
      cosHalfX * cosHalfY * cosHalfZ + sinHalfX * sinHalfY * sinHalfZ
    )
  }

  makePitchYawRollDegrees(v: Vec3Source): Quat {
    return this.makePitchYawRollRadians(SCRATCH_V[0].setFrom(v).setScale(DEG_TO_RAD))
  }

  // Create a Quat which represents a zero rotation.
  makeZero(): Quat {
    return this.setXyzw(0, 0, 0, 1)
  }

  data(): number[] {
    return this._data
  }
}

const newQuat = (): Quat => new QuatImpl()

/// /////////////////// Exported External Factories //////////////////////

const quat: QuatFactory = {
  axisAngle: (aa: Vec3Source, target: Quat = newQuat()): Quat => target.makeAxisAngle(aa),
  lookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source): Quat => (
    newQuat().makeLookAt(eye, target, up)
  ),
  from: (source: QuatSource): Quat => (
    newQuat().setXyzw(source.x, source.y, source.z, source.w)
  ),
  pitchYawRollDegrees: (v: Vec3Source): Quat => newQuat().makePitchYawRollDegrees(v),
  pitchYawRollRadians: (v: Vec3Source): Quat => newQuat().makePitchYawRollRadians(v),
  xDegrees: (degrees: number): Quat => newQuat().makeXDegrees(degrees),
  xRadians: (radians: number): Quat => newQuat().makeXRadians(radians),
  xyzw: (x: number, y: number, z: number, w: number): Quat => (
    newQuat().setXyzw(x, y, z, w)
  ),
  yDegrees: (degrees: number): Quat => newQuat().makeYDegrees(degrees),
  yRadians: (radians: number): Quat => newQuat().makeYRadians(radians),
  zDegrees: (degrees: number): Quat => newQuat().makeZDegrees(degrees),
  zRadians: (radians: number): Quat => newQuat().makeZRadians(radians),
  zero: (): Quat => newQuat().makeZero(),
}

SCRATCH_Q.push(new QuatImpl())

export type {
  Quat,
  QuatFactory,
  QuatSource,
}

export {
  quat,
}
