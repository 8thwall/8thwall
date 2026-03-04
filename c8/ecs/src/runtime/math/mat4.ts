import {fill4x4, rotation4x4ToQuat, unrolled4x4Mul4x4, sqlen3} from './algorithms'
import type {Quat, QuatSource} from './quat'
import {quat} from './quat'
import type {Vec3, Vec3Source} from './vec3'
import {vec3} from './vec3'

/// /////////////////// Type interfaces //////////////////////

/**
 * Interface representing a 4x4 matrix.  A 4x4 matrix is represented by a 16 dimensional array of
 * data, with elements stored in column major order. A special kind of matrix, known as a TRS matrix
 * (for Translation, Rotation, and Scale) is common in 3D geometry for representing the position,
 * orientation, and size of points in a 3D scene. Many special types of matrices have easily
 * specified inverses. By specifying these ahead of time, Mat4 allows for matrix inverse to be a
 * very fast O(1) operation. `Mat4` objects are created with the `ecs.math.mat4` `Mat4Factory`, or
 * through operations on other `Mat4` objects.
 */
interface Mat4 {
  /// //////////////////////////////// Immutable API ////////////////////////////////
  /**
   * Create a new matrix with the same components as this matrix.
   *
   * API Type: Immutable API.
   *
   * @returns a new matrix with the same components as this matrix.
   */
  clone: () => Mat4
  /**
   * Get the raw data of the matrix, in column-major order.
   *
   * API Type: Immutable API.
   *
   * @returns the 16-element raw data of the matrix, in column-major order.
   */
  data: () => number[]
  /**
   * Decompose the matrix into its translation, rotation, and scale components, assuming it was
   * formed by a translation, rotation, and scale in that order. If `target` is supplied, the result
   * will be stored in `target` and `target` will be returned. Otherwise, a new {t, r, s} object
   * will be created and returned.
   *
   * If you don't need the rotation in quaternion form, decomposeT and decomposeS are more efficient
   * ways to get just the translation or scale.
   *
   * API Type: Immutable API.
   *
   * @param target optional target object to store the result in.
   * @returns target if supplied, otherwise a new {t, r, s} object.
   */
  decomposeTrs: (target?: {t: Vec3, r: Quat, s: Vec3}) => {t: Vec3, r: Quat, s: Vec3}
  /**
   * Get the rotation component of the TRS matrix
   *
   * @param target optional target object to store the result in.
   * @returns target if supplied, otherwise a new quaternion.
   */
  decomposeR: (target?: Quat) => Quat
  /**
   * Get the translation component of the TRS matrix.
   *
   * @param target optional target object to store the result in.
   * @returns target if supplied, otherwise a new vec3.
   */
  decomposeT: (target?: Vec3) => Vec3
  /**
   * Get the scale component of the TRS matrix.
   *
   * @param target optional target object to store the result in.
   * @returns target if supplied, otherwise a new vec3.
   */
  decomposeS: (target?: Vec3) => Vec3
  /**
   * Compute the determinant of the matrix.
   *
   * API Type: Immutable API.
   *
   * @returns the determinant of the matrix.
   */
  determinant: () => number
  /**
   * Check whether two matrices are equal, with a specified floating point tolerance.
   *
   * API Type: Immutable API.
   *
   * @param m matrix to compare to.
   * @param tolerance used to judge near equality.
   * @returns true if all matrix elements are equal within the specified tolerance, false otherwise.
   */
  equals: (m: Mat4, tolerance: number) => boolean
  /**
   * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
   * inverse, this operation is very fast.
   *
   * API Type: Immutable API.
   *
   * @returns the inverse of the matrix.
   */
  inv: () => Mat4
  /**
   * Get the raw data of the inverse matrix, in column-major order, or null if the matrix is not
   * invertible.
   *
   * API Type: Immutable API.
   *
   * @returns the 16-element raw data of the inverse matrix, in column-major order, or null if the
   *   matrix is not invertible.
   */
  inverseData: () => number[] | null
  /**
   * Get a matrix with the same position and scale as this matrix, but with the rotation set to look
   * at the target.
   *
   * API Type: Immutable API.
   *
   * @param target vector where the target is located.
   * @param up vector representing the up direction from the mat4's perspective.
   * @returns a new matrix with the same position and scale as this matrix, but with the rotation
   *   set to look at the target.
   */
  lookAt: (target: Vec3Source, up: Vec3Source) => Mat4
  /**
   * Multiply the matrix by a scalar. Scaling by 0 throws an error.
   *
   * API Type: Immutable API.
   *
   * @param s scalar to multiply the matrix by.
   * @returns the matrix multiplied by the scalar.
   */
  scale: (s: number) => Mat4
  /**
   * Get the transpose of the matrix.
   *
   * API Type: Immutable API.
   *
   * @returns the transpose of the matrix.
   */
  transpose: () => Mat4
  /**
   * Multiply the matrix by another matrix.
   *
   * API Type: Immutable API.
   *
   * @param m matrix to multiply by.
   * @returns the matrix multiplied by another matrix.
   */
  times: (m: Mat4) => Mat4
  /**
   * Multiply the matrix by a vector using homogeneous coordinates.
   *
   * API Type: Immutable API.
   *
   * @param v vector to multiply by.
   * @param target optional target to store the result in.
   * @returns the transformed vector.
   */
  timesVec: (v: Vec3Source, target?: Vec3) => Vec3
  /// //////////////////////////////// Mutable API ////////////////////////////////
  /**
   * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
   * inverse, this operation is very fast. Store the result in this Mat4 and return this Mat4 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this matrix for chaining.
   */
  setInv: () => Mat4
  /**
   * Set the matrix rotation to look at the target, keeping translation and scale unchanged. Store
   * the result in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param target vector where the target is located.
   * @param up vector representing the up direction from the mat4's perspective.
   * @returns this matrix for chaining.
   */
  setLookAt: (target: Vec3Source, up: Vec3Source) => Mat4
  /**
   * Set the matrix to the result of m times this matrix. Store the result in this Mat4 and return
   * this Mat4 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param m matrix to premultiply by.
   * @returns this matrix for chaining.
   */
  setPremultiply: (m: Mat4) => Mat4
  /**
   * Multiply each element of the matrix by a scaler. Scaling by 0 throws an error. Store the result
   * in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Mutable API.
   *
   * @param s scalar to multiply the matrix by.
   * @returns this matrix for chaining.
   */
  setScale: (s: number) => Mat4
  /**
   * Multiply the matrix by another matrix. Store the result in this Mat4 and return this Mat4 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @param m matrix to multiply by.
   * @returns this matrix for chaining.
   */
  setTimes: (m: Mat4) => Mat4
  /**
   * Set the matrix to its transpose. Store the result in this Mat4 and return this Mat4 for
   * chaining.
   *
   * API Type: Mutable API.
   *
   * @returns this matrix for chaining.
   */
  setTranspose: () => Mat4
  /// //////////////////////////////// Set API ////////////////////////////////
  /**
   * Set the matrix to the identity matrix. Store the result in this Mat4 and return this Mat4 for
   * chaining.
   *
   * API Type: Set API.
   *
   * @returns this matrix for chaining.
   */
  makeI: () => Mat4
  /**
   * Set this matrix to a rotation matrix from the specified quaternion. Store the result in this
   * Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param r quaternion representing the desired rotation matrix.
   * @returns this matrix for chaining.
   */
  makeR: (r: QuatSource) => Mat4
  /**
   * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
   * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
   * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
   * calling inv() will throw an error.
   *
   * API Type: Set API.
   *
   * @param rowData for the matrix, 4 arrays of 4 elements each.
   * @param inverseRowData optional inverse row data for the matrix, 4 arrays of 4 elements each.
   * @returns this matrix for chaining.
   */
  makeRows: (rowData: number[][], inverseRowData?: number[][]) => Mat4
  /**
   * Set this matrix to a scale matrix from the specified vector. No element of the vector should be
   * zero. Store the result in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param s vector representing the desired scale in each of the x, y, and z dimensions.
   * @returns this matrix for chaining.
   */
  makeS: (s: Vec3Source) => Mat4
  /**
   * Set this matrix to a translation matrix from the specified vector. Store the result in this
   * Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param t vector representing the desired translation in each of the x, y, and z dimensions.
   * @returns this matrix for chaining.
   */
  makeT: (t: Vec3Source) => Mat4
  /**
   * Set this matrix to a translation and rotation matrix from the specified vector and quaternion.
   * Store the result in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param t vector representing the desired translation in each of the x, y, and z dimensions.
   * @param r quaternion representing the desired rotation matrix.
   * @returns this matrix for chaining.
   */
  makeTr: (t: Vec3Source, r: QuatSource) => Mat4
  /**
   * Set this matrix to a translation, rotation, and scale matrix from the specified vectors and
   * quaternion. Store the result in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param t vector representing the desired translation in each of the x, y, and z dimensions.
   * @param r quaternion representing the desired rotation matrix.
   * @param s vector representing the desired scale in each of the x, y, and z dimensions.
   * @returns this matrix for chaining.
   */
  makeTrs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4
  /**
   * Set the value of the matrix and inverse to the provided values. If no inverse is provided, one
   * will be computed if possible. If the matrix is not invertible, calling inv() will throw an
   * error. Store the result in this Mat4 and return this Mat4 for chaining.
   *
   * API Type: Set API.
   *
   * @param data for the matrix, 16 elements in column-major order.
   * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
   * @returns this matrix for chaining.
   */
  set: (data: number[], inverseData?: number[]) => Mat4
}

