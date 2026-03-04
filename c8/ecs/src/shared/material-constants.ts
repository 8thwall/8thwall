const MATERIAL_DEFAULTS = {
  roughness: 0.5,
  metalness: 0.5,
  opacity: 1,
  side: 'front' as const,
  normalScale: 1,
  emissiveIntensity: 0,
  blending: 'normal' as const,
  repeatX: 1,
  repeatY: 1,
  offsetX: 0,
  offsetY: 0,
  wrap: 'repeat' as const,
  depthTest: true,
  depthWrite: true,
  wireframe: false,
  forceTransparent: false,
  textureFiltering: 'smooth' as const,
  mipmaps: true,
}

const DEFAULT_MATERIAL_COLOR = '#FFFFFF'
const DEFAULT_EMISSIVE_COLOR = '#FFFFFF'
const DEFAULT_SHADOW_COLOR = '#000000'
const DEFAULT_SHADOW_OPACITY = 0.4

// Maximum (recommended) texture size for a single texture before we warn the user about potential
// performance issues
const MAX_RECOMMENDED_TEXTURE_SIZE = 2048

export {
  MATERIAL_DEFAULTS,
  DEFAULT_EMISSIVE_COLOR,
  DEFAULT_MATERIAL_COLOR,
  DEFAULT_SHADOW_COLOR,
  DEFAULT_SHADOW_OPACITY,
  MAX_RECOMMENDED_TEXTURE_SIZE,
}
