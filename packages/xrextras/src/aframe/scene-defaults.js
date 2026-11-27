/* globals AFRAME */

import {isXrScene} from './xr-scene'

// A-Frame scene components that are not compatible with the 8th Wall Engine, disabled by default:
// - xr-mode-ui: its enter VR/AR buttons would start a WebXR session outside of the 8th Wall
//   Engine, which starts headset sessions itself and shows its own "Enter AR/VR" button when needed.
// - device-orientation-permission-ui: the 8th Wall Engine requests the motion permission itself.
// A-Frame adds them to every scene and initializes them as soon as the scene is connected, so
// change the default of their `enabled` property. This works for scenes in the HTML and scenes
// added later, e.g. after `xrloaded`. A value set on the scene is kept.
const DISABLED_COMPONENTS = [
  'xr-mode-ui',
  'device-orientation-permission-ui',
]

const disableComponentsByDefault = () => {
  DISABLED_COMPONENTS.forEach((name) => {
    const component = AFRAME.components[name]
    if (component && component.schema && component.schema.enabled) {
      component.schema.enabled.default = false
    }
  })
}

// A-Frame's loading screen renders its own loop on the scene canvas while XRExtras shows its
// loading screen (xrextras-loading). It isn't a component: A-Frame reads the `loading-screen`
// attribute when it sets up the scene, after creating the canvas. A-Frame emits
// `render-target-loaded` (bubbling) once the canvas is created, so set the attribute on 8th Wall
// scenes then.
const disableLoadingScreen = () => {
  document.addEventListener('render-target-loaded', (event) => {
    const scene = event.target
    if (!scene || !scene.isScene || !isXrScene(scene)) {
      return
    }
    HTMLElement.prototype.setAttribute.call(scene, 'loading-screen', 'enabled: false')
  })
}

// Disables A-Frame's enter VR/AR buttons, motion permission dialog and loading screen, not
// compatible with the 8th Wall Engine. XRExtras needs to be loaded before the scene is added to
// the page.
const applySceneDefaults = () => {
  disableComponentsByDefault()
  disableLoadingScreen()
}

export {
  applySceneDefaults,
}