/**
 * Factory for Mat4. Mat4 objects are created with the `ecs.math.mat4` Mat4Factory.
 */
interface Mat4Factory {
  /**
   * Identity matrix.
   *
   * API Type: Factory API.
   *
   * @returns the identity matrix.
   */
  i: () => Mat4
  /**
   * Create the matrix with directly specified data, in column major order. An optional inverse can
   * be specified. If the inverse is not specified, it will be computed if the matrix is invertible.
   * If the matrix is not invertible, calling inv() will throw an error.
   *
   * API Type: Factory API.
   *
   * @param data for the matrix, 16 elements in column-major order.
   * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
   * @returns the matrix with the specified data.
   */
  of: (data: number[], inverseData?: number[]) => Mat4
  /**
   * Create a rotation matrix from a quaternion.
   *
   * API Type: Factory API.
   *
   * @param q quaternion representing the rotation.
   * @returns the rotation matrix.
   */
  r: (q: QuatSource) => Mat4
  /**
   * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
   * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
   * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
   * calling inv() will throw an error.
   *
   * API Type: Factory API.
   *
   * @param dataRows for the matrix, 4 arrays of 4 elements each.
   * @param inverseDataRows optional inverse row data for the matrix, 4 arrays of 4 elements each.
   * @returns the matrix with the specified row data.
   */
  rows: (dataRows: number[][], inverseDataRows?: number[][]) => Mat4
  /**
   * Create a scale matrix. No scale element should be zero.
   *
   * API Type: Factory API.
   *
   * @param v vector representing the scale in each of the x, y, and z dimensions.
   * @returns the scale matrix.
   */
  s: (v: Vec3Source) => Mat4
  /**
   * Create a translation matrix.
   *
   * API Type: Factory API.
   *
   * @param v vector representing the translation in each of the x, y, and z dimensions.
   * @returns the translation matrix.
   */
  t: (v: Vec3Source) => Mat4
  /**
   * Create a translation and rotation matrix.
   *
   * API Type: Factory API.
   *
   * @param t vector representing the translation in each of the x, y, and z dimensions.
   * @param r quaternion representing the rotation matrix.
   * @returns the translation and rotation matrix.
   */
  tr: (t: Vec3Source, r: QuatSource) => Mat4
  /**
   * Create a translation, rotation, and scale matrix.
   *
   * API Type: Factory API.
   *
   * @param t vector representing the translation in each of the x, y, and z dimensions.
   * @param r quaternion representing the rotation matrix.
   * @param s vector representing the scale in each of the x, y, and z dimensions.
   * @returns the translation, rotation, and scale matrix.
   */
  trs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4
}

