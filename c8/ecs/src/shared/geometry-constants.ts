import type {Faces} from './scene-graph'

const DEFAULT_RADIAL_SEGMENTS = 32
const DEFAULT_CAP_SEGMENTS = 4

const TETRAHEDRON_VERTICES = [1, 1, 1, -1, -1, 1, -1, 1, -1, 1, -1, -1]
const TETRAHEDRON_INDICES = [2, 1, 0, 0, 3, 2, 1, 3, 0, 2, 3, 1]
const OCTAHEDRON_VERTICES = [
  1, 0, 0, -1, 0, 0, 0, 1, 0, 0, -1, 0,
  0, 0, 1, 0, 0, -1,
]

const OCTAHEDRON_INDICES = [
  0, 2, 4, 0, 4, 3, 0, 3, 5,
  0, 5, 2, 1, 2, 5, 1, 5, 3,
  1, 3, 4, 1, 4, 2,
]

// https://github.com/mrdoob/three.js/blob/master/src/geometries/DodecahedronGeometry.js
const t = (1 + Math.sqrt(5)) / 2
const r = 1 / t

const ICOSAHEDRON_VERTICES = [
  -1, t, 0, 1, t, 0, -1, -t, 0, 1, -t, 0,
  0, -1, t, 0, 1, t, 0, -1, -t, 0, 1, -t,
  t, 0, -1, t, 0, 1, -t, 0, -1, -t, 0, 1,
]
const ICOSAHEDRON_INDICES = [
  0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
  1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
  3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
  4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1,
]

const DODECAHEDRON_VERTICES = [
  // (±1, ±1, ±1)
  -1, -1, -1, -1, -1, 1,
  -1, 1, -1, -1, 1, 1,
  1, -1, -1, 1, -1, 1,
  1, 1, -1, 1, 1, 1,

  // (0, ±1/φ, ±φ)
  0, -r, -t, 0, -r, t,
  0, r, -t, 0, r, t,

  // (±1/φ, ±φ, 0)
  -r, -t, 0, -r, t, 0,
  r, -t, 0, r, t, 0,

  // (±φ, 0, ±1/φ)
  -t, 0, -r, t, 0, -r,
  -t, 0, r, t, 0, r,
]

const DODECAHEDRON_INDICES = [
  3, 11, 7, 3, 7, 15, 3, 15, 13,
  7, 19, 17, 7, 17, 6, 7, 6, 15,
  17, 4, 8, 17, 8, 10, 17, 10, 6,
  8, 0, 16, 8, 16, 2, 8, 2, 10,
  0, 12, 1, 0, 1, 18, 0, 18, 16,
  6, 10, 2, 6, 2, 13, 6, 13, 15,
  2, 16, 18, 2, 18, 3, 2, 3, 13,
  18, 1, 9, 18, 9, 11, 18, 11, 3,
  4, 14, 12, 4, 12, 0, 4, 0, 8,
  11, 9, 5, 11, 5, 19, 11, 19, 7,
  19, 5, 14, 19, 14, 4, 19, 4, 17,
  1, 12, 14, 1, 14, 5, 1, 5, 9,
]

type GeometryMapping = {
  [key: number]: {
    vertices: number[]
    indices: number[]
  }
};

const FACES_TO_GEOMETRY: GeometryMapping = {
  4: {
    vertices: TETRAHEDRON_VERTICES,
    indices: TETRAHEDRON_INDICES,
  },
  8: {
    vertices: OCTAHEDRON_VERTICES,
    indices: OCTAHEDRON_INDICES,
  },
  12: {
    vertices: ICOSAHEDRON_VERTICES,
    indices: ICOSAHEDRON_INDICES,
  },
  20: {
    vertices: DODECAHEDRON_VERTICES,
    indices: DODECAHEDRON_INDICES,
  },
}

const getPolyhedronGeometry = (faces: Faces) => {
  if (!(faces in FACES_TO_GEOMETRY)) {
    // eslint-disable-next-line no-console
    console.error(`Invalid number of faces: ${faces}`)
    return null
  }
  return FACES_TO_GEOMETRY[faces]
}

export {
  DEFAULT_RADIAL_SEGMENTS,
  DEFAULT_CAP_SEGMENTS,
  getPolyhedronGeometry,
}
