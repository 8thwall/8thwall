const vertexShader = `
varying vec3 vWorldPosition;
void main() {
  vec4 worldPosition = modelMatrix * vec4(position, 1.0);
  vWorldPosition = worldPosition.xyz;
  gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
}`

const fragmentShader = `
uniform vec3 bottomColor;
uniform vec3 topColor;
uniform float offset;
uniform float exponent;
varying vec3 vWorldPosition;
void main() {
  float h = normalize(vWorldPosition + offset).y;
  float alpha = max(pow(max(h, 0.0), exponent), 0.0);
  gl_FragColor = vec4(mix(bottomColor, topColor, alpha), 1.0);
}`

const DEFAULT_TOP_COLOR = '#BDC0D6'
const DEFAULT_BOTTOM_COLOR = '#1A1C2A'

// eslint-disable-next-line max-len
const DEFAULT_FLOOR_TEXTURE = 'data:image/png;base64,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'

const addVoidSpace = ({
  renderer,
  scene,
  floorScale: floorRepeatScale = 1,
  floorTexture = DEFAULT_FLOOR_TEXTURE,
  floorColor: groundColor = DEFAULT_BOTTOM_COLOR,
  fogIntensity: fogIntensityScale = 1,
  skyTopColor = DEFAULT_TOP_COLOR,
  skyBottomColor = DEFAULT_BOTTOM_COLOR,
  skyGradientStrength = 1,
  skyOffset = 0,
}) => {
  const skyExponent = skyGradientStrength * 2
  const hasOutputEncoding = renderer.outputColorSpace !== window.THREE.LinearColorSpace
  const floorTex = new Promise((resolve) => {
    const texture = new window.THREE.TextureLoader().load(floorTexture, () => resolve(texture))
    texture.wrapS = window.THREE.RepeatWrapping
    texture.wrapT = window.THREE.RepeatWrapping
    // magFilter can be one of:
    //  - NearestFilter, *LinearFilter
    // minFilter can be one of:
    //  - NearestFilter, NearestMipmapNearestFilter, NearestMipmapLinearFilter,
    //    LinearFilter, LinearMipmapNearestFilter, *LinearMipmapLinearFilter
    // where * is the default magFilter and minFilter.
    texture.magFilter = window.THREE.LinearFilter
    texture.minFilter = window.THREE.LinearMipmapLinearFilter
    texture.colorSpace = window.THREE.SRGBColorSpace
    texture.repeat.set(175 / floorRepeatScale, 175 / floorRepeatScale)
  })

  const originalFog = scene.fog
  const voidFog = fogIntensityScale > 0
    ? new window.THREE.FogExp2(skyBottomColor, fogIntensityScale * 0.08)
    : null

  const background = new window.THREE.Group()
  background.name = 'xrweb-background'
  scene.add(background)

  const skyGeometry = new window.THREE.SphereGeometry(1, 64, 32)
  const skyMaterial = new window.THREE.ShaderMaterial({
    uniforms: {
      bottomColor: {value: new window.THREE.Color(skyBottomColor)},
      topColor: {value: new window.THREE.Color(skyTopColor)},
      offset: {value: skyOffset},
      exponent: {value: skyExponent},
    },
    vertexShader,
    fragmentShader,
  })
  const sky = new window.THREE.Mesh(skyGeometry, skyMaterial)
  skyGeometry.scale(-1, 1, 1)
  sky.scale.set(500, 500, 500)
  background.add(sky)

  const floorGeometry = new window.THREE.CircleGeometry()
  const floorMaterial = new window.THREE.MeshBasicMaterial({
    color: hasOutputEncoding
      ? new window.THREE.Color(groundColor).convertSRGBToLinear()
      : groundColor,
  })
  const floor = new window.THREE.Mesh(floorGeometry, floorMaterial)
  floor.visible = false
  floor.scale.set(100, 100, 100)
  floor.position.set(0, -0.01, 0)
  floor.rotateX(-90 * (Math.PI / 180))
  background.add(floor)

  floorTex.then((texture) => {
    floorMaterial.map = texture
    floor.visible = true
  })

  const rescale = (initialSceneCameraMatrix, camera) => {
    // Set the sphere diameter to just under farClip so that one end is visible from the other end,
    // allowing for numeric instability.
    const intrinsics = camera.projectionMatrix.elements
    const farClip = intrinsics[14] / (intrinsics[10] + 1)
    const skyScale = farClip * 0.49
    sky.scale.set(skyScale, skyScale, skyScale)

    // Fog:
    //
    // We want the fog value to be about 90% at a scaled distance of 20. The fog equation is
    //
    //   fog = 1 - exp(- density ^2 * distance^2)
    //
    // so for our targets this becomes
    //       0.9 = 1 - exp(-density^2 * (scale * 20)^2)
    //       0.1 = exp(-density^2 * (scale * 20)^2)
    //  -ln(0.1) = density^2 / (scale * 20)^2
    //   density = sqrt(-ln(0.1)) / (scale * 20)
    //
    // Note that sqrt(-ln(0.1)) ~= 1.5, so we have
    //
    //   density = 0.08 / scale
    const scale = initialSceneCameraMatrix.elements[13] || 1
    if (fogIntensityScale > 0) {
      scene.fog = new window.THREE.FogExp2(skyBottomColor, (fogIntensityScale * 0.08) / scale)
    }

    const floorScale = 100 * scale
    floor.scale.set(floorScale, floorScale, floorScale)
  }

  const setVisibility = (visible) => {
    background.visible = visible

    if (voidFog) {
      scene.fog = visible ? voidFog : originalFog
    }
  }

  setVisibility(true)

  return {
    background,
    remove: () => {
      setVisibility(false)
      if (background.parent) {  // removeFromParent added in threejs r129, just call remove.
        background.parent.remove(background)
      }
    },
    rescale,
    setVisibility,
  }
}

export {
  DEFAULT_FLOOR_TEXTURE,
  DEFAULT_BOTTOM_COLOR,
  DEFAULT_TOP_COLOR,
  addVoidSpace,
}
