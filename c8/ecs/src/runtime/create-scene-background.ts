import type {DeepReadonly} from 'ts-essentials'

import THREE from './three'
import {
  BACKGROUND_SKYBOX_ORDER,
  DEFAULT_BOTTOM_COLOR, DEFAULT_HEIGHT_SEGMENTS, DEFAULT_SIZE, DEFAULT_TOP_COLOR,
  DEFAULT_WIDTH_SEGMENTS,
} from '../shared/sky-constants'
import type {EffectsManagerSky} from './effects-manager-types'
import type {Mesh, MeshBasicMaterial, ShaderMaterial, Vector3} from './three-types'
import {assets} from './assets'

const hexToVec3 = (hex: string) => {
  const c = new THREE.Color(hex)
  return new THREE.Vector3(c.r, c.g, c.b)
}

// Note(Dale): GLSL doesn't support dynamic array sizes, so we need to pad the colors array
const padColorsArray = (colors: Vector3[]) => {
  while (colors.length < 5) {
    colors.push(colors[colors.length - 1] || new THREE.Vector3(0, 0, 0))
  }
}

const createImageMaterial = async (src: string | undefined) => {
  if (!src) {
    return undefined
  }

  const loader = new THREE.TextureLoader()

  return new Promise<MeshBasicMaterial | undefined>((resolve) => {
    assets.load({url: src}).then((asset) => {
      loader.load(asset.localUrl, (texture) => {
        resolve(new THREE.MeshBasicMaterial({
          map: texture,
          side: THREE.BackSide,
          depthTest: false,
          depthWrite: false,
        }))
      })
    })
  })
}

const createUniforms = (sky: DeepReadonly<EffectsManagerSky> | null) => {
  if (sky?.type !== 'gradient') {
    return undefined
  }

  let colorVectors: Vector3[] = []

  if (sky && sky.colors && sky.colors.length > 0) {
    colorVectors = sky.colors.map(color => hexToVec3(color))
  } else {
    const topColorVec = hexToVec3(DEFAULT_TOP_COLOR)
    const bottomColorVec = hexToVec3(DEFAULT_BOTTOM_COLOR)
    colorVectors = [topColorVec, bottomColorVec]
  }

  const {length} = colorVectors
  padColorsArray(colorVectors)

  const gradientStyle = sky.style === 'radial' ? 1 : 0

  return {
    colors: {value: colorVectors},
    numColors: {value: length},
    gradientStyle: {value: gradientStyle},
  }
}

const createGradientMaterial = (sky: DeepReadonly<EffectsManagerSky> | null) => {
  const uniforms = createUniforms(sky)

  if (!uniforms) {
    return undefined
  }

  return new THREE.ShaderMaterial({
    uniforms,
    vertexShader: `
      varying vec2 vUv;
      void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,
    fragmentShader: `
      #define MAX_COLORS 5
      uniform int numColors;
      uniform vec3 colors[MAX_COLORS];
      uniform int gradientStyle;
      varying vec2 vUv;

      void main() {
        vec3 gradientColor = colors[0];

        if (numColors == 1) {
          gl_FragColor = vec4(gradientColor, 1.0);
          return;
        }

        if (gradientStyle == 0) {
          float step = 1.0 / float(numColors - 1);
          for (int i = 1; i < numColors; ++i) {
            gradientColor = mix(
              gradientColor, colors[i], smoothstep(step * float(i - 1),
              step * float(i), 1.0 - vUv.y)
            );
          }
        } else {
          float radius = length(vUv - vec2(0.5, 0.5)) * 2.0;
          float step = 1.0 / float(numColors - 1);
          for (int i = 1; i < numColors; ++i) {
            gradientColor = mix(
              gradientColor, colors[i], smoothstep(step * float(i - 1),
              step * float(i), radius)
            );
          }
        }

        gl_FragColor = vec4(gradientColor, 1.0);
        #include <colorspace_fragment>
      }
    `,
    side: THREE.BackSide,
    depthTest: false,
    depthWrite: false,
  })
}

const createSolidMaterial = (color: string) => new THREE.MeshBasicMaterial({
  color: new THREE.Color(color),
  side: THREE.BackSide,
  depthTest: false,
  depthWrite: false,
})

const createSceneBackgroundMaterial = async (sky: DeepReadonly<EffectsManagerSky> | undefined) => {
  if (sky?.type === 'none') {
    return undefined
  }

  let material: ShaderMaterial | MeshBasicMaterial | undefined

  switch (sky?.type) {
    case 'color':
      material = createSolidMaterial(sky.color || DEFAULT_BOTTOM_COLOR)
      break
    case 'gradient':
      material = createGradientMaterial(sky)
      break
    case 'image':
      material = await createImageMaterial(sky.src)
      break
    default:
      material = createGradientMaterial(null)
  }

  return material
}

const createSceneBackground = async (sky: DeepReadonly<EffectsManagerSky> | undefined) => {
  const material = await createSceneBackgroundMaterial(sky)

  if (!material) {
    return undefined
  }

  material.userData.disposable = true

  const sphereGeometry = new THREE.SphereGeometry(
    DEFAULT_SIZE, DEFAULT_WIDTH_SEGMENTS, DEFAULT_HEIGHT_SEGMENTS
  )

  const mesh = new THREE.Mesh(sphereGeometry, material)
  mesh.geometry.userData.disposable = true
  mesh.renderOrder = BACKGROUND_SKYBOX_ORDER
  mesh.raycast = () => {}  // NOTE(johnny): Disable raycasting on the skybox.
  return mesh
}

const updateSkyBoxMaterial = async (
  skyBox: Mesh,
  sky: DeepReadonly<EffectsManagerSky> | undefined
) => {
  const material = await createSceneBackgroundMaterial(sky)

  if (!material) {
    return undefined
  }

  skyBox.material = material
  return material
}

export {
  createSceneBackground,
  updateSkyBoxMaterial,
}
