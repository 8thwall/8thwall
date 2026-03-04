// @attr(npm_rule = "@npm-rendering//:npm-rendering")
import {assert, describe, it} from '@nia/bzl/js/chai-js'

import {XrRigidTransformImpl} from '@nia/c8/xrapi/transform'

describe('XR Transform Tests', () => {
  it('test XrRigidTransform Creation', () => {
    const position = {x: 1, y: 2, z: 3, w: 1}
    const orientation = {x: 0, y: 0, z: 0, w: 1}
    const expectedMatrix = new Float32Array([  // column-major
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      1, 2, 3, 1,
    ])

    const transform = new XrRigidTransformImpl(position, orientation)

    assert.deepEqual(transform.position, position)
    assert.deepEqual(transform.orientation, orientation)
    assert.deepEqual(transform.matrix, expectedMatrix)
  })

  it('test XrRigidTransform Creation with Three types', () => {
    const position = {x: 1, y: 2, z: 3, w: 1}
    const orientation = {x: 0, y: 0, z: 0, w: 1}
    const expectedMatrix = new Float32Array([  // column-major
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      1, 2, 3, 1,
    ])

    const threeQuat = {isQuaternion: true, x: 0, y: 0, z: 0, w: 1}
    const threeVec = {isVector3: true, x: 1, y: 2, z: 3}

    const transform = new XrRigidTransformImpl(position, threeQuat)
    assert.deepEqual(transform.position, position)
    assert.deepEqual(transform.orientation, orientation)
    assert.deepEqual(transform.matrix, expectedMatrix)

    const transform2 = new XrRigidTransformImpl(threeVec as any)
    assert.deepEqual(transform2.position, position)
    assert.deepEqual(transform2.orientation, orientation)
    assert.deepEqual(transform2.matrix, new Float32Array([  // column-major
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      1, 2, 3, 1,
    ]))
  })

  it('test XrRigidTransform Inverse Creation', () => {
    const position = {x: 1, y: 2, z: 3, w: 1}
    const orientation = {x: 0, y: 0, z: 0, w: 1}
    const transform = new XrRigidTransformImpl(position, orientation)

    const {inverse} = transform

    const expectedPosition = {x: -1, y: -2, z: -3, w: 1}
    const expectedOrientation = {x: 0, y: 0, z: 0, w: 1}
    const expectedMatrix = new Float32Array([  // column-major
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      -1, -2, -3, 1,
    ])

    assert.deepEqual(inverse.position, expectedPosition)
    assert.deepEqual(inverse.orientation, expectedOrientation)
    assert.deepEqual(inverse.matrix, expectedMatrix)
  })

  it('test XrRigidTransform Inverse Inverse Equality', () => {
    const position = {x: 1, y: 2, z: 3, w: 1}
    const orientation = {x: 0, y: 0, z: 0, w: 1}
    const transform1 = new XrRigidTransformImpl(position, orientation)

    const {inverse: transform2} = transform1
    const {inverse: transform3} = transform2

    assert.deepEqual(transform1.position, transform3.position)
    assert.deepEqual(transform1.orientation, transform3.orientation)
    assert.deepEqual(transform1.matrix, transform3.matrix)
    assert.strictEqual(transform1, transform3)
  })

  it('test XrRigidTransform references', () => {
    const position = {x: 1, y: 2, z: 3, w: 1}
    const orientation = {x: 0, y: 0, z: 0, w: 1}
    const transform = new XrRigidTransformImpl(position, orientation)

    // Change original values' attributes
    position.x = 6
    position.y = 5
    position.z = 4
    position.w = 1

    orientation.x = 1
    orientation.y = 2
    orientation.z = 3
    orientation.w = 1

    assert.strictEqual(transform.position.x, 1)
    assert.strictEqual(transform.position.y, 2)
    assert.strictEqual(transform.position.z, 3)
    assert.strictEqual(transform.position.w, 1)

    assert.strictEqual(transform.orientation.x, 0)
    assert.strictEqual(transform.orientation.y, 0)
    assert.strictEqual(transform.orientation.z, 0)
    assert.strictEqual(transform.orientation.w, 1)
  })

  it('test XrRigidTransform lazy decompose', () => {
    const expectedMatrix = new Float32Array([  // column-major
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      1, 2, 3, 1,
    ])
    const transform = new XrRigidTransformImpl(undefined, undefined, true, expectedMatrix)

    assert.strictEqual(transform.position.x, 1)
    assert.strictEqual(transform.position.y, 2)
    assert.strictEqual(transform.position.z, 3)
    assert.strictEqual(transform.position.w, 1)

    assert.strictEqual(transform.orientation.x, 0)
    assert.strictEqual(transform.orientation.y, 0)
    assert.strictEqual(transform.orientation.z, 0)
    assert.strictEqual(transform.orientation.w, 1)
  })

  it('test XrRigidTransform fast inverse from matrix', () => {
    const expectedMatrix = new Float32Array([  // column-major
      0.866, 0.5, 0, 0,
      -0.5, 0.866, 0, 0,
      0, 0, 1, 0,
      2, 3, 1, 1,
    ])
    const transform = new XrRigidTransformImpl(undefined, undefined, true, expectedMatrix)
    const {inverse} = transform

    assert.isTrue(Math.abs(inverse.matrix[0] - 0.866) < 1e-4, 0)
    assert.isTrue(Math.abs(inverse.matrix[1] - -0.5) < 1e-4, 1)
    assert.isTrue(Math.abs(inverse.matrix[2] - 0) < 1e-4, 2)
    assert.isTrue(Math.abs(inverse.matrix[4] - 0.5) < 1e-4, 4)
    assert.isTrue(Math.abs(inverse.matrix[5] - 0.866) < 1e-4, 5)
    assert.isTrue(Math.abs(inverse.matrix[6] - 0) < 1e-4, 6)
    assert.isTrue(Math.abs(inverse.matrix[8] - 0) < 1e-4, 8)
    assert.isTrue(Math.abs(inverse.matrix[9] - 0) < 1e-4, 9)
    assert.isTrue(Math.abs(inverse.matrix[10] - 1) < 1e-4, 10)

    assert.isTrue(Math.abs(inverse.position.x - -3.231) < 1e-3, 'x')
    assert.isTrue(Math.abs(inverse.position.y - -1.598) < 1e-3, 'y')
    assert.isTrue(Math.abs(inverse.position.z - -1) < 1e-3, 'z')
    assert.isTrue(Math.abs(inverse.position.w - 1) < 1e-3, 'w')
  })
})
