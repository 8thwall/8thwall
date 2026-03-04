import {type Mat4, mat4, quat} from '@nia/c8/ecs/src/runtime/math/math'
import {unrolled4x4Mul4x4} from '@nia/c8/ecs/src/runtime/math/algorithms'

import type {DOMPointReadOnly, XrRigidTransform} from './xrapi-types'

const floatsToMatrix = (floats: number[]): Float32Array => {
  const mat = new Float32Array(16)
  for (let i = 0; i < 16; i++) {
    mat[i] = floats[i]
  }
  return mat
}

const normalizeQuat = (q: DOMPointReadOnly): DOMPointReadOnly => ({...quat.from(q).setNormalize()})

const isValidDOMPoint = (point: DOMPointReadOnly): boolean => (
  Number.isFinite(point.x) &&
      Number.isFinite(point.y) &&
      Number.isFinite(point.z) &&
      Number.isFinite(point.w)
)

const getRtInverse = (matrix: Float32Array): Float32Array => {
  const inv = new Float32Array(16)
  // inv of rotation = transpose of rotation
  // inv of translation = -translation * inv of rotation
  // eslint-disable prefer-destructuring
  inv[0] = matrix[0]
  inv[1] = matrix[4]
  inv[2] = matrix[8]
  inv[3] = 0
  inv[4] = matrix[1]
  inv[5] = matrix[5]
  inv[6] = matrix[9]
  inv[7] = 0
  inv[8] = matrix[2]
  inv[9] = matrix[6]
  inv[10] = matrix[10]
  inv[11] = 0
  inv[12] = -matrix[12] * matrix[0] - matrix[13] * matrix[1] - matrix[14] * matrix[2]
  inv[13] = -matrix[12] * matrix[4] - matrix[13] * matrix[5] - matrix[14] * matrix[6]
  inv[14] = -matrix[12] * matrix[8] - matrix[13] * matrix[9] - matrix[14] * matrix[10]
  inv[15] = 1
  // eslint-enable prefer-destructuring
  return inv
}

class XrRigidTransformImpl implements XrRigidTransform {
  private _position: DOMPointReadOnly

  private _orientation: DOMPointReadOnly

  private _matrix: Float32Array | null = null;

  private _inverse: XrRigidTransform | null = null;

  // When true, _matrix is set but position/orientation are not
  private _needsDecompose = false;

  private _decomposeMatrix(): void {
    const {t, r} = mat4.of(Array.from(this._matrix!)).decomposeTrs()
    this._position = {...t, w: 1}
    this._orientation = r
    this._needsDecompose = false
  }

  get position(): DOMPointReadOnly {
    if (this._needsDecompose) {
      this._decomposeMatrix()
    }
    return this._position
  }

  get orientation(): DOMPointReadOnly {
    if (this._needsDecompose) {
      this._decomposeMatrix()
    }
    return this._orientation
  }

  // https://immersive-web.github.io/webxr/#dom-xrrigidtransform-xrrigidtransform
  constructor(
    position?: DOMPointReadOnly, orientation?: DOMPointReadOnly, alreadyNormalized = false,
    matrix?: Float32Array
  ) {
    if (matrix) {
      this._matrix = matrix
      this._needsDecompose = true
      return
    }

    this._position = {
      x: position?.x ?? 0,
      y: position?.y ?? 0,
      z: position?.z ?? 0,
      w: position?.w ?? 1,
    }
    const orientationWithDefaults = {
      x: orientation?.x ?? 0,
      y: orientation?.y ?? 0,
      z: orientation?.z ?? 0,
      w: orientation?.w ?? 1,
    }
    this._orientation = alreadyNormalized
      ? orientationWithDefaults
      : normalizeQuat(orientationWithDefaults)
    this._needsDecompose = false

    if (this._position.w !== 1) {
      throw new TypeError('[xr-api] XrRigidTransform: w component of position must be 1')
    }

    if (!isValidDOMPoint(this._position) || !isValidDOMPoint(this._orientation)) {
      throw new TypeError('[xr-api] XrRigidTransform: position must be a valid DOMPointReadOnly')
    }
  }

