interface Quaternion {
  x: number
  y: number
  z: number
  w: number
}

/**
 * Get a quaternion. Data comes from the `deviceorientation_event`:
 * - https://developer.mozilla.org/en-US/docs/Web/API/Window/deviceorientation_event
 * @param alpha A number representing the motion of the device around the z axis, express in degrees
 * with values ranging from 0 (inclusive) to 360 (exclusive).
 * @param beta A number representing the motion of the device around the x axis, expressed in
 * degrees with values ranging from -180 (inclusive) to 180 (exclusive). This represents the front
 * to back motion of the device.
 * @param gamma A number representing the motion of the device around the y axis, expressed in
 * degrees with values ranging from -90 (inclusive) to 90 (exclusive). This represents the left to
 * right motion of the device.
*/
const getQuaternion = (alpha: number, beta: number, gamma: number): Quaternion => {
  const degToRad = Math.PI / 180

  const _x = beta * degToRad || 0
  const _y = gamma * degToRad || 0
  const _z = alpha * degToRad || 0

  const cX = Math.cos(_x / 2)
  const cY = Math.cos(_y / 2)
  const cZ = Math.cos(_z / 2)
  const sX = Math.sin(_x / 2)
  const sY = Math.sin(_y / 2)
  const sZ = Math.sin(_z / 2)

  const w = cX * cY * cZ - sX * sY * sZ
  const x = sX * cY * cZ - cX * sY * sZ
  const y = cX * sY * cZ + sX * cY * sZ
  const z = cX * cY * sZ + sX * sY * cZ

  return {x, y, z, w}
}

export {Quaternion, getQuaternion}
