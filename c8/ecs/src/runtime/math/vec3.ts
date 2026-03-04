import {sqlen3} from './algorithms'
/// /////////////////// Type interfaces //////////////////////

/**
 * Interface representing any object that has x, y, and z properties and hence can be used as a data
 * source to create a Vec3. In addition, Vec3Source can be used as an argument to Vec3 algorithms,
 * meaning that any object with `{x: number, y: number, z: number}` properties can be used.
 */
interface Vec3Source {
  /// //////////////////////////////// Properties ////////////////////////////////
  /**
   * Access the x component of the vector.
   */
  readonly x: number
  /**
   * Access the y component of the vector.
   */
  readonly y: number
  /**
   * Access the z component of the vector.
   */
  readonly z: number
}

/**
 * Interface representing a 3D vector. A 3D vector is represented by (x, y, z) coordinates, and can
 * represent a point in space, a directional vector, or other types of data with three ordered
 * dimensions. 3D vectors can be multiplied by 4x4 matrices (`Mat4`) using homogeneous coordinate
 * math, enabling efficient 3D geometry computation. `Vec3` objects are created with the
 * `ecs.math.vec3` `Vec3Factory`, or through operations on other `Vec3` objects.
 */
interface Vec3 extends Vec3Source {
  /// //////////////////////////////// Immutable API ////////////////////////////////
  /**
   * Create a new vector with the same components as this vector.
   *
   * API Type: Immutable API.
   *
   * @returns a new vector with the same components as this vector.
   */
  clone: () => Vec3
  /**
   * Compute the cross product of this vector and another vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the cross product with.
   * @returns the cross product of this vector and another vector.
   */
  cross: (v: Vec3) => Vec3
  /**
   * Access the vector as a homogeneous array (4 dimensions).
   *
   * API Type: Immutable API.
   *
   * @returns a homogeneous array (4 dimensions) representing the vector.
   */
  data: () => number[]
  /**
   * Compute the euclidean distance between this vector and another vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the distance to.
   * @returns the euclidean distance between this vector and v.
   */
  distanceTo: (v: Vec3Source) => number
  /**
   * Element-wise vector division.
   *
   * API Type: Immutable API.
   *
   * @param v vector to divide by.
   * @returns the result of dividing each element of this vector by each element of v.
   */
  divide: (v: Vec3Source) => Vec3
  /**
   * Compute the dot product of this vector and another vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the dot product with.
   * @returns the dot product of this vector and v.
   */
  dot: (v: Vec3Source) => number
  /**
   * Check whether two vectors are equal, with a specified floating point tolerance.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compare to.
   * @param tolerance used to judge near equality.
   * @returns true if vector components are each equal within the specified tolerance, false
   *   otherwise.
   */
  equals: (v: Vec3Source, tolerance: number) => boolean
  /**
   * Compute the length of the vector.
   *
   * API Type: Immutable API.
   *
   * @returns the length of the vector.
   */
  length: () => number
  /**
   * Subtract a vector from this vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to subtract.
   * @returns the result of subtracting v from this vector.
   */
  minus: (v: Vec3Source) => Vec3
  /**
   * Compute a linear interpolation between this vector and another vector v with a factor t such
   * that the result is thisVec * (1 - t) + v * t.
   *
   * API Type: Immutable API.
   *
   * @param v vector to interpolate with.
   * @param t factor to interpolate; should be between in 0 to 1, inclusive.
   * @returns the result of the linear interpolation.
   */
  mix: (v: Vec3Source, t: number) => Vec3
  /**
   * Return a new vector with the same direction as this vector, but with a length of 1.
   *
   * API Type: Immutable API.
   *
   * @returns a new vector with the same direction as this vector, but with a length of 1.
   */
  normalize: () => Vec3
  /**
   * Add two vectors together.
   *
   * API Type: Immutable API.
   *
   * @param v vector to add.
   * @returns the result of adding v to this vector.
   */
  plus: (v: Vec3Source) => Vec3
  /**
   * Multiply the vector by a scalar.
   *
   * API Type: Immutable API.
   *
   * @param s scalar to multiply by.
   * @returns the result of multiplying this vector by s.
   */
  scale: (s: number) => Vec3
  /**
   * Element-wise vector multiplication.
   *
   * API Type: Immutable API.
   *
   * @param v vector to multiply by.
   * @returns the result of multiplying each element of this vector by each element of v.
   */
  times: (v: Vec3Source) => Vec3
  /// //////////////////////////////// Mutable API ////////////////////////////////
  /**
   * Compute the cross product of this vector and another vector. Store the result in this Vec3
   * and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to compute cross product with.
   * @returns this vector for chaining.
   */
  setCross: (v: Vec3Source) => Vec3
  /**
   * Element-wise vector division. Store the result in this Vec3 and return this Vec3 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to divide by.
   * @returns this vector for chaining.
   */
  setDivide: (v: Vec3Source) => Vec3
  /**
   * Subtract a vector from this vector. Store the result in this Vec3 and return this Vec3
   * for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to subtract.
   * @returns this vector for chaining.
   */
  setMinus: (v: Vec3Source) => Vec3
  /**
   * Compute a linear interpolation between this vector and another vector v with a factor t such
   * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
   * result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to interpolate with.
   * @param t factor to interpolate; should be between in 0 to 1, inclusive.
   * @returns this vector for chaining.
   */
  setMix: (v: Vec3Source, t: number) => Vec3
  /**
   * Set the vector to be a version of itself with the same direction, but with length 1. Store the
   * result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this vector for chaining.
   */
  setNormalize: () => Vec3
  /**
   * Add two vectors together. Store the result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to add.
   * @returns this vector for chaining.
   */
  setPlus: (v: Vec3Source) => Vec3
  /**
   * Multiply the vector by a scalar. Store the result in this Vec3 and return this Vec3 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param s scalar to multiply by.
   * @returns this vector for chaining.
   */
  setScale: (s: number) => Vec3
  /**
   * Element-wise vector multiplication. Store the result in this Vec3 and return this Vec3
   * for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to multiply by.
   * @returns this vector for chaining.
   */
  setTimes: (v: Vec3Source) => Vec3
  /**
   * Set the Vec3's x component. Store the result in this Vec3 and return this for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v value to set this vector's x component to.
   * @returns this vector for chaining.
   */
  setX(v: number): Vec3
  /**
   * Set the Vec3's y component. Store the result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v value to set this vector's y component to.
   * @returns this vector for chaining.
   */
  setY(v: number): Vec3
  /**
   * Set the Vec3's z component. Store the result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v value to set this vector's z component to.
   * @returns this vector for chaining.
   */
  setZ(v: number): Vec3
  // //////////////////////////////// Set API ////////////////////////////////
  /**
   * Set the Vec3 to be all ones. Store the result in this Vec3 and return this Vec3 for chaining.
   *
   * API Type: Set API.
   *
   * @returns this vector for chaining.
   */
  makeOne: () => Vec3
  /**
   * Set the Vec3 to have all components set to the scale value s. Store the result in this Vec3
   * and return this Vec3 for chaining.
   *
   * API Type: Set API.
   *
   * @param s value to set all components to.
   * @returns this vector for chaining.
   */
  makeScale: (s: number) => Vec3
  /**
   * Set the Vec3 to be pointed in the positive y direction. Store the result in this Vec3 and
   * return this Vec3 for chaining.
   *
   * API Type: Set API.
   *
   * @returns this vector for chaining.
   */
  makeUp: () => Vec3
  /**
   * Set the Vec3 to be all zeros. Store the result in this Vec3 and return this Vec3 for
   * chaining.
   *
   * API Type: Set API.
   *
   * @returns this vector for chaining.
   */
  makeZero: () => Vec3
  /**
   * Set this Vec3 to have the same value as another Vec3. Store the result in this Vec3 and return
   * this Vec3 for chaining.
   *
   * API Type: Set API.
   *
   * @param v vector to copy from.
   * @returns this vector for chaining.
   */
  setFrom: (v: Vec3Source) => Vec3
  /**
   * Set the Vec3's x, y, and z components. Store the result in this Vec3 and return this for
   * chaining.
   *
   * API Type: Set API.
   *
   * @param x value to set this vector's x component to.
   * @param y value to set this vector's y component to.
   * @param z value to set this vector's z component to.
   * @returns this vector for chaining.
   */
  setXyz: (x: number, y: number, z: number) => Vec3
}

