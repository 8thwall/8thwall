import React from 'react'
import {useThree} from '@react-three/fiber'
import * as THREE from 'three'
import {THREE_LAYERS} from '@ecs/shared/three-layers'

import {makeOutlineMaterial} from './shader/outline-material'
import {UseFrame} from './use-frame'
import {useStudioStateContext} from './studio-state-context'
import {mango} from '../static/styles/settings'

type PostProcessingPass = {
  outlineFbo: THREE.WebGLRenderTarget
  outlineMaterial: THREE.ShaderMaterial
  redMaterial: THREE.MeshBasicMaterial
  fullScreenCamera: THREE.OrthographicCamera
  fullScreenScene: THREE.Scene
}

const createPostProcessingPass = () => {
  const outlineFbo = new THREE.WebGLRenderTarget(0, 0, {
    depthBuffer: false,
    colorSpace: THREE.LinearSRGBColorSpace,
  })
  outlineFbo.texture.generateMipmaps = false

  const outlineMaterial = makeOutlineMaterial({
    texture: outlineFbo.texture,
    canvasWidth: 0,
    canvasHeight: 0,
    thickness: 1.0,
    color: new THREE.Color(mango),
  })

  const fullScreenCamera = new THREE.OrthographicCamera(-1, 1, 1, -1, 0, 1)

  const fullScreenScene = new THREE.Scene()
  const quad = new THREE.Mesh(new THREE.PlaneGeometry(2, 2), outlineMaterial)
  fullScreenScene.add(quad)

  return {
    outlineFbo,
    outlineMaterial,
    redMaterial: new THREE.MeshBasicMaterial({color: 'red', depthWrite: false}),
    fullScreenCamera,
    fullScreenScene,
  }
}

const OutlinePostProcessor: React.FC<{}> = () => {
  const {size} = useThree()
  const {selectedIds} = useStudioStateContext().state
  const postProcessingPass = React.useRef<PostProcessingPass | null>(null)

  const getPostProcessingPass = () => {
    if (!postProcessingPass.current) {
      postProcessingPass.current = createPostProcessingPass()
    }
    return postProcessingPass.current
  }

  React.useEffect(() => () => {
    if (postProcessingPass.current) {
      postProcessingPass.current.outlineFbo.texture.dispose()
      postProcessingPass.current.outlineFbo.dispose()
    }
  }, [])

  React.useEffect(() => {
    const {outlineFbo, outlineMaterial} = getPostProcessingPass()
    outlineFbo.setSize(size.width, size.height)
    outlineMaterial.uniforms.resolution.value.set(size.width, size.height)
  }, [size])

  if (selectedIds.length) {
    return (
      <>
        <UseFrame
          callback={(state) => {
            // NOTE(chloe): Draw texture with high render priority to prevent de-sync with the
            // selected objects' poses. Set camera to outline layer and set scene.overrideMaterial
            // to redMaterial to render outlined objects as red in the outlineFbo.texture as a
            // makeshift "stencil" for the outline shader to process.
            const {outlineFbo, redMaterial} = getPostProcessingPass()
            state.gl.setRenderTarget(outlineFbo)
            const {mask} = state.camera.layers
            state.camera.layers.set(THREE_LAYERS.outline)
            state.scene.overrideMaterial = redMaterial

            state.gl.render(state.scene, state.camera)

            state.camera.layers.mask = mask
            state.gl.setRenderTarget(null)
            state.scene.overrideMaterial = null
          }}
          renderPriority={1000}
        />
        <UseFrame
          callback={(state) => {
            // NOTE(chloe): Disable autoClear to prevent main scene from being cleared before
            // rendering the outlines overlaid on top.
            const {fullScreenScene, fullScreenCamera} = getPostProcessingPass()
            state.gl.autoClear = false
            state.gl.render(fullScreenScene, fullScreenCamera)
            state.gl.autoClear = true
          }}
          renderPriority={1001}
        />
      </>
    )
  }
  return null
}

export {
  OutlinePostProcessor,
}
