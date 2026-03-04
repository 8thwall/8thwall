import type {Eid} from '../shared/schema'
import type {Vec3} from './math/vec3'
import type {Intersection} from './three-types'

type IntersectionResult = {
  eid?: Eid
  point: Vec3
  distance: number
  threeData: Intersection
}

export type {
  IntersectionResult,
}