/**
 * Factory for Vec3. Vec3 objects are created with the `ecs.math.vec3` Vec3Factory.
 */
interface Vec3Factory {
  /**
   * Create a Vec3 from an object with x, y, z properties.
   *
   * API Type: Factory API.
   *
   * @param source to copy.
   * @returns a new Vec3 with the same components as the source.
   */
  from: (source: Vec3Source) => Vec3
  /**
   * Create a Vec3 with all elements set to one.
   *
   * API Type: Factory API.
   *
   * @returns a new Vec3 with all elements set to one.
   */
  one: () => Vec3
  /**
   * Create a Vec3 with all elements set to the scale value s.
   *
   * API Type: Factory API.
   *
   * @param s value to set all components to.
   * @returns a new Vec3 with all elements set to the scale value s.
   */
  scale: (s: number) => Vec3
  /**
   * Create a Vec3 pointing in the positive y direction.
   *
   * API Type: Factory API.
   *
   * @returns a new Vec3 pointing in the positive y direction.
   */
  up: () => Vec3
  /**
   * Create a Vec3 from x, y, z values.
   *
   * API Type: Factory API.
   *
   * @param x value to set the x component to.
   * @param y value to set the y component to.
   * @param z value to set the z component to.
   * @returns a new Vec3 with the x, y, and z components set to the specified values.
   */
  xyz: (x: number, y: number, z: number) => Vec3
  /**
   * Create a Vec3 with all components set to zero.
   *
   * API Type: Factory API.
   *
   * @returns a new Vec3 with all components set to zero.
   */
  zero: () => Vec3
}

