import {
  DEFAULT_FLOOR_TEXTURE, DEFAULT_BOTTOM_COLOR, DEFAULT_TOP_COLOR,
} from '../webxrcontrollersthreejs/void-space'

// IMPORTANT: Add all components which may not be be added to a-scene to this list.
//
// XR8 components which are not added on the a-scene may not be registered as AFrame components
// before AFrame starts initializing entities. List them so xrconfig can check whether they were not
// registered in time, and so did not get added to their entity.
const NON_SCENE_COMPONENTS = ['xrlayerscene']

const xrConfigSchema = {
  verbose: {type: 'bool', default: false},  // undocumented
  delayRun: {default: false},  // undocumented
  cameraDirection: {type: 'string', default: 'back'},
  allowedDevices: {type: 'string', default: 'mobile-and-headsets'},
  mirroredDisplay: {type: 'bool', default: false},
  disableXrTablet: {default: false},
  xrTabletStartsMinimized: {default: false},
  disableDefaultEnvironment: {default: false},
  disableDesktopCameraControls: {type: 'bool', default: false},
  disableDesktopTouchEmulation: {type: 'bool', default: false},
  disableXrTouchEmulation: {default: false},
  disableCameraReparenting: {type: 'bool', default: false},
  defaultEnvironmentFloorScale: {default: 1},
  defaultEnvironmentFloorTexture: {default: DEFAULT_FLOOR_TEXTURE, type: 'asset'},
  defaultEnvironmentFloorColor: {default: DEFAULT_BOTTOM_COLOR},
  defaultEnvironmentFogIntensity: {default: 1},
  defaultEnvironmentSkyTopColor: {default: DEFAULT_TOP_COLOR},
  defaultEnvironmentSkyBottomColor: {default: DEFAULT_BOTTOM_COLOR},
  defaultEnvironmentSkyGradientStrength: {default: 1},
}

const xrFaceSchema = {
  meshGeometry: {type: 'array', default: ['face']},
  uvType: {type: 'string', default: 'standard'},
  maxDetections: {type: 'number', default: 1},
  enableEars: {type: 'bool', default: false},
}

const xrWebSchema = {
  disableWorldTracking: {type: 'bool', default: false},
  scale: {type: 'string', default: 'responsive'},
  enableVps: {type: 'bool', default: false},
  projectWayspots: {type: 'array', default: []},
  mapSrcUrl: {type: 'string', default: ''},  // undocumented
}

const clearSceneListeners = (sceneListeners: any[], aScene) => {
  sceneListeners.forEach(l => aScene.removeEventListener(l.name, l.callback))
  // eslint-disable-next-line no-param-reassign
  sceneListeners = []
}

const addSceneListener = (sceneListeners: any[], aScene, name: string, callback) => {
  aScene.addEventListener(name, callback)
  sceneListeners.push({name, callback})
}

export {
  NON_SCENE_COMPONENTS, xrConfigSchema, xrFaceSchema, xrWebSchema,
  clearSceneListeners, addSceneListener,
}
