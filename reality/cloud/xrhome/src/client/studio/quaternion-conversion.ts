import {degreesToRadians, radiansToDegrees} from '@ecs/shared/angle-conversion'
import {Euler, Quaternion, Vector3} from 'three'
import type {DeepReadonly} from 'ts-essentials'

type QuaternionArray = DeepReadonly<[number, number, number, number]>

const EULER_ORDER = 'YXZ'

const tempQuaternion = new Quaternion()
const tempEuler = new Euler(0, 0, 0, EULER_ORDER)

const normalizeDegrees = (degrees: number) => ((((degrees + 180) % 360) + 360) % 360) - 180

const eulerDegreesToQuaternion = (
  x: number, y: number, z: number
): [number, number, number, number] => {
  tempEuler.set(degreesToRadians(x), degreesToRadians(y), degreesToRadians(z), EULER_ORDER)
  tempQuaternion.setFromEuler(tempEuler)
  return [tempQuaternion.x, tempQuaternion.y, tempQuaternion.z, tempQuaternion.w]
}

const quaternionToEulerDegrees = (
  quaternion: readonly [number, number, number, number]
): {x: number, y: number, z: number} => {
  tempQuaternion.fromArray(quaternion)
  tempEuler.setFromQuaternion(tempQuaternion)
  return {
    x: Math.round(radiansToDegrees(tempEuler.x)),
    y: Math.round(radiansToDegrees(tempEuler.y)),
    z: Math.round(radiansToDegrees(tempEuler.z)),
  }
}

const getRotationQuaternion = (
  angle: number, axisToRotate: Vector3, currentQuaternion: Quaternion
) => {
  tempEuler.setFromQuaternion(currentQuaternion, EULER_ORDER)
  const currentAxis = axisToRotate.clone().applyEuler(tempEuler)
  const rotationQuaternion = new Quaternion()
    .setFromAxisAngle(currentAxis, degreesToRadians(angle))
  return rotationQuaternion
}

const handleRotationDrag = (
  currentRotations: QuaternionArray[],
  onChange: (newValues: QuaternionArray[]) => void,
  dX: number,
  dY: number,
  dZ: number
) => {
  const xAxis = new Vector3(1, 0, 0)
  const yAxis = new Vector3(0, 1, 0)
  const zAxis = new Vector3(0, 0, 1)

  const newRotations = currentRotations.map((currentRotation) => {
    const currentQuaternion = new Quaternion().fromArray(currentRotation)
    const rotateX = getRotationQuaternion(dX, xAxis, currentQuaternion)
    const rotateY = getRotationQuaternion(dY, yAxis, currentQuaternion)
    const rotateZ = getRotationQuaternion(dZ, zAxis, currentQuaternion)
    tempQuaternion.fromArray(currentRotation)
    tempQuaternion.premultiply(rotateZ).premultiply(rotateX).premultiply(rotateY)

    return ([
      tempQuaternion.x, tempQuaternion.y, tempQuaternion.z, tempQuaternion.w,
    ] as const)
  })

  onChange(newRotations)
}

export {
  eulerDegreesToQuaternion,
  quaternionToEulerDegrees,
  getRotationQuaternion,
  handleRotationDrag,
  normalizeDegrees,
  EULER_ORDER,
}

export type {
  QuaternionArray,
}