/// /////////////////// Unexported Internal Factories //////////////////////

const SCRATCH_VEC: Vec3[] = []

class Vec3Impl implements Vec3 {
  private _data: number[]

  public readonly x!: number

  public readonly y!: number

  public readonly z!: number

  constructor() {
    this._data = [0, 0, 0, 1]
    Object.defineProperties(this, {
      _data: {enumerable: false},
      x: {enumerable: true, get: () => this._data[0]},
      y: {enumerable: true, get: () => this._data[1]},
      z: {enumerable: true, get: () => this._data[2]},
    })
  }

  setXyz(x: number, y: number, z: number): Vec3 {
    this._data[0] = x
    this._data[1] = y
    this._data[2] = z
    return this
  }

  setX(v: number): Vec3 {
    this._data[0] = v
    return this
  }

  setY(v: number): Vec3 {
    this._data[1] = v
    return this
  }

  setZ(v: number): Vec3 {
    this._data[2] = v
    return this
  }

  setFrom(v: Vec3Source): Vec3 {
    return this.setXyz(v.x, v.y, v.z)
  }

  clone(): Vec3 {
    return new Vec3Impl().setFrom(this)
  }

  dot(v: Vec3Source): number {
    return this.x * v.x + this.y * v.y + this.z * v.z
  }

  length(): number {
    return Math.sqrt(sqlen3(this.x, this.y, this.z))
  }

  distanceTo(v: Vec3Source): number {
    return SCRATCH_VEC[0].setFrom(v).setMinus(this).length()
  }

  equals(v: Vec3Source, tolerance: number): boolean {
    return (
      Math.abs(this.x - v.x) <= tolerance &&
      Math.abs(this.y - v.y) <= tolerance &&
      Math.abs(this.z - v.z) <= tolerance
    )
  }

  setPlus(v: Vec3Source): Vec3 {
    return this.setXyz(this.x + v.x, this.y + v.y, this.z + v.z)
  }

  plus(v: Vec3Source): Vec3 {
    return this.clone().setPlus(v)
  }

  setMinus(v: Vec3Source): Vec3 {
    return this.setXyz(this.x - v.x, this.y - v.y, this.z - v.z)
  }

  minus(v: Vec3Source): Vec3 {
    return this.clone().setMinus(v)
  }

  setMix(v: Vec3Source, t: number): Vec3 {
    return this.setPlus(SCRATCH_VEC[0].setFrom(v).setMinus(this).setScale(t))
  }

  mix(v: Vec3Source, t: number): Vec3 {
    return this.clone().setMix(v, t)
  }

  setTimes(v: Vec3Source): Vec3 {
    return this.setXyz(this.x * v.x, this.y * v.y, this.z * v.z)
  }

  times(v: Vec3Source): Vec3 {
    return this.clone().setTimes(v)
  }

  setDivide(v: Vec3Source): Vec3 {
    return this.setXyz(this.x / v.x, this.y / v.y, this.z / v.z)
  }

  divide(v: Vec3Source): Vec3 {
    return this.clone().setDivide(v)
  }

  setScale(s: number): Vec3 {
    return this.setXyz(this.x * s, this.y * s, this.z * s)
  }

  scale(s: number): Vec3 {
    return this.clone().setScale(s)
  }

  setNormalize(): Vec3 {
    return this.setScale(1 / (this.length() || 1))
  }

  normalize(): Vec3 {
    return this.clone().setNormalize()
  }

  setCross(v: Vec3Source): Vec3 {
    return this.setXyz(
      this.y * v.z - this.z * v.y,
      this.z * v.x - this.x * v.z,
      this.x * v.y - this.y * v.x
    )
  }

  cross(v: Vec3Source): Vec3 {
    return this.clone().setCross(v)
  }

  makeOne(): Vec3 {
    return this.setXyz(1, 1, 1)
  }

  makeScale(s: number): Vec3 {
    return this.setXyz(s, s, s)
  }

  makeUp(): Vec3 {
    return this.setXyz(0, 1, 0)
  }

  makeZero(): Vec3 {
    return this.setXyz(0, 0, 0)
  }

  data(): number[] {
    return this._data
  }
}

const newVec3 = (): Vec3 => new Vec3Impl()

/// /////////////////// Exported External Factories //////////////////////

const vec3: Vec3Factory = {
  from: (source: Vec3Source): Vec3 => newVec3().setXyz(source.x, source.y, source.z),
  one: (): Vec3 => newVec3().makeOne(),
  scale: (s: number): Vec3 => newVec3().makeScale(s),
  up: (): Vec3 => newVec3().makeUp(),
  xyz: (x: number, y: number, z: number): Vec3 => newVec3().setXyz(x, y, z),
  zero: (): Vec3 => newVec3().makeZero(),
}

SCRATCH_VEC.push(vec3.zero())

export type {
  Vec3,
  Vec3Factory,
  Vec3Source,
}

export {
  vec3,
}
