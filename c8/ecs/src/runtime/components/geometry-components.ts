import * as Types from '../types'

import {registerAttribute} from '../registry'

const SphereGeometry = registerAttribute('sphere-geometry', {radius: Types.f32})

const BoxGeometry = registerAttribute('box-geometry', {
  width: Types.f32,
  height: Types.f32,
  depth: Types.f32,
})

const PlaneGeometry = registerAttribute('plane-geometry', {
  width: Types.f32,
  height: Types.f32,
})

const CapsuleGeometry = registerAttribute('capsule-geometry', {
  radius: Types.f32,
  height: Types.f32,
})

const ConeGeometry = registerAttribute('cone-geometry', {
  radius: Types.f32,
  height: Types.f32,
})

const CylinderGeometry = registerAttribute('cylinder-geometry', {
  radius: Types.f32,
  height: Types.f32,
})

const TetrahedronGeometry = registerAttribute('tetrahedron-geometry', {
  radius: Types.f32,
})

const PolyhedronGeometry = registerAttribute('polyhedron-geometry', {
  faces: Types.ui8,
  radius: Types.f32,
})

const CircleGeometry = registerAttribute('circle-geometry', {
  radius: Types.f32,
})

const RingGeometry = registerAttribute('ring-geometry', {
  innerRadius: Types.f32,
  outerRadius: Types.f32,
})

const TorusGeometry = registerAttribute('torus-geometry', {
  radius: Types.f32,
  tubeRadius: Types.f32,
})

const FaceGeometry = registerAttribute('face-geometry', {})

export {
  SphereGeometry,
  BoxGeometry,
  PlaneGeometry,
  CapsuleGeometry,
  ConeGeometry,
  CylinderGeometry,
  TetrahedronGeometry,
  PolyhedronGeometry,
  CircleGeometry,
  RingGeometry,
  TorusGeometry,
  FaceGeometry,
}
