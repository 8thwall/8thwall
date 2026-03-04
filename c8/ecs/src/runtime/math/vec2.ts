/// /////////////////// Type interfaces //////////////////////

/**
 * Interface representing any object that has x and y properties and hence can be used as a data
 * source to create a Vec2. In addition, Vec2Source can be used as an argument to Vec2 algorithms,
 * meaning that any object with `{x: number, y: number}` properties can be used.
 */
interface Vec2Source {
  /// //////////////////////////////// Properties ////////////////////////////////
  /**
   * Access the x component of the vector.
   */
  readonly x: number
  /**
   * Access the y component of the vector.
   */
  readonly y: number
}

/**
 * Interface representing a 2D vector. A 2D vector is represented by (x, y) coordinates, and can
 * represent a point in space, a directional vector, or other types of data with three ordered
 * dimensions. `Vec2` objects are created with the `ecs.math.vec2` `Vec2Factory`, or through
 * operations on other `Vec2` objects.
 */
interface Vec2 extends Vec2Source {
  /// //////////////////////////////// Immutable API ////////////////////////////////
  /**
   * Create a new vector with the same components as this vector.
   *
   * API Type: Immutable API.
   *
   * @returns a new vector with the same components as this vector.
   */
  clone: () => Vec2
  /**
   * Compute the cross product of this vector and another vector. For 2D vectors, the cross product
   * is the magnitude of the z component of the 3D cross product of the two vectors with 0 as the z
   * component.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the cross product with.
   * @returns the cross product of this vector and another vector.
   */
  cross: (v: Vec2) => number
  /**
   * Compute the euclidean distance between this vector and another vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the distance to.
   * @returns the euclidean distance between this vector and v.
   */
  distanceTo: (v: Vec2Source) => number
  /**
   * Element-wise vector division.
   *
   * API Type: Immutable API.
   *
   * @param v vector to divide by.
   * @returns the result of dividing each element of this vector by each element of v.
   */
  divide: (v: Vec2Source) => Vec2
  /**
   * Compute the dot product of this vector and another vector.
   *
   * API Type: Immutable API.
   *
   * @param v vector to compute the dot product with.
   * @returns the dot product of this vector and v.
   */
  dot: (v: Vec2Source) => number
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
  equals: (v: Vec2Source, tolerance: number) => boolean
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
  minus: (v: Vec2Source) => Vec2
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
  mix: (v: Vec2Source, t: number) => Vec2
  /**
   * Return a new vector with the same direction as this vector, but with a length of 1.
   *
   * API Type: Immutable API.
   *
   * @returns a new vector with the same direction as this vector, but with a length of 1.
   */
  normalize: () => Vec2
  /**
   * Add two vectors together.
   *
   * API Type: Immutable API.
   *
   * @param v vector to add.
   * @returns the result of adding v to this vector.
   */
  plus: (v: Vec2Source) => Vec2
  /**
   * Multiply the vector by a scalar.
   *
   * API Type: Immutable API.
   *
   * @param s scalar to multiply by.
   * @returns the result of multiplying this vector by s.
   */
  scale: (s: number) => Vec2
  /**
   * Element-wise vector multiplication.
   *
   * API Type: Immutable API.
   *
   * @param v vector to multiply by.
   * @returns the result of multiplying each element of this vector by each element of v.
   */
  times: (v: Vec2Source) => Vec2
  /// //////////////////////////////// Mutable API ////////////////////////////////
  /**
   * Element-wise vector division. Store the result in this Vec2 and return this Vec2 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to divide by.
   * @returns this vector for chaining.
   */
  setDivide: (v: Vec2Source) => Vec2
  /**
   * Subtract a vector from this vector. Store the result in this Vec2 and return this Vec2
   * for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to subtract.
   * @returns this vector for chaining.
   */
  setMinus: (v: Vec2Source) => Vec2
  /**
   * Compute a linear interpolation between this vector and another vector v with a factor t such
   * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
   * result in this Vec2 and return this Vec2 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to interpolate with.
   * @param t factor to interpolate; should be between in 0 to 1, inclusive.
   * @returns this vector for chaining.
   */
  setMix: (v: Vec2Source, t: number) => Vec2
  /**
   * Set the vector to be a version of itself with the same direction, but with length 1. Store the
   * result in this Vec2 and return this Vec2 for chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this vector for chaining.
   */
  setNormalize: () => Vec2
  /**
   * Add two vectors together. Store the result in this Vec2 and return this Vec2 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to add.
   * @returns this vector for chaining.
   */
  setPlus: (v: Vec2Source) => Vec2
  /**
   * Multiply the vector by a scalar. Store the result in this Vec2 and return this Vec2 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param s scalar to multiply by.
   * @returns this vector for chaining.
   */
  setScale: (s: number) => Vec2
  /**
   * Element-wise vector multiplication. Store the result in this Vec2 and return this Vec2
   * for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v vector to multiply by.
   * @returns this vector for chaining.
   */
  setTimes: (v: Vec2Source) => Vec2
  /**
   * Set the Vec2's x component. Store the result in this Vec2 and return this for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v value to set this vector's x component to.
   * @returns this vector for chaining.
   */
  setX(v: number): Vec2
  /**
   * Set the Vec2's y component. Store the result in this Vec2 and return this Vec2 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param v value to set this vector's y component to.
   * @returns this vector for chaining.
   */
  setY(v: number): Vec2
  // //////////////////////////////// Set API ////////////////////////////////
  /**
   * Set the Vec2 to be all ones. Store the result in this Vec2 and return this Vec2 for chaining.
   *
   * API Type: Set API.
   *
   * @returns this vector for chaining.
   */
  makeOne: () => Vec2
  /**
   * Set the Vec2 to have all components set to the scale value s. Store the result in this Vec2
   * and return this Vec2 for chaining.
   *
   * API Type: Set API.
   *
   * @param s value to set all components to.
   * @returns this vector for chaining.
   */
  makeScale: (s: number) => Vec2
  /**
   * Set the Vec2 to be all zeros. Store the result in this Vec2 and return this Vec2 for
   * chaining.
   *
   * API Type: Set API.
   *
   * @returns this vector for chaining.
   */
  makeZero: () => Vec2
  /**
   * Set this Vec2 to have the same value as another Vec2. Store the result in this Vec2 and return
   * this Vec2 for chaining.
   *
   * API Type: Set API.
   *
   * @param v vector to copy from.
   * @returns this vector for chaining.
   */
  setFrom: (v: Vec2Source) => Vec2
  /**
   * Set the Vec2's x and y components. Store the result in this Vec2 and return this for chaining.
   *
   * API Type: Set API.
   *
   * @param x value to set this vector's x component to.
   * @param y value to set this vector's y component to.
   * @returns this vector for chaining.
   */
  setXy: (x: number, y: number) => Vec2
}

