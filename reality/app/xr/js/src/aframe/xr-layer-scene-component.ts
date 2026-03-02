// Copyright (c) 2022 Niantic, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

/* globals XR8:readonly THREE:readonly */

import {DEFAULT_INVERT_LAYER_MASK, DEFAULT_EDGE_SMOOTHNESS} from '../types/layers'

interface XrLayerSceneComponentState {
  name?: string
  initialized: boolean
  el?: any
  aScene?: any
  layerScene?: any
  renderTarget?: any
  displaySize: {w: number, h: number}
}

const xrlayersceneComponent = () => {
  const state: XrLayerSceneComponentState = {
    name: null,
    initialized: false,
    el: null,
    aScene: null,
    layerScene: null,
    renderTarget: null,
    displaySize: {w: 300, h: 150},
  }

  function init() {
    state.name = this.data.name
    XR8.LayersController.configure({
      layers: {
        [state.name]: {
          invertLayerMask: this.data.invertLayerMask,
          edgeSmoothness: this.data.edgeSmoothness,
        },
      },
    })

    state.el = this.el
    state.aScene = this.el.sceneEl
    state.layerScene = new window.THREE.Scene()
    state.renderTarget = new THREE.WebGLRenderTarget(state.displaySize.w, state.displaySize.h, {
      depthBuffer: true,
      colorSpace: state.aScene.renderer.outputColorSpace,
    })
  }

  const setMatrix = (object3D, matrix) => {
    object3D.matrix.copy(matrix)
    object3D.matrix.decompose(object3D.position, object3D.quaternion, object3D.scale)
    object3D.updateMatrix(true)
    object3D.updateMatrixWorld(true)
  }

  const provideTexture = (): WebGLTexture => {
    if (state.displaySize.w !== state.aScene.canvas.width ||
      state.displaySize.h !== state.aScene.canvas.height) {
      // Change the size right before drawing the new texture b/c setSize() will call this.dispose()
      // and free the texture. Example:
      // - https://r105.threejsfundamentals.org/threejs/lessons/threejs-rendertargets.html
      state.renderTarget?.setSize(state.aScene.canvas.width, state.aScene.canvas.height)
      state.displaySize = {w: state.aScene.canvas.width, h: state.aScene.canvas.height}
    }

    const {renderer, camera} = state.aScene

    // Update the encoding in case it has changed.
    if (state.renderTarget.texture.colorSpace !== renderer.outputColorSpace) {
      state.renderTarget.texture.colorSpace = renderer.outputColorSpace
    }

    // Update the camera parent. Without this content in the layerScene is behind content in the
    // main scene.
    camera.updateWorldMatrix(true, false)
    state.el.object3D.updateMatrix(true)
    state.el.object3D.updateMatrixWorld(true)

    // Save the original local and world space matrices.
    const matrixWorld = state.el.object3D.matrixWorld.clone()
    const matrixLocal = state.el.object3D.matrix.clone()

    // Add the object3D to the layer scene so we can render it to a texture.
    const {parent} = state.el.object3D
    state.layerScene.add(state.el.object3D)
    state.el.object3D.visible = true
    // We want the object3D to have it's original world pose. So copy over the original world pose
    // as the new local pose. Do this b/c when the object3D loses its original parent, it will
    // only have its local pose.
    setMatrix(state.el.object3D, matrixWorld)

    // Render the object3D to a texture.
    renderer.setRenderTarget(state.renderTarget)
    renderer.clear()
    renderer.render(state.layerScene, camera)
    renderer.setRenderTarget(null)

    // Add the object3D back to its parent and make it not visible (so it's not rendered twice).
    parent.add(state.el.object3D)
    state.el.object3D.visible = false
    // Set the original local pose back on the object3D.
    setMatrix(state.el.object3D, matrixLocal)

    return renderer.properties.get(state.renderTarget.texture).__webglTexture
  }

  const tick = () => {
    // Emit here instead of in play() b/c we have seen cases where this play() is called before
    // xrlayers's init(). And if that happens than xrlayers won't receive this event.
    if (!state.initialized) {
      state.aScene.emit('layertextureprovider', {name: state.name, provideTexture})
      state.initialized = true
    }
  }

  return {
    schema: {
      name: {type: 'string', default: ''},
      invertLayerMask: {type: 'bool', default: DEFAULT_INVERT_LAYER_MASK},
      edgeSmoothness: {default: DEFAULT_EDGE_SMOOTHNESS},
    },
    init,
    tick,
  }
}

export {xrlayersceneComponent}
