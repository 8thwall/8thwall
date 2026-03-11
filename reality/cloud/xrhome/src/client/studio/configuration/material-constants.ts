import * as THREE from 'three'

const MATERIAL_BLENDING = {
  'no': THREE.NoBlending,
  'normal': THREE.NormalBlending,
  'additive': THREE.AdditiveBlending,
  'subtractive': THREE.SubtractiveBlending,
  'multiply': THREE.MultiplyBlending,
}

const MATERIAL_SIDE = {
  'front': THREE.FrontSide,
  'back': THREE.BackSide,
  'double': THREE.DoubleSide,
}

// Duplicates of the constants at ecs/shared/material-constants.ts
// TODO: Remove these when release is up-to-date
const DEFAULT_SHADOW_COLOR = '#000000'
const DEFAULT_SHADOW_OPACITY = 0.4

export {
  MATERIAL_BLENDING,
  MATERIAL_SIDE,
  DEFAULT_SHADOW_COLOR,
  DEFAULT_SHADOW_OPACITY,
}
