const THREE_BASE = 'https://cdn.jsdelivr.net/gh/mrdoob/three.js@r131'

const THREE_SRC = `${THREE_BASE}/build/three.min.js`
const ORBIT_SRC = `${THREE_BASE}/examples/js/controls/OrbitControls.js`
const GLTF_SRC = `${THREE_BASE}/examples/js/loaders/GLTFLoader.js`

const ORBIT_SRC_FOR_AFRAME = 'https://cdn.jsdelivr.net/gh/supermedium/three.js@super-r113' +
  '/examples/js/controls/OrbitControls.js'

const loadScript = (url: string) => new Promise<void>((resolve, reject) => {
  const script = document.createElement('script') as unknown as HTMLScriptElement
  script.async = true
  script.crossOrigin = 'anonymous'
  script.onload = () => resolve()
  script.onerror = reject
  script.src = url
  document.head.appendChild(script)
})

let loadPromise = null

const loadThreeJs = () => {
  if (loadPromise) {
    return loadPromise
  }

  loadPromise = (async () => {
    if (window.AFRAME) {
      if (!window.THREE || !window.THREE.GLTFLoader) {
        throw new Error('Expected AFRAME to define THREE and THREE.GLTFLoader')
      }
      if (!window.THREE.OrbitControls) {
        await loadScript(ORBIT_SRC_FOR_AFRAME)
      }
      return
    }

    if (!window.THREE) {
      await loadScript(THREE_SRC)
    }
    const orbitPromise = window.THREE.OrbitControls ? null : loadScript(ORBIT_SRC)
    const gltfPromise = window.THREE.GLTFLoader ? null : loadScript(GLTF_SRC)
    await Promise.all([orbitPromise, gltfPromise])
  })()

  return loadPromise
}

export {
  loadThreeJs,
}