/// /////////////////// Unexported Internal Factories //////////////////////

const SCRATCH_TRS = {
  t: vec3.zero(),
  r: quat.zero(),
  s: vec3.zero(),
}
const SCRATCH_MAT: Mat4[] = []

const SCRATCH_4X4: number[] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

const unrolled4x4Mul4x1 = (A: number[], B: number[], Outs: number[]) => {
  Outs[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3]
  Outs[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3]
  Outs[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3]
  Outs[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3]
}

const det2 = (m00: number, m01: number, m10: number, m11: number) => (m00 * m11 - m01 * m10)

const det3 = (
  m00: number,
  m01: number,
  m02: number,
  m10: number,
  m11: number,
  m12: number,
  m20: number,
  m21: number,
  m22: number
): number => (
  m00 * det2(m11, m12, m21, m22) -    // Col 0
    m01 * det2(m10, m12, m20, m22) +  // Col 1
    m02 * det2(m10, m11, m20, m21)    // Col 2
)

const invert4x4 = (m: number[], inv: number[]): boolean => {
  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] -
           m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
           m[13] * m[6] * m[11] - m[13] * m[7] * m[10]

  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
           m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
           m[12] * m[6] * m[11] + m[12] * m[7] * m[10]

  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] -
           m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
           m[12] * m[5] * m[11] - m[12] * m[7] * m[9]

  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] + m[12] * m[6] * m[9]

  const det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12]
  if (det === 0) {
    inv.fill(0)
    return false
  }

  const idet = 1.0 / det
  inv[0] *= idet
  inv[4] *= idet
  inv[8] *= idet
  inv[12] *= idet

  inv[1] = (-m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
             m[13] * m[2] * m[11] + m[13] * m[3] * m[10]) * idet

  inv[5] = (m[0] * m[10] * m[15] - m[0] * m[11] * m[14] -
            m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
            m[12] * m[2] * m[11] - m[12] * m[3] * m[10]) * idet

  inv[9] = (-m[0] * m[9] * m[15] + m[0] * m[11] * m[13] +
             m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
             m[12] * m[1] * m[11] + m[12] * m[3] * m[9]) * idet

  inv[13] = (m[0] * m[9] * m[14] - m[0] * m[10] * m[13] -
             m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
             m[12] * m[1] * m[10] - m[12] * m[2] * m[9]) * idet

  inv[2] = (m[1] * m[6] * m[15] - m[1] * m[7] * m[14] -
            m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
            m[13] * m[2] * m[7] - m[13] * m[3] * m[6]) * idet

  inv[6] = (-m[0] * m[6] * m[15] + m[0] * m[7] * m[14] +
             m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
             m[12] * m[2] * m[7] + m[12] * m[3] * m[6]) * idet

  inv[10] = (m[0] * m[5] * m[15] - m[0] * m[7] * m[13] -
             m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
             m[12] * m[1] * m[7] - m[12] * m[3] * m[5]) * idet

  inv[14] = (-m[0] * m[5] * m[14] + m[0] * m[6] * m[13] +
              m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
              m[12] * m[1] * m[6] + m[12] * m[2] * m[5]) * idet

  inv[3] = (-m[1] * m[6] * m[11] + m[1] * m[7] * m[10] +
             m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] + m[9] * m[3] * m[6]) * idet

  inv[7] = (m[0] * m[6] * m[11] - m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] - m[8] * m[3] * m[6]) * idet

  inv[11] = (-m[0] * m[5] * m[11] + m[0] * m[7] * m[9] +
              m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] + m[8] * m[3] * m[5]) * idet

  inv[15] = (m[0] * m[5] * m[10] - m[0] * m[6] * m[9] -
             m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
             m[8] * m[1] * m[6] - m[8] * m[2] * m[5]) * idet

  return true
}