/**
 * Factory for Vec2. Vec2 objects are created with the `ecs.math.vec2` Vec2Factory.
 */
interface Vec2Factory {
  /**
   * Create a Vec2 from an object with x, y properties.
   *
   * API Type: Factory API.
   *
   * @param source to copy.
   * @returns a new Vec2 with the same components as the source.
   */
  from: (source: Vec2Source) => Vec2
  /**
   * Create a Vec2 with all elements set to one.
   *
   * API Type: Factory API.
   *
   * @returns a new Vec2 with all elements set to one.
   */
  one: () => Vec2
  /**
   * Create a Vec2 with all elements set to the scale value s.
   *
   * API Type: Factory API.
   *
   * @param s value to set all components to.
   * @returns a new Vec2 with all elements set to the scale value s.
   */
  scale: (s: number) => Vec2
  /**
   * Create a Vec2 from x, y, z values.
   *
   * API Type: Factory API.
   *
   * @param x value to set the x component to.
   * @param y value to set the y component to.
   * @returns a new Vec2 with the x and y components set to the specified values.
   */
  xy: (x: number, y: number) => Vec2
  /**
   * Create a Vec2 with all components set to zero.
   *
   * API Type: Factory API.
   *
   * @returns a new Vec2 with all components set to zero.
   */
  zero: () => Vec2
}

