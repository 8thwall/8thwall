/// <reference types="webxr" />
/* global XRLayer XRProjectionLayer XRWebGLSubImage */

import type {
  Camera, Material, Object3D, Vector4, Scene, WebGLRenderer, BufferGeometry,
  ShaderMaterial, WebGLRenderTarget,
} from './three-types'

import THREE from './three'

import {motionPassFrag, motionPassVert} from './motion-pass-shaders'

type XRWebGLSubImageMotion = XRWebGLSubImage & {
  motionVectorTexture: WebGLTexture
  motionVectorTextureWidth: number
  motionVectorTextureHeight: number
}

// setRenderTargetTextures is a public method in the WebGLRenderer class,
// but it is not part of the type definition
type WebGLRendererMotion = WebGLRenderer & {
  setRenderTargetTextures: (
    renderTarget: WebGLRenderTarget,
    motionVectorTexture: WebGLTexture,
    depthStencilTexture: WebGLTexture
  ) => void
}

const createMotionRenderPass = (scene: Scene) => {
  const motionShaderUniformDefinition = {
    skipMotionRenderPass: {value: false},
    motionVectorScale: {value: new THREE.Vector4(1, 1, 1, 1)},
    boneTexture: {value: new THREE.DataTexture()},
    prevWorldMatrix: {value: new THREE.Matrix4()},
    prevCameraProjectionMatrix: {value: new THREE.Matrix4()},
    prevCameraMatrixWorldInverse: {value: new THREE.Matrix4()},
  }

  const motionShader = new THREE.ShaderMaterial({
    uniforms: motionShaderUniformDefinition,
    vertexShader: motionPassVert,
    fragmentShader: motionPassFrag,
  })

  // eslint-disable-next-line prefer-destructuring
  const uniforms = (motionShader.uniforms as typeof motionShaderUniformDefinition)

  let previousOverrideMaterial: Material | null = null

  const seenCameras = new Set<Camera>()
  const seenObjects = new Set<Object3D>()

  let lastCamera: Camera | undefined

  function motionShaderOnBeforeRender(
    this: ShaderMaterial,
    _renderer: WebGLRenderer,
    _scene: Scene,
    camera: Camera,
    _geometry: BufferGeometry,
    object: Object3D
  ) {
    const material = this

    if (lastCamera !== camera) {
      if (!camera.userData.prevCameraProjectionMatrix) {
        camera.userData.prevCameraProjectionMatrix = camera.projectionMatrix.clone()
      }

      if (!camera.userData.prevCameraMatrixWorldInverse) {
        camera.userData.prevCameraMatrixWorldInverse = camera.matrixWorldInverse.clone()
      }

      uniforms.prevCameraProjectionMatrix.value.copy(camera.userData.prevCameraProjectionMatrix)
      uniforms.prevCameraMatrixWorldInverse.value.copy(camera.userData.prevCameraMatrixWorldInverse)

      lastCamera = camera
      seenCameras.add(camera)
    }

    if (!object.userData.prevWorldMatrix) {
      object.userData.prevWorldMatrix = object.matrixWorld.clone()
    }

    uniforms.skipMotionRenderPass.value = object.userData.skipMotionRenderPass
    uniforms.prevWorldMatrix.value.copy(object.userData.prevWorldMatrix)

    if (object instanceof THREE.SkinnedMesh && object.skeleton?.boneTexture) {
      uniforms.boneTexture.value = object.skeleton.boneTexture
    }

    seenObjects.add(object)

    material.uniformsNeedUpdate = true
  }

  motionShader.onBeforeRender = motionShaderOnBeforeRender

  const beginFrame = (renderer: WebGLRenderer) => {
    previousOverrideMaterial = scene.overrideMaterial
    scene.overrideMaterial = motionShader

    if (renderer.xr && renderer.xr.enabled) {
      const session = renderer.xr.getSession()
      const {layers} = session!.renderState
      const projLayer = layers?.find(
        (layer: any) => layer.textureArrayLength !== undefined
      ) as XRProjectionLayer | undefined

      // Motion Pass needs to consider scaling for double wide texture
      if (projLayer && projLayer.textureArrayLength === 1) {
        uniforms.motionVectorScale.value.set(0.5, 1, 1, 1)
      }
    }

    motionShader.uniformsNeedUpdate = true
  }

  const endFrame = () => {
    for (const camera of seenCameras) {
      camera.userData.prevCameraProjectionMatrix.copy(camera.projectionMatrix)
      camera.userData.prevCameraMatrixWorldInverse.copy(camera.matrixWorldInverse)
    }

    for (const object of seenObjects) {
      object.userData.prevWorldMatrix.copy(object.matrixWorld)
    }

    seenCameras.clear()
    seenObjects.clear()
    lastCamera = undefined
    scene.overrideMaterial = previousOverrideMaterial
  }

  let motionRenderTarget: WebGLRenderTarget
  let cachedMotionVectorTexture: WebGLTexture | null = null
  let cachedDepthStencilTexture: WebGLTexture | null = null
  const render = (renderer: WebGLRenderer | undefined, frame: any) => {
    // Checks for motion pass requirements
    if (!renderer || !frame) {
      return
    }

    const session = renderer.xr.getSession()
    if (!session || !session.renderState.layers) {
      return
    }

    const spaceWarpAvailable = session.enabledFeatures?.includes('space-warp')
    if (!spaceWarpAvailable) {
      return
    }

    const webGlBinding = renderer.xr.getBinding()
    const view = frame.getViewerPose(renderer.xr.getReferenceSpace()).views[0]
    const {layers} = session!.renderState

    // TODO(lreyna): Get better types for projLayer
    const projLayer = layers?.find(
      (layer: XRLayer) => (layer as XRProjectionLayer).textureArrayLength !== undefined
    ) as XRProjectionLayer | undefined

    if (!projLayer) {
      return
    }

    const subImageObject = webGlBinding.getViewSubImage(projLayer, view) as XRWebGLSubImageMotion

    const {motionVectorTexture} = subImageObject as XRWebGLSubImageMotion
    const {depthStencilTexture} = subImageObject

    if (!motionRenderTarget) {
      motionRenderTarget = new THREE.WebGLRenderTarget(
        subImageObject.motionVectorTextureWidth,
        subImageObject.motionVectorTextureHeight,
        {
          format: THREE.RGBAFormat,
          type: THREE.HalfFloatType,
          depthTexture: new THREE.DepthTexture(
            subImageObject.motionVectorTextureWidth,
            subImageObject.motionVectorTextureHeight,
            THREE.UnsignedInt248Type,
            undefined,
            undefined,
            undefined,
            undefined,
            undefined,
            undefined,
            THREE.DepthFormat
          ),
          resolveDepthBuffer: true,
        }
      )
    }

    if (cachedMotionVectorTexture !== motionVectorTexture ||
      cachedDepthStencilTexture !== depthStencilTexture) {
      (renderer as WebGLRendererMotion).setRenderTargetTextures(
        motionRenderTarget,
        motionVectorTexture,
        depthStencilTexture
      )

      // Needed to avoid Three JS from spamming console with warnings
      cachedMotionVectorTexture = motionVectorTexture
      cachedDepthStencilTexture = depthStencilTexture
    }

    renderer.setRenderTarget(motionRenderTarget)

    const xrCamera = renderer.xr.getCamera()
    const cacheViewports: Vector4[] = []
    for (let i = 0, l = xrCamera.cameras.length; i < l; i++) {
      const camera2 = xrCamera.cameras[i]
      cacheViewports.push(camera2.viewport)
      camera2.viewport.set(
        i * (subImageObject.motionVectorTextureWidth / 2),
        0,
        subImageObject.motionVectorTextureWidth / 2,
        subImageObject.motionVectorTextureHeight
      )
    }

    beginFrame(renderer)
    renderer.render(scene, xrCamera)

    for (let i = 0, l = xrCamera.cameras.length; i < l; i++) {
      xrCamera.cameras[i].viewport.copy(cacheViewports[i])
    }

    endFrame()
  }

  return {
    render,
  }
}

export {
  createMotionRenderPass,
}
