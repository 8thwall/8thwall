import {type Mat4, mat4} from '../ecs/src/runtime/math/math'
import {matrixToTransform} from './transform'
import type {XrPose} from './xrapi-types'

const matrixToPose = (matrix: Mat4): XrPose => ({
  transform: matrixToTransform(matrix),
  linearVelocity: {x: 0, y: 0, z: 0, w: 0},
  angularVelocity: {x: 0, y: 0, z: 0, w: 0},
})

const poseToMatrix = (pose: XrPose): Mat4 => mat4.of(Array.from(pose.transform.matrix))

export {
  matrixToPose,
  poseToMatrix,
}