  private _composeMatrix() {
    // Directly compose into two matrices
    const r = this.orientation
    const wx = r.w * r.x
    const wy = r.w * r.y
    const wz = r.w * r.z
    const xx = r.x * r.x
    const xy = r.x * r.y
    const xz = r.x * r.z
    const yy = r.y * r.y
    const yz = r.y * r.z
    const zz = r.z * r.z

    const mat = new Float32Array(16)
    const invMat = new Float32Array(16)
    // eslint-disable no-multi-assign
    mat[0] = invMat[0] = 1.0 - 2.0 * (yy + zz)
    mat[4] = invMat[1] = 2.0000000 * (xy - wz)
    mat[8] = invMat[2] = 2.0000000 * (xz + wy)
    mat[12] = this.position.x

    mat[1] = invMat[4] = 2.0000000 * (xy + wz)
    mat[5] = invMat[5] = 1.0 - 2.0 * (xx + zz)
    mat[9] = invMat[6] = 2.0000000 * (yz - wx)
    mat[13] = this.position.y

    mat[2] = invMat[8] = 2.0000000 * (xz - wy)
    mat[6] = invMat[9] = 2.0000000 * (yz + wx)
    mat[10] = invMat[10] = 1.0 - 2.0 * (xx + yy)
    mat[14] = this.position.z

    invMat[12] = -mat[12] * mat[0] - mat[13] * mat[1] - mat[14] * mat[2]
    invMat[13] = -mat[12] * mat[4] - mat[13] * mat[5] - mat[14] * mat[6]
    invMat[14] = -mat[12] * mat[8] - mat[13] * mat[9] - mat[14] * mat[10]

    mat[3] = invMat[3] = 0
    mat[7] = invMat[7] = 0
    mat[11] = invMat[11] = 0
    mat[15] = invMat[15] = 1
    // eslint-enable no-multi-assign

    this._matrix = mat
    this._inverse = new XrRigidTransformImpl(undefined, undefined, true, invMat)
    // @ts-ignore
    this._inverse._inverse = this
  }

  // https://immersive-web.github.io/webxr/#xrrigidtransform-obtain-the-matrix
  get matrix(): Float32Array {
    if (!this._matrix) {
      this._composeMatrix()
    }
    return this._matrix!
  }

  // https://immersive-web.github.io/webxr/#dom-xrrigidtransform-inverse
  get inverse(): XrRigidTransform {
    if (!this._inverse) {
      if (this._matrix) {
        // This is only correct when there is no scale in these matrices
        // See mat4.makeTrs for a version that works with scale
        this._inverse = new XrRigidTransformImpl(undefined, undefined, true,
          getRtInverse(this._matrix))
        // @ts-ignore
        this._inverse._inverse = this
      } else {
        this._composeMatrix()
      }
    }
    return this._inverse!
  }
}

const matrixToTransform = (matrix: Mat4): XrRigidTransform => {
  const {t, r} = matrix.decomposeTrs()
  return new XrRigidTransformImpl({...t, w: 1}, r)
}

const floatsToTransform = (floats: number[]): XrRigidTransform => (
  new XrRigidTransformImpl(undefined, undefined, true, floatsToMatrix(floats))
)

// https://immersive-web.github.io/webxr/#multiply-transforms
const multiplyTransform = (a: XrRigidTransform, b: XrRigidTransform): XrRigidTransform => {
  const mat = new Float32Array(16)
  unrolled4x4Mul4x4(a.matrix, b.matrix, mat)
  return new XrRigidTransformImpl(undefined, undefined, true, mat)
}

export {
  XrRigidTransformImpl,
  floatsToMatrix,
  floatsToTransform,
  matrixToTransform,
  multiplyTransform,
}