const computedInverse = (m: number[]): number[] | null => {
  const inv = new Array(16)
  if (!invert4x4(m, inv)) {
    return null
  }
  return inv
}

const transpose4x4 = (m: number[], o: number[]): void => {
  [o[0], o[4], o[8], o[12],
    o[1], o[5], o[9], o[13],
    o[2], o[6], o[10], o[14],
    o[3], o[7], o[11], o[15]] = m
}

// Identity Matrix
const EYE = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
const SCRATCH_MAT1 = new Array(16)
const SCRATCH_MAT2 = new Array(16)
const SCRATCH_VEC1 = new Array(4)
const SCRATCH_VEC2 = new Array(4)

const copyMat4x4 = (src: number[], dst: number[]): void => {
  for (let i = 0; i < 16; i++) {
    dst[i] = src[i]
  }
}

const newMat4 = (): Mat4 => {
  let data_ = [...EYE]
  let inverseData_: number[] | null = [...EYE]

  const set = (data: number[], inverseData: number[] | null = null): Mat4 => {
    copyMat4x4(data, data_)
    const cinv = inverseData || computedInverse(data)
    if (cinv) {
      if (!inverseData_) {
        inverseData_ = new Array(16)
      }
      copyMat4x4(cinv, inverseData_)
    } else {
      inverseData_ = null
    }

    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const clone = (): Mat4 => newMat4().set(data_, inverseData_ || undefined)

  const setInv = (): Mat4 => {
    if (!inverseData_) {
      throw new Error('Mat4 is not invertible')
    }
    const temp = data_
    data_ = inverseData_
    inverseData_ = temp
    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const inv = (): Mat4 => clone().setInv()

  const setTranspose = (): Mat4 => {
    transpose4x4(data_, SCRATCH_MAT1)
    if (!inverseData_) {
      return set(SCRATCH_MAT1, null)
    }
    transpose4x4(inverseData_, SCRATCH_MAT2)
    return set(SCRATCH_MAT1, SCRATCH_MAT2)
  }

  const transpose = (): Mat4 => clone().setTranspose()

  const setScale = (s: number): Mat4 => {
    if (s === 0) {
      throw new Error('Cannot scale matrix by zero')
    }
    for (let i = 0; i < 16; i++) {
      data_[i] *= s
    }
    if (inverseData_) {
      const is = 1 / s
      for (let i = 0; i < 16; i++) {
        inverseData_[i] *= is
      }
    }
    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const scale = (s: number): Mat4 => clone().setScale(s)

  const setTimes = (m: Mat4): Mat4 => {
    unrolled4x4Mul4x4(data_, m.data(), SCRATCH_MAT1)
    if (!(inverseData_ && m.inverseData())) {
      return set(SCRATCH_MAT1, null)
    }
    unrolled4x4Mul4x4(m.inverseData() as number[], inverseData_ as number[], SCRATCH_MAT2)
    return set(SCRATCH_MAT1, SCRATCH_MAT2)
  }

  const setPremultiply = (m: Mat4): Mat4 => {
    unrolled4x4Mul4x4(m.data(), data_, SCRATCH_MAT1)
    if (!(inverseData_ && m.inverseData())) {
      return set(SCRATCH_MAT1, null)
    }
    unrolled4x4Mul4x4(inverseData_ as number[], m.inverseData() as number[], SCRATCH_MAT2)
    return set(SCRATCH_MAT1, SCRATCH_MAT2)
  }

  const times = (m: Mat4): Mat4 => clone().setTimes(m)

  const timesVec = (v: Vec3Source, target: Vec3 = vec3.zero()): Vec3 => {
    const o = SCRATCH_VEC1
    SCRATCH_VEC2[0] = v.x
    SCRATCH_VEC2[1] = v.y
    SCRATCH_VEC2[2] = v.z
    SCRATCH_VEC2[3] = 1
    unrolled4x4Mul4x1(data_, SCRATCH_VEC2, o)
    return target.setXyz(o[0] / o[3], o[1] / o[3], o[2] / o[3])
  }

  const determinant = (): number => {
    const m = data_
    const m0 = m[0]
    const m1 = m[4]
    const m2 = m[8]
    const m3 = m[12]
    return m0 * det3(m[5], m[9], m[13], m[6], m[10], m[14], m[7], m[11], m[15]) -
      m1 * det3(m[1], m[9], m[13], m[2], m[10], m[14], m[3], m[11], m[15]) +
      m2 * det3(m[1], m[5], m[13], m[2], m[6], m[14], m[3], m[7], m[15]) -
      m3 * det3(m[1], m[5], m[9], m[2], m[6], m[10], m[3], m[7], m[11])
  }

  const decomposeT = (target: Vec3 = vec3.zero()): Vec3 => (
    target.setXyz(data_[12], data_[13], data_[14])
  )

  const decomposeS = (target: Vec3 = vec3.zero()): Vec3 => {
    const m = data_
    const sx = Math.sqrt(sqlen3(m[0], m[1], m[2]))
    const sy = Math.sqrt(sqlen3(m[4], m[5], m[6]))
    const sz = Math.sqrt(sqlen3(m[8], m[9], m[10]))

    // Detect a scale inversion, and negate the scale of the x-axis arbitrarily.
    const detSign = det3(
      m[0], m[4], m[8],
      m[1], m[5], m[9],
      m[2], m[6], m[10]
    ) < 0
      ? -1
      : 1

    return target.setXyz(sx * detSign, sy, sz)
  }

  const decomposeRs = (r: Quat, s: Vec3) => {
    // First compute the scale.
    decomposeS(s)

    const isx = 1 / s.x
    const isy = 1 / s.y
    const isz = 1 / s.z

    // Normalize the matrix to remove scale.
    unrolled4x4Mul4x4(
      data_, fill4x4(SCRATCH_4X4, isx, 0, 0, 0, 0, isy, 0, 0, 0, 0, isz, 0, 0, 0, 0, 1),
      SCRATCH_MAT1
    )

    rotation4x4ToQuat(SCRATCH_MAT1, SCRATCH_VEC1)
    r.setXyzw(SCRATCH_VEC1[0], SCRATCH_VEC1[1], SCRATCH_VEC1[2], SCRATCH_VEC1[3])
  }

  const decomposeR = (target = quat.zero()): Quat => {
    decomposeRs(target, SCRATCH_TRS.s)
    return target
  }

  const decomposeTrs = (
    target = {t: vec3.zero(), r: quat.zero(), s: vec3.zero()}
  ): {t: Vec3, r: Quat, s: Vec3} => {
    decomposeRs(target.r, target.s)
    decomposeT(target.t)
    return target
  }

  const equals = (m: Mat4, tolerance: number): boolean => (
    !data_.some((v, i) => Math.abs(v - m.data()[i]) > tolerance)
  )

  const makeI = (): Mat4 => set(EYE, EYE)

  const makeR = (q: QuatSource): Mat4 => {
    const wx = q.w * q.x
    const wy = q.w * q.y
    const wz = q.w * q.z
    const xx = q.x * q.x
    const xy = q.x * q.y
    const xz = q.x * q.z
    const yy = q.y * q.y
    const yz = q.y * q.z
    const zz = q.z * q.z

    copyMat4x4(EYE, data_)
    data_[0] = 1.0 - 2.0 * (yy + zz)
    data_[4] = 2.0000000 * (xy - wz)
    data_[8] = 2.0000000 * (xz + wy)

    data_[1] = 2.0000000 * (xy + wz)
    data_[5] = 1.0 - 2.0 * (xx + zz)
    data_[9] = 2.0000000 * (yz - wx)

    data_[2] = 2.0000000 * (xz - wy)
    data_[6] = 2.0000000 * (yz + wx)
    data_[10] = 1.0 - 2.0 * (xx + yy)

    if (!inverseData_) {
      inverseData_ = new Array(16)
    }
    transpose4x4(data_, inverseData_)
    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const makeS = (v: Vec3Source): Mat4 => {
    copyMat4x4(EYE, data_)
    data_[0] = v.x
    data_[5] = v.y
    data_[10] = v.z

    if (!inverseData_) {
      inverseData_ = new Array(16)
    }
    copyMat4x4(EYE, inverseData_)
    inverseData_[0] = 1 / v.x
    inverseData_[5] = 1 / v.y
    inverseData_[10] = 1 / v.z

    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const makeT = (v: Vec3Source): Mat4 => {
    copyMat4x4(EYE, data_)
    data_[12] = v.x
    data_[13] = v.y
    data_[14] = v.z

    if (!inverseData_) {
      inverseData_ = new Array(16)
    }
    copyMat4x4(EYE, inverseData_)
    inverseData_[12] = -v.x
    inverseData_[13] = -v.y
    inverseData_[14] = -v.z

    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const makeTr = (t: Vec3Source, r: QuatSource): Mat4 => makeT(t).setTimes(SCRATCH_MAT[0].makeR(r))

  const makeTrs = (t: Vec3Source, r: QuatSource, s: Vec3Source): Mat4 => {
    const wx = r.w * r.x
    const wy = r.w * r.y
    const wz = r.w * r.z
    const xx = r.x * r.x
    const xy = r.x * r.y
    const xz = r.x * r.z
    const yy = r.y * r.y
    const yz = r.y * r.z
    const zz = r.z * r.z

    const r0 = 1 - 2 * yy - 2 * zz
    const r1 = 2 * xy - 2 * wz
    const r2 = 2 * xz + 2 * wy
    const r3 = 2 * xy + 2 * wz
    const r4 = 1 - 2 * xx - 2 * zz
    const r5 = 2 * yz - 2 * wx
    const r6 = 2 * xz - 2 * wy
    const r7 = 2 * yz + 2 * wx
    const r8 = 1 - 2 * xx - 2 * yy

    data_[0] = s.x * r0
    data_[4] = s.y * r1
    data_[8] = s.z * r2
    data_[12] = t.x
    data_[1] = s.x * r3
    data_[5] = s.y * r4
    data_[9] = s.z * r5
    data_[13] = t.y
    data_[2] = s.x * r6
    data_[6] = s.y * r7
    data_[10] = s.z * r8
    data_[14] = t.z
    data_[3] = 0
    data_[7] = 0
    data_[11] = 0
    data_[15] = 1

    if (!inverseData_) {
      inverseData_ = new Array(16)
    }
    // eslint-disable no-mixed-operators
    const invSx = 1 / s.x
    const invSy = 1 / s.y
    const invSz = 1 / s.z
    inverseData_[0] = invSx * r0
    inverseData_[1] = invSy * r1
    inverseData_[2] = invSz * r2
    inverseData_[3] = 0
    inverseData_[4] = invSx * r3
    inverseData_[5] = invSy * r4
    inverseData_[6] = invSz * r5
    inverseData_[7] = 0
    inverseData_[8] = invSx * r6
    inverseData_[9] = invSy * r7
    inverseData_[10] = invSz * r8
    inverseData_[11] = 0

    inverseData_[12] = -invSx * (t.x * r0 + t.y * r3 + t.z * r6)
    inverseData_[13] = -invSy * (t.x * r1 + t.y * r4 + t.z * r7)
    inverseData_[14] = -invSz * (t.x * r2 + t.y * r5 + t.z * r8)
    inverseData_[15] = 1
    // eslint-enable no-mixed-operators
    return out_
  }

  const makeRows = (rowData: number[][], inverseRowData?: number[][]): Mat4 => {
    [[data_[0], data_[4], data_[8], data_[12]],
      [data_[1], data_[5], data_[9], data_[13]],
      [data_[2], data_[6], data_[10], data_[14]],
      [data_[3], data_[7], data_[11], data_[15]]] = rowData
    if (inverseRowData) {
      if (!inverseData_) {
        inverseData_ = new Array(16)
      }
      [[inverseData_[0], inverseData_[4], inverseData_[8], inverseData_[12]],
        [inverseData_[1], inverseData_[5], inverseData_[9], inverseData_[13]],
        [inverseData_[2], inverseData_[6], inverseData_[10], inverseData_[14]],
        [inverseData_[3], inverseData_[7], inverseData_[11], inverseData_[15]]] = rowData
    } else {
      const cinv = computedInverse(data_)
      if (cinv) {
        if (!inverseData_) {
          inverseData_ = new Array(16)
        }
        copyMat4x4(cinv, inverseData_)
      } else {
        inverseData_ = null
      }
    }
    return out_  // eslint-disable-line @typescript-eslint/no-use-before-define
  }

  const setLookAt = (target: Vec3Source, up: Vec3Source): Mat4 => {
    const {t, r, s} = decomposeTrs(SCRATCH_TRS)
    r.makeLookAt(SCRATCH_TRS.t, target, up)

    return makeTrs(t, r, s)
  }

  const lookAt = (target: Vec3Source, up: Vec3Source): Mat4 => clone().setLookAt(target, up)

  const out_: Mat4 = {
    clone,
    data: () => data_,
    decomposeTrs,
    decomposeR,
    decomposeT,
    decomposeS,
    determinant,
    equals,
    inverseData: () => inverseData_,
    inv,
    lookAt,
    scale,
    times,
    timesVec,
    transpose,
    set,
    setInv,
    setLookAt,
    setPremultiply,
    setScale,
    setTimes,
    setTranspose,
    makeI,
    makeR,
    makeRows,
    makeS,
    makeT,
    makeTr,
    makeTrs,
  }

  return out_
}

/// /////////////////// Exported External Factories //////////////////////

const mat4: Mat4Factory = {
  i: (): Mat4 => newMat4().makeI(),
  of: (data: number[], inverseData?: number[]): Mat4 => (
    newMat4().set(data, inverseData)
  ),
  r: (q: QuatSource): Mat4 => newMat4().makeR(q),
  rows: (dataRows: number[][], inverseDataRows?: number[][]) => (
    newMat4().makeRows(dataRows, inverseDataRows)
  ),
  s: (v: Vec3Source): Mat4 => newMat4().makeS(v),
  t: (v: Vec3Source): Mat4 => newMat4().makeT(v),
  tr: (t: Vec3Source, r: QuatSource): Mat4 => newMat4().makeTr(t, r),
  trs: (t: Vec3Source, r: QuatSource, s: Vec3Source): Mat4 => newMat4().makeTrs(t, r, s),
  // TODO: Camera projection
}

SCRATCH_MAT.push(newMat4())

export type {
  Mat4,
  Mat4Factory,
}

export {
  mat4,
}
