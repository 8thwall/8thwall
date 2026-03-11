import React from 'react'

import {
  DEFAULT_BOTTOM_COLOR, DEFAULT_HEIGHT_SEGMENTS, DEFAULT_SIZE, DEFAULT_TOP_COLOR,
  DEFAULT_WIDTH_SEGMENTS,
  ISOLATION_BOTTOM_COLOR,
  ISOLATION_TOP_COLOR,
} from '@ecs/shared/sky-constants'
import {BackSide, Color, Material, MeshBasicMaterial, ShaderMaterial, Texture, Vector3} from 'three'
import type {Resource, Sky} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {useTexture} from './hooks/use-texture'
import {useUiTheme} from '../ui/theme'
import {BACKGROUND_SKYBOX_ORDER} from '../../shared/ecs/shared/sky-constants'

const hexToVec3 = (hex: string) => {
  const c = new Color(hex)
  return new Vector3(c.r, c.g, c.b)
}

// Note(Dale): GLSL doesn't support dynamic array sizes, so we need to pad the colors array
const padColorsArray = (colors: Vector3[]): void => {
  while (colors.length < 5) {
    colors.push(colors[colors.length - 1] || new Vector3(0, 0, 0))
  }
}

const createImageMaterial = (texture: Texture) => new MeshBasicMaterial({
  map: texture,
  side: BackSide,
})

const createUniforms = (sky: DeepReadonly<Sky>, topColor?: string, bottomColor?: string) => {
  if (sky?.type !== 'gradient') {
    return undefined
  }

  let colorVectors: Vector3[] = []

  if (sky && sky.colors && sky.colors.length > 0) {
    colorVectors = sky.colors.map(color => hexToVec3(color))
  } else {
    const topColorVec = hexToVec3(topColor || DEFAULT_TOP_COLOR)
    const bottomColorVec = hexToVec3(bottomColor || DEFAULT_BOTTOM_COLOR)
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

const createGradientMaterial = (
  sky: DeepReadonly<Sky>, topColor?: string, bottomColor?: string
) => {
  const uniforms = createUniforms(sky, topColor, bottomColor)

  if (!uniforms) {
    return undefined
  }

  /* eslint-disable local-rules/hardcoded-copy */
  return new ShaderMaterial({
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
    side: BackSide,
  })
  /* eslint-enable local-rules/hardcoded-copy */
}

const createSolidMaterial = (color: string) => new MeshBasicMaterial({
  color: new Color(color),
  side: BackSide,
})

const createSceneBackgroundMaterial = (
  sky: DeepReadonly<Sky>, topColor?: string, bottomColor?: string
) => {
  if (sky?.type === 'none' || sky?.type === 'image') {
    return undefined
  }

  let material: ShaderMaterial | MeshBasicMaterial

  switch (sky?.type) {
    case 'color':
      material = createSolidMaterial(sky.color || DEFAULT_BOTTOM_COLOR)
      break
    case 'gradient':
      material = createGradientMaterial(sky, topColor, bottomColor)
      break
    default:
      material = createGradientMaterial({type: 'gradient'}, topColor, bottomColor)
  }

  return material
}

interface IImageBackground {
  src: Resource
}

const ImageBackground: React.FC<IImageBackground> = ({src}) => {
  const texture = useTexture(src)

  const material = React.useMemo(() => {
    if (!texture) {
      return undefined
    }
    return createImageMaterial(texture)
  }, [texture])

  if (!material) {
    return undefined
  }

  // note(alancastillo): render order needs to be less than all of the other entities in the scene
  return (
    <mesh renderOrder={BACKGROUND_SKYBOX_ORDER}>
      <sphereGeometry args={[DEFAULT_SIZE, DEFAULT_WIDTH_SEGMENTS, DEFAULT_HEIGHT_SEGMENTS]} />
      <primitive object={material} attach='material' />
    </mesh>
  )
}

interface ISceneBackground {
  sky: DeepReadonly<Sky>
  isIsolated?: boolean
}

const SceneBackground: React.FC<ISceneBackground> = ({sky, isIsolated}) => {
  const theme = useUiTheme()

  let material: Material

  if (isIsolated) {
    material = createGradientMaterial(
      {type: 'gradient'}, ISOLATION_TOP_COLOR, ISOLATION_BOTTOM_COLOR
    )
  } else if (sky?.type === 'image') {
    return <ImageBackground src={sky.src} />
  } else {
    material = createSceneBackgroundMaterial(sky, theme.studioSkyboxStart, theme.studioSkyboxEnd)
  }

  if (!isIsolated && sky?.type === 'image') {
    return <ImageBackground src={sky.src} />
  }

  if (!material) {
    return undefined
  }

  return (
    <mesh renderOrder={BACKGROUND_SKYBOX_ORDER} raycast={() => {}}>
      <sphereGeometry args={[DEFAULT_SIZE, DEFAULT_WIDTH_SEGMENTS, DEFAULT_HEIGHT_SEGMENTS]} />
      <primitive object={material} attach='material' />
    </mesh>
  )
}

export {
  SceneBackground,
}
