const hasGltfLoader = () => !!(window.THREE?.GLTFLoader) || !!window.GLTFLoader

const makeGltfLoader = () => {
  if (window.THREE.GLTFLoader) {
    return new window.THREE.GLTFLoader()
  } else if (window.GLTFLoader) {
    return new window.GLTFLoader()
  } else {
    throw new Error(
      'Expected GLTF loader to be present as window.THREE.GLTFLoader or window.GLTFLoader'
    )
  }
}

export {
  hasGltfLoader,
  makeGltfLoader,
}
