const skyboxFragmentShader = `
    uniform samplerCube envMap;
    uniform float flipEnvMap;
    varying vec3 vWorldDirection;
    void main() {
      vec4 texColor = textureCube( envMap, 
        vec3( flipEnvMap * vWorldDirection.x, vWorldDirection.yz ) );
      gl_FragColor = texColor;
      #include <colorspace_fragment>
    }
  `

const createCubemapRenderer = (backgroundCubeTexture) => {
  const {THREE} = (window as any)
  if (!THREE) {
    throw new Error('THREE not found')
  }

  const skyboxMesh = new THREE.Mesh(
    new THREE.BoxGeometry(1, 1, 1),
    new THREE.ShaderMaterial({
      name: 'BackgroundCubeMaterial',
      uniforms: THREE.UniformsUtils.clone(THREE.ShaderLib.backgroundCube.uniforms),
      vertexShader: THREE.ShaderLib.backgroundCube.vertexShader,
      fragmentShader: skyboxFragmentShader,
      side: THREE.BackSide,
      fog: false,
    })
  )

  skyboxMesh.geometry.deleteAttribute('normal')
  skyboxMesh.geometry.deleteAttribute('uv')

  // eslint-disable-next-line func-names
  skyboxMesh.onBeforeRender = function (renderer, scene, camera) {
    this.matrixWorld.copyPosition(camera.matrixWorld)
  }

  Object.defineProperty(skyboxMesh.material, 'envMap', {
    get() {
      return this.uniforms.envMap.value
    },
  })

  skyboxMesh.material.uniforms.envMap.value = backgroundCubeTexture
  skyboxMesh.material.uniforms.flipEnvMap.value = 1
  skyboxMesh.material.uniforms.backgroundBlurriness.value = 0
  skyboxMesh.material.uniforms.backgroundIntensity.value = 1
  skyboxMesh.material.toneMapped = false

  skyboxMesh.layers.enableAll()
  skyboxMesh.frustumCulled = false
  return skyboxMesh
}

const addSplatCubeMap = (
  splat, images: {data: Uint8Array, width: number, height: number}[]
) => {
  const {THREE} = (window as any)
  if (!THREE) {
    throw new Error('THREE not found')
  }

  const cubeTexture = new THREE.CubeTexture(images.map((image) => {
    const tex = new THREE.DataTexture(image.data, image.width, image.height)
    tex.needsUpdate = true
    return tex
  }))
  cubeTexture.colorSpace = THREE.SRGBColorSpace
  cubeTexture.needsUpdate = true
  const cubeMesh = createCubemapRenderer(cubeTexture)
  splat.add(cubeMesh)
  return cubeMesh
}

export {addSplatCubeMap}
