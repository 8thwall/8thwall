import type {Vec3Tuple} from '@ecs/shared/scene-graph'

const computeCentroid = (positions: Readonly<Vec3Tuple>[]): Vec3Tuple => {
  if (positions.length === 0) {
    throw new Error('Cannot compute centroid of an empty list of positions')
  }

  const centroid: Vec3Tuple = [0, 0, 0]
  positions.forEach((pos) => {
    if (!pos || pos.length !== 3) {
      throw new Error('Invalid position data')
    }
    centroid[0] += pos[0]
    centroid[1] += pos[1]
    centroid[2] += pos[2]
  })

  centroid[0] /= positions.length
  centroid[1] /= positions.length
  centroid[2] /= positions.length

  return centroid
}

export {
  computeCentroid,
}
