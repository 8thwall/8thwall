import * as React from 'react'
import {Grid, Line, Text} from '@react-three/drei'
import {Canvas} from '@react-three/fiber'
import type {Object3D} from 'three'

import {brandAltBlack, brandBlack, brandWhite, brandPurple, brandPurpleDark, gray4}
  from '../static/styles/settings'
import {fileExt} from '../editor/editor-common'
import {Splat} from './splat'
import type {ModelInfo} from '../editor/asset-preview-types'
import {GltfPreview} from './gltf-preview'
import type {DebugLightConfig} from './hooks/use-gltf-debug-gui'
import {DEFAULT_DIRECTIONAL_LIGHT_POSITION} from './make-object'
import {PreviewCameraComponent} from './preview-camera'
import {useProcessedGltf} from './hooks/gltf'

const LINE_STEPS = Array.from({length: 12}).map((_, i) => 2 ** i)
LINE_STEPS.unshift(0.5)

interface ILineSquare {
  color: string
  size: number
}

const AXIS_LINES_Y = 0.0009

const LineSquare: React.FC<ILineSquare> = ({color, size}) => (
  <>
    <Text
      fontSize={size / 4}
      color={color}
      position={[-size, 0, -size]}
      rotation-x={-Math.PI / 2}
      anchorX='left'
      anchorY='bottom'
    >
      {size}
    </Text>
    <group scale={[size, size, size]}>
      <Line
        color={color}
        points={[
          [-0.5, 0, -0.5],
          [0.5, 0, -0.5],
        ]}
      />
      <Line
        color={color}
        points={[
          [-0.5, 0, -0.5],
          [-0.5, 0, 0.5],
        ]}
      />
      <Line
        color={color}
        points={[
          [0.5, 0, 0.5],
          [0.5, 0, -0.5],
        ]}
      />
      <Line
        color={color}
        points={[
          [0.5, 0, 0.5],
          [-0.5, 0, 0.5],
        ]}
      />
    </group>
  </>
)

const getLineColor = (isPublicView: boolean, theme: string) => {
  if (isPublicView) {
    return brandPurple
  } else {
    return theme === 'dark' ? brandWhite : brandBlack
  }
}

const getLineColorDark = (isPublicView: boolean, theme: string) => {
  if (isPublicView) {
    return brandPurpleDark
  } else {
    return theme === 'dark' ? brandAltBlack : gray4
  }
}

interface IModelPreview {
  src: string | Object3D
  srcExt?: string
  // When src is not a string, metadata can be passed in
  srcMetadata?: ModelInfo
  wireframe?: boolean
  envName?: string
  onGltfLoad?: (gltf: Object3D) => void
  onModelLoad?: () => void
  grid?: boolean
  gridColor?: string
  showDebugUi?: boolean
  onModelInfo?: (info: ModelInfo) => void
  isPublicView?: boolean
  children?: React.ReactNode
  cameraSticky?: boolean  // If true, the camera position will not change when the model changes
}

// You can pass in a src URL and this component will load the gltf
// e.g. <StudioThreeModelPreview src="https://example.com/model.gltf" />
// Or, you can load the gltf yourself, then pass the src and its metadata
//   // First load the gltf and its metadata
//   const {model, metadata} = useProcessedGltf('https://example.com/model.gltf')
//   // Then, render like this
//   StudioThreeModelPreview src={model} srcMetadata={metadata} />
const StudioThreeModelPreview: React.FunctionComponent<IModelPreview> = ({
  src, srcExt, srcMetadata, wireframe = false, envName, onGltfLoad, onModelLoad, grid = true,
  gridColor, showDebugUi = false, onModelInfo, children = null, isPublicView = false,
  cameraSticky = false,
}) => {
  const isSourceUrl = typeof src === 'string'
  const ext = srcExt || (isSourceUrl ? fileExt(src) : '')
  const {model, metadata} = useProcessedGltf(isSourceUrl && ext !== 'spz' ? src : null)

  const [lightConfig, setLightConfig] = React.useState<DebugLightConfig>({
    ambientIntensity: 1,
    ambientColor: '#ffffff',
    directionalIntensity: 1,
    directionalColor: '#ffffff',
  })

  const theme = 'dark'

  const {ambientIntensity, ambientColor, directionalColor, directionalIntensity} = lightConfig
  const lineColor = getLineColor(isPublicView, theme)
  const lineColorDark = getLineColorDark(isPublicView, theme)

  React.useEffect(() => {
    if (model && onGltfLoad) {
      onGltfLoad(model)
    }
  }, [model, onGltfLoad])

  return (
    <Canvas
      style={{position: 'absolute'}}
    >
      <ambientLight intensity={ambientIntensity * Math.PI} color={ambientColor} />
      <directionalLight
        position={DEFAULT_DIRECTIONAL_LIGHT_POSITION}
        intensity={directionalIntensity * Math.PI}
        color={directionalColor}
      />

      { /* We don't use <GltfModel /> here because we want to handle our own animations. */ }
      {(ext !== 'spz') && (src || model)
        ? (
          <GltfPreview
            metadata={isSourceUrl ? metadata : srcMetadata}
            scene={isSourceUrl ? model : src}
            onModelInfo={onModelInfo}
            showDebug={showDebugUi}
            wireframe={wireframe}
            envMap={envName}
            lightConfig={lightConfig}
            onLightConfigChange={setLightConfig}
          />
        )
        : (
          (ext === 'spz') && isSourceUrl &&
            <Splat src={{type: 'url', url: src}} onLoad={onModelLoad} />
        )}

      {/* Axis lines */}
      <Line color='red' points={[[0, AXIS_LINES_Y, 0], [16, AXIS_LINES_Y, 0]]} />
      <Line color='green' points={[[0, 0, 0], [0, 16, 0]]} />
      <Line color='blue' points={[[0, AXIS_LINES_Y, 0], [0, AXIS_LINES_Y, 16]]} />

      {/* Scale indicators and grid */}
      {grid && LINE_STEPS.map(size => (
        <LineSquare key={size} color={gridColor || lineColor} size={size} />
      ))}
      <PreviewCameraComponent model={isSourceUrl ? model : src} sticky={cameraSticky} />
      {grid && <Grid args={[32, 32, 32, 32]} sectionColor={gridColor || lineColorDark} />}
      {children}
    </Canvas>
  )
}

export default StudioThreeModelPreview
export type {IModelPreview}
