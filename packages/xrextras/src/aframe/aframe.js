/* globals AFRAME, XRExtras, XR8 */

import {xrComponents} from './xr-components'
import {xrPrimitives} from './xr-primitives'
import {ensureXrAndExtras} from './ensure'
import {getStartFlowStringOverrides} from './components/start-flow-component'

let xrextrasAframe = null

// We want to start showing the start flow eagerly before AFRAME has loaded and parsed the scene.
const eagerload = () => {
  // Manually traverse the dom for an aframe scene and check its attributes.
  const scene = document.getElementsByTagName('a-scene')[0]
  if (!scene) {
    return
  }
  const attrs = scene.attributes

  // Eagerly inspect the DOM and trigger the start flow if appropriate.
  Object.keys(attrs).forEach((a) => {
    const attr = attrs.item(a).name
    if (attr === 'xrextras-start-flow') {
      const config = AFRAME.utils.styleParser.parse(attrs.item(a).value)
      const stringOverrides = getStartFlowStringOverrides(config)
      window.XRExtras.StartFlow.setStringOverrides(stringOverrides)
      window.XRExtras.StartFlow.show()
    }
  })
}

function create() {
  let registered = false

  // NOTE: new versions of 8frame should be added in descending order, so that the latest version
  // is always in position 1.
  const allowed8FrameVersions = ['latest', '0.9.0', '0.8.2']
  const LATEST_8FRAME = allowed8FrameVersions[1]  // The 'latest' version of 8frame.

  // Check that the requested version of AFrame has a corresponding 8Frame implementation.
  const checkAllowed8FrameVersions = version => new Promise((resolve, reject) => (allowed8FrameVersions.includes(version)
    ? resolve(version === 'latest' ? LATEST_8FRAME : version)
    : reject(`'${version}' is an unsupported AFrame version. Alowed versions: (${allowed8FrameVersions})`)))

  // Load an external javascript resource in a promise that resolves when the javascript has loaded.
  const loadJsPromise = url => new Promise((resolve, reject) => document.head.appendChild(Object.assign(
    document.createElement('script'), {async: true, onload: resolve, onError: reject, src: url}
  )))

  // If XR or XRExtras load before AFrame, we need to manually register their AFrame components.
  const ensureAFrameComponents = () => {
    if (window.XR8) {
      window.AFRAME.registerComponent('xrconfig', XR8.AFrame.xrconfigComponent())
      window.AFRAME.registerComponent('xrweb', XR8.AFrame.xrwebComponent())
      window.AFRAME.registerComponent('xrface', XR8.AFrame.xrfaceComponent())
      window.AFRAME.registerComponent('xrlayers', XR8.AFrame.xrlayersComponent())
      window.AFRAME.registerComponent('xrlayerscene', XR8.AFrame.xrlayersceneComponent())
      window.AFRAME.registerPrimitive('sky-scene', XR8.AFrame.skyscenePrimitive())
    }
    // eslint-disable-next-line @typescript-eslint/no-unused-expressions
    window.XRExtras && window.XRExtras.AFrame.registerXrExtrasComponents()
  }

  // Register a map of component-name -> component, e.g.
  // {
  //   'component-1': component1,
  //   'component-2': component2,
  // }
  const registerComponents =
    components => Object.keys(components).map(k => AFRAME.registerComponent(k, components[k]))

  const registerPrimitives =
    primitives => Object.keys(primitives).map(k => AFRAME.registerPrimitive(k, primitives[k]))

  // Load the 8th Wall preferred version of AFrame at runtime ensuring that xr components are added.
  const loadAFrameForXr = (args) => {
    const {version = 'latest', components = {}} = args || {}
    return checkAllowed8FrameVersions(version)
      .then(ver => loadJsPromise(`./external/scripts/8frame-${ver}.min.js`))
      .then(ensureAFrameComponents)
      .then(ensureXrAndExtras)
      .then(_ => registerComponents(components))
  }

  // Register XRExtras AFrame components.
  const registerXrExtrasComponents = () => {
    // If AFrame is not ready, or we already registered components, skip.
    if (registered || !window.AFRAME) {
      return
    }

    // Only register the components once.
    registered = true

    // Show the loading screen as early as we can - add a System because it will get `init` called
    // before assets finish loading, versus a Component which gets `init` after load.
    AFRAME.registerSystem('eager-load-system', {
      init() {
        try {
          /* eslint-disable-next-line @typescript-eslint/no-unused-expressions */
          window.XRExtras
            ? eagerload()
            : window.addEventListener('xrextrasloaded', eagerload, {once: true})
        } catch (err) {
          // eslint-disable-next-line no-console
          console.error(err)
        }
      },
    })
    registerComponents(xrComponents())
    registerPrimitives(xrPrimitives())
  }

  // Eagerly try to register the aframe components, if aframe has already loaded.
  registerXrExtrasComponents()

  return {
    // Load the 8th Wall version of AFrame at runtime ensuring that xr components are added.
    loadAFrameForXr,
    // Register the XRExtras components. This should only be called after AFrame has loaded.
    registerXrExtrasComponents,
  }
}

const AFrameFactory = () => {
  if (!xrextrasAframe) {
    xrextrasAframe = create()
  }

  return xrextrasAframe
}

export {
  AFrameFactory,
}
