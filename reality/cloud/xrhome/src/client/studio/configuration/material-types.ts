import type {
  MaterialBlending, Resource, Side, TextureWrap, TextureFiltering,
} from '@ecs/shared/scene-graph'

type ConfiguredTextureResource = Resource | {type: 'default', dataUrl: string}
type MaterialTypes = 'basic' | 'unlit'

// ConfiguredMaterial is essentially the partial properties of any potential material with
// texture resources
type ConfiguredMaterial = {
  type?: MaterialTypes
  color?: string
  textureSrc?: string | ConfiguredTextureResource
  roughness?: number
  metalness?: number
  opacity?: number
  normalScale?: number
  emissiveIntensity?: number
  roughnessMap?: string | ConfiguredTextureResource
  metalnessMap?: string | ConfiguredTextureResource
  opacityMap?: string | ConfiguredTextureResource
  normalMap?: string | ConfiguredTextureResource
  emissiveMap?: string | ConfiguredTextureResource
  emissiveColor?: string
  side?: Side
  blending?: MaterialBlending
  repeatX?: number
  repeatY?: number
  offsetX?: number
  offsetY?: number
  wrap?: TextureWrap
  depthTest?: boolean
  depthWrite?: boolean
  wireframe?: boolean
  forceTransparent?: boolean
  textureFiltering?: TextureFiltering
  mipmaps?: boolean
}

export {
  MaterialTypes,
  ConfiguredMaterial,
  ConfiguredTextureResource,
}
