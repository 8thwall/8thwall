type EcsTextureKey = 'textureSrc' | 'roughnessMap' | 'metalnessMap' | 'normalMap' | 'opacityMap' |
  'emissiveMap'
type ThreeTextureKey = 'map' | 'roughnessMap' | 'metalnessMap' | 'normalMap' | 'alphaMap' |
  'emissiveMap'

const threeToEcsTextureKey = (threeTextureKey: ThreeTextureKey): EcsTextureKey => {
  switch (threeTextureKey) {
    case 'map':
      return 'textureSrc'
    case 'alphaMap':
      return 'opacityMap'
    default:
      return threeTextureKey
  }
}

export type {
  EcsTextureKey,
  ThreeTextureKey,
}

export {
  threeToEcsTextureKey,
}
