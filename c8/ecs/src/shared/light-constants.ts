const SHADOW_LOW_QUALITY_SIZE = 512
const SHADOW_MEDIUM_QUALITY_SIZE = 1024
const SHADOW_HIGH_QUALITY_SIZE = 2048

const LIGHT_DEFAULTS = {
  castShadow: true,
  intensity: 0.5,
  r: 255,
  g: 255,
  b: 255,
  targetX: 0,
  targetY: 0,
  targetZ: 0,
  shadowBias: -0.005,
  shadowNormalBias: 0,
  shadowRadius: 1,
  shadowAutoUpdate: true,
  shadowBlurSamples: 8,
  shadowMapSizeHeight: SHADOW_MEDIUM_QUALITY_SIZE,
  shadowMapSizeWidth: SHADOW_MEDIUM_QUALITY_SIZE,
  shadowCameraNear: 0.5,
  shadowCameraFar: 200,
  shadowCameraLeft: -50,
  shadowCameraRight: 50,
  shadowCameraTop: 50,
  shadowCameraBottom: -50,
  distance: 0,
  decay: 2,
  followCamera: true,
  angle: Math.PI / 3,
  penumbra: 0,
  colorMap: '',
  width: 10,
  height: 10,
}

const DEFAULT_COLOR = '#ffffff'

export {
  LIGHT_DEFAULTS, DEFAULT_COLOR,
  SHADOW_LOW_QUALITY_SIZE, SHADOW_MEDIUM_QUALITY_SIZE, SHADOW_HIGH_QUALITY_SIZE,
}
