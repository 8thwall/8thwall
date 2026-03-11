import {useEphemeralEditState} from './ephemeral-edit-state'

import {
  eulerDegreesToQuaternion,
  quaternionToEulerDegrees,
  handleRotationDrag,
  type QuaternionArray,
} from '../quaternion-conversion'

const useEulerEdit = (
  currentRotations: QuaternionArray[],
  onChange: (newValues: QuaternionArray[]) => void
) => {
  const {editValue, setEditValue} = useEphemeralEditState({
    value: currentRotations,
    deriveEditValue: (quaternions): {x: number, y: number, z: number} => {
      const eulerValues = quaternions.map(quaternionToEulerDegrees)

      const isAllEqual = eulerValues.length < 2 ||
        eulerValues.every(v => v.x === eulerValues[0].x && v.y === eulerValues[0].y &&
        v.z === eulerValues[0].z)

      if (isAllEqual) {
        return eulerValues[0]
      } else {
        return {x: 0, y: 0, z: 0}
      }
    },
    parseEditValue: (v: {x: number, y: number, z: number}) => {
      if (currentRotations.length === 0) {
        return [false]
      }
      const newQuaternionValues: [number, number, number, number][] = []
      currentRotations.forEach(() => {
        const quaternion = eulerDegreesToQuaternion(v.x, v.y, v.z)
        newQuaternionValues.push(quaternion)
      })
      return [
        true,
        newQuaternionValues,
      ] as const
    },
    compareValue: (a: QuaternionArray[] | null, b: QuaternionArray[] |null) => {
      if (!a || !b || a.length === 0 || b.length === 0) {
        return false
      }
      return a?.every((quaternion, i) => quaternion.every((v, j) => v === b[i][j]) ||
        quaternion.every((v, j) => v === -b[i][j]))
    },
    onChange,
  })

  const handleChange = (latestX: number, latestY: number, latestZ: number) => {
    setEditValue({x: latestX, y: latestY, z: latestZ})
  }

  const {x, y, z} = editValue

  return {
    x,
    y,
    z,
    setX: (newX: number) => handleChange(newX, y, z),
    setY: (newY: number) => handleChange(x, newY, z),
    setZ: (newZ: number) => handleChange(x, y, newZ),
    setDragX: (newDX: number) => handleRotationDrag(currentRotations, onChange, newDX, 0, 0),
    setDragY: (newDY: number) => handleRotationDrag(currentRotations, onChange, 0, newDY, 0),
    setDragZ: (newDZ: number) => handleRotationDrag(currentRotations, onChange, 0, 0, newDZ),
  }
}

export {
  useEulerEdit,
}