/// /////////////////// Unexported Internal Factories //////////////////////

const SCRATCH_VEC: Vec2[] = []

class Vec2Impl implements Vec2 {
  private _data: number[]

  public readonly x!: number

  public readonly y!: number

  constructor() {
    this._data = [0, 0, 1]
    Object.defineProperties(this, {
      _data: {enumerable: false},
      x: {enumerable: true, get: () => this._data[0]},
      y: {enumerable: true, get: () => this._data[1]},
    })
  }

  setXy(x: number, y: number): Vec2 {
    this._data[0] = x
    this._data[1] = y
    return this
  }

  setX(v: number): Vec2 {
    this._data[0] = v
    return this
  }

  setY(v: number): Vec2 {
    this._data[1] = v
    return this
  }

  setFrom(v: Vec2Source): Vec2 {
    return this.setXy(v.x, v.y)
  }

  clone(): Vec2 {
    return new Vec2Impl().setFrom(this)
  }

  dot(v: Vec2Source): number {
    return this.x * v.x + this.y * v.y
  }

  length(): number {
    return Math.sqrt(this.dot(this))
  }

  distanceTo(v: Vec2Source): number {
    return SCRATCH_VEC[0].setFrom(v).setMinus(this).length()
  }

  equals(v: Vec2Source, tolerance: number): boolean {
    return (
      Math.abs(this.x - v.x) <= tolerance &&
      Math.abs(this.y - v.y) <= tolerance
    )
  }

  setPlus(v: Vec2Source): Vec2 {
    return this.setXy(this.x + v.x, this.y + v.y)
  }

  plus(v: Vec2Source): Vec2 {
    return this.clone().setPlus(v)
  }

  setMinus(v: Vec2Source): Vec2 {
    return this.setXy(this.x - v.x, this.y - v.y)
  }

  minus(v: Vec2Source): Vec2 {
    return this.clone().setMinus(v)
  }

  setMix(v: Vec2Source, t: number): Vec2 {
    return this.setPlus(SCRATCH_VEC[0].setFrom(v).setMinus(this).setScale(t))
  }

  mix(v: Vec2Source, t: number): Vec2 {
    return this.clone().setMix(v, t)
  }

  setTimes(v: Vec2Source): Vec2 {
    return this.setXy(this.x * v.x, this.y * v.y)
  }

  times(v: Vec2Source): Vec2 {
    return this.clone().setTimes(v)
  }

  setDivide(v: Vec2Source): Vec2 {
    return this.setXy(this.x / v.x, this.y / v.y)
  }

  divide(v: Vec2Source): Vec2 {
    return this.clone().setDivide(v)
  }

  setScale(s: number): Vec2 {
    return this.setXy(this.x * s, this.y * s)
  }

  scale(s: number): Vec2 {
    return this.clone().setScale(s)
  }

  setNormalize(): Vec2 {
    return this.setScale(1 / (this.length() || 1))
  }

  normalize(): Vec2 {
    return this.clone().setNormalize()
  }

  cross(v: Vec2Source): number {
    return this.x * v.y - this.y * v.x
  }

  makeOne(): Vec2 {
    return this.setXy(1, 1)
  }

  makeScale(s: number): Vec2 {
    return this.setXy(s, s)
  }

  makeZero(): Vec2 {
    return this.setXy(0, 0)
  }
}

const newVec2 = (): Vec2 => new Vec2Impl()

/// /////////////////// Exported External Factories //////////////////////

const vec2: Vec2Factory = {
  from: (source: Vec2Source): Vec2 => newVec2().setXy(source.x, source.y),
  one: (): Vec2 => newVec2().makeOne(),
  scale: (s: number): Vec2 => newVec2().makeScale(s),
  xy: (x: number, y: number): Vec2 => newVec2().setXy(x, y),
  zero: (): Vec2 => newVec2().makeZero(),
}

SCRATCH_VEC.push(vec2.zero())

export type {
  Vec2,
  Vec2Factory,
  Vec2Source,
}

export {
  vec2,
}
