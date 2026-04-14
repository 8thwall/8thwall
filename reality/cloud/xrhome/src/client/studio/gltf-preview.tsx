import React from 'react'
import {EquirectangularReflectionMapping, Mesh, Object3D, Texture, TextureLoader} from 'three'
import {RGBELoader} from 'three/examples/jsm/loaders/RGBELoader'

import type {ModelInfo} from '../editor/asset-preview-types'
import {DebugLightConfig, useGltfDebugGui} from './hooks/use-gltf-debug-gui'
import {useAbandonableEffect} from '../hooks/abandonable-effect'
import {GltfLoadBoundary} from './gltf-load-boundary'

const changeEnvMap = (scene: Object3D, newEnvMap: Texture) => {
  scene.traverse((object) => {
    if (object instanceof Mesh && object.isMesh) {
      const {material} = object
      material.envMap = newEnvMap
      material.needsUpdate = true
    }
  })
}

interface IGltfPreview {
  scene: Object3D
  metadata: ModelInfo
  showDebug?: boolean
  wireframe?: boolean
  envMap?: string
  lightConfig?: DebugLightConfig
  onLightConfigChange?: (config: DebugLightConfig) => void
  onModelInfo?: (info: ModelInfo) => void
}

const GltfPreviewInner: React.FC<IGltfPreview> = ({
  scene, metadata, showDebug = false, wireframe = false, envMap,
  lightConfig, onLightConfigChange, onModelInfo,
}) => {
  const [guiWireframe, setWireframe] = React.useState(wireframe)
  const [guiEnvMap, setEnvMap] = React.useState(envMap)

  useGltfDebugGui({
    visible: showDebug,
    scene,
    lightConfig,
    onLightConfigChange,
    onEnvMapChange: setEnvMap,
    onWireframeChange: setWireframe,
  })

  React.useEffect(() => {
    if (!metadata) {
      return
    }

    onModelInfo?.(metadata)
  }, [metadata])

  React.useEffect(() => {
    if (!scene) {
      return
    }

    scene.traverse((object) => {
      if (object instanceof Mesh && object.isMesh) {
        const {material} = object
        material.wireframe = wireframe || guiWireframe
        material.needsUpdate = true
      }
    })
  }, [scene, wireframe, guiWireframe])

  useAbandonableEffect(async (abandon) => {
    const finalEnvMap = envMap || guiEnvMap
    if (!finalEnvMap || !scene) { return }
    const ext = finalEnvMap.split('.').pop()
    const isSpecial = ext === 'hdr' || ext === 'exr'
    const loader = isSpecial ? new RGBELoader() : new TextureLoader()
    if (finalEnvMap && finalEnvMap !== 'none') {
      const texture = await abandon(new Promise<Texture>((resolve, reject) => {
        loader.load(finalEnvMap, (text) => {
          text.mapping = EquirectangularReflectionMapping
          resolve(text)
        }, undefined, reject)
      }))

      changeEnvMap(scene, texture)
    } else {
      changeEnvMap(scene, null)
    }
  }, [envMap, guiEnvMap, scene])

  return scene && <primitive object={scene} />
}

const GltfPreview: React.FC<IGltfPreview> = ({...rest}) => (
  <GltfLoadBoundary>
    <GltfPreviewInner {...rest} />
  </GltfLoadBoundary>
)

export {
  GltfPreview,
}
