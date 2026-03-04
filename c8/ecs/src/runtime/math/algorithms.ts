// @visibility(//visibility:public)

const fill4x1 = (
  v: [number, number, number, number], a: number, b: number, c: number, d: number
): [number, number, number, number] => {
  v[0] = a
  v[1] = b
  v[2] = c
  v[3] = d
  return v
}

const fill4x4 = (
  v: number[], a: number, b: number, c: number, d: number,
  e: number, f: number, g: number, h: number,
  i: number, j: number, k: number, l: number,
  m: number, n: number, o: number, p: number
): number[] => {
  v[0] = a
  v[1] = b
  v[2] = c
  v[3] = d
  v[4] = e
  v[5] = f
  v[6] = g
  v[7] = h
  v[8] = i
  v[9] = j
  v[10] = k
  v[11] = l
  v[12] = m
  v[13] = n
  v[14] = o
  v[15] = p
  return v
}

const rotation4x4ToQuat = (m: number[], q: number[]): void => {
  const trace = m[0] + m[5] + m[10]
  if (trace >= 0.0) {
    let t = Math.sqrt(trace + 1.0)
    q[3] = 0.5 * t
    t = 0.5 / t
    q[0] = (m[2 + 1 * 4] - m[1 + 2 * 4]) * t
    q[1] = (m[0 + 2 * 4] - m[2 + 0 * 4]) * t
    q[2] = (m[1 + 0 * 4] - m[0 + 1 * 4]) * t
  } else {
    // Find the largest diagonal element and jump to the appropriate case (0, 1 or 2)
    // eslint-disable-next-line no-nested-ternary
    const i = m[0] > m[5] ? (m[0] > m[10] ? 0 : 2) : (m[5] > m[10] ? 1 : 2)
    const j = (i + 1) % 3
    const k = (j + 1) % 3
    let t = Math.sqrt(1.0 + m[i + i * 4] - m[j + j * 4] - m[k + k * 4])
    q[i] = 0.5 * t
    t = 0.5 / t
    q[j] = (m[j + i * 4] + m[i + j * 4]) * t
    q[k] = (m[k + i * 4] + m[i + k * 4]) * t
    q[3] = (m[k + j * 4] - m[j + k * 4]) * t
  }
}

const sqlen3 = (x: number, y: number, z: number): number => x * x + y * y + z * z

type NumberArray = number[] | Float32Array
const unrolled4x4Mul4x4 = (A: NumberArray, B: NumberArray, Outs: NumberArray) => {
  Outs[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3]
  Outs[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3]
  Outs[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3]
  Outs[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3]

  Outs[4] = A[0] * B[4] + A[4] * B[5] + A[8] * B[6] + A[12] * B[7]
  Outs[5] = A[1] * B[4] + A[5] * B[5] + A[9] * B[6] + A[13] * B[7]
  Outs[6] = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] + A[14] * B[7]
  Outs[7] = A[3] * B[4] + A[7] * B[5] + A[11] * B[6] + A[15] * B[7]

  Outs[8] = A[0] * B[8] + A[4] * B[9] + A[8] * B[10] + A[12] * B[11]
  Outs[9] = A[1] * B[8] + A[5] * B[9] + A[9] * B[10] + A[13] * B[11]
  Outs[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] + A[14] * B[11]
  Outs[11] = A[3] * B[8] + A[7] * B[9] + A[11] * B[10] + A[15] * B[11]

  Outs[12] = A[0] * B[12] + A[4] * B[13] + A[8] * B[14] + A[12] * B[15]
  Outs[13] = A[1] * B[12] + A[5] * B[13] + A[9] * B[14] + A[13] * B[15]
  Outs[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] + A[14] * B[15]
  Outs[15] = A[3] * B[12] + A[7] * B[13] + A[11] * B[14] + A[15] * B[15]
}

export {
  fill4x1,
  fill4x4,
  rotation4x4ToQuat,
  sqlen3,
  unrolled4x4Mul4x4,
}

export type {
  NumberArray,
}
