// @sublibrary(:three-types)
import type * as THREE_TYPES from './three-types'

type ShadowLights = THREE_TYPES.DirectionalLight | THREE_TYPES.PointLight | THREE_TYPES.SpotLight
type NoShadowLights = THREE_TYPES.AmbientLight | THREE_TYPES.RectAreaLight
type Lights = ShadowLights | NoShadowLights

export type {
  ShadowLights,
  NoShadowLights,
  Lights,
}
