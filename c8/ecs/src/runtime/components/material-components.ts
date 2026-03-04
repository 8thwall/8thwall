import * as Types from '../types'

import {registerAttribute} from '../registry'
import {DEFAULT_SHADOW_OPACITY, MATERIAL_DEFAULTS} from '../../shared/material-constants'

const Material = registerAttribute('material', {
  r: Types.ui8,
  g: Types.ui8,
  b: Types.ui8,
  textureSrc: Types.string,
  roughness: Types.f32,
  metalness: Types.f32,
  opacity: Types.f32,
  roughnessMap: Types.string,
  metalnessMap: Types.string,
  side: Types.string,
  normalScale: Types.f32,
  emissiveIntensity: Types.f32,
  emissiveR: Types.ui8,
  emissiveG: Types.ui8,
  emissiveB: Types.ui8,
  opacityMap: Types.string,
  normalMap: Types.string,
  emissiveMap: Types.string,
  blending: Types.string,
  repeatX: Types.f32,
  repeatY: Types.f32,
  offsetX: Types.f32,
  offsetY: Types.f32,
  wrap: Types.string,
  depthTest: Types.boolean,
  depthWrite: Types.boolean,
  wireframe: Types.boolean,
  forceTransparent: Types.boolean,
  textureFiltering: Types.string,
  mipmaps: Types.boolean,
}, MATERIAL_DEFAULTS)

const UnlitMaterial = registerAttribute('unlit-material', {
  r: Types.ui8,
  g: Types.ui8,
  b: Types.ui8,
  textureSrc: Types.string,
  opacity: Types.f32,
  side: Types.string,
  opacityMap: Types.string,
  blending: Types.string,
  repeatX: Types.f32,
  repeatY: Types.f32,
  offsetX: Types.f32,
  offsetY: Types.f32,
  wrap: Types.string,
  depthTest: Types.boolean,
  depthWrite: Types.boolean,
  wireframe: Types.boolean,
  forceTransparent: Types.boolean,
  textureFiltering: Types.string,
  mipmaps: Types.boolean,
}, {
  opacity: MATERIAL_DEFAULTS.opacity,
  side: MATERIAL_DEFAULTS.side,
  blending: MATERIAL_DEFAULTS.blending,
  repeatX: MATERIAL_DEFAULTS.repeatX,
  repeatY: MATERIAL_DEFAULTS.repeatY,
  offsetX: MATERIAL_DEFAULTS.offsetX,
  offsetY: MATERIAL_DEFAULTS.offsetY,
  wrap: MATERIAL_DEFAULTS.wrap,
  depthTest: MATERIAL_DEFAULTS.depthTest,
  depthWrite: MATERIAL_DEFAULTS.depthWrite,
  wireframe: MATERIAL_DEFAULTS.wireframe,
  textureFiltering: MATERIAL_DEFAULTS.textureFiltering,
  mipmaps: MATERIAL_DEFAULTS.mipmaps,
})

const ShadowMaterial = registerAttribute('shadow-material', {
  r: Types.ui8,
  g: Types.ui8,
  b: Types.ui8,
  opacity: Types.f32,
  side: Types.string,
  depthTest: Types.boolean,
  depthWrite: Types.boolean,
}, {
  opacity: DEFAULT_SHADOW_OPACITY,
  side: MATERIAL_DEFAULTS.side,
  depthTest: MATERIAL_DEFAULTS.depthTest,
  depthWrite: MATERIAL_DEFAULTS.depthWrite,
})

const HiderMaterial = registerAttribute('hider-material', {})

const VideoMaterial = registerAttribute('video-material', {
  r: Types.ui8,
  g: Types.ui8,
  b: Types.ui8,
  textureSrc: Types.string,
  opacity: Types.f32,
}, {
  opacity: MATERIAL_DEFAULTS.opacity,
})

export {
  Material,
  HiderMaterial,
  ShadowMaterial,
  UnlitMaterial,
  VideoMaterial,
}
