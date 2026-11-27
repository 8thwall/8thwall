// Components that make an <a-scene> an 8th Wall scene.
const XR_SCENE_COMPONENTS = ['xrweb', 'xrface', 'xrlayers', 'xrconfig']

const isXrScene = scene => XR_SCENE_COMPONENTS.some(name => scene.hasAttribute(name))

export {
  isXrScene,
}
