import React from 'react'
import type {Object3D} from 'three'
import {GLTF, GLTFLoader} from 'three/examples/jsm/loaders/GLTFLoader'
import {DRACOLoader} from 'three/examples/jsm/loaders/DRACOLoader'
import {suspend} from 'suspend-react'

import {useGLTF} from '@react-three/drei'

import {DRACO_DECODER_PATH} from '../../../shared/studio/cdn-assets'

import type {ModelInfo} from '../../editor/asset-preview-types'
import {clone as skeletonClone} from '../skeleton-utils'
import {cloneMaterialAndGeometry, getObject3DInfo} from '../asset-previews/asset-utils'

const resolveInput = (input: string | null | undefined) => {
  if (!input) {
    return null
  }
  try {
    const url = new URL(input.startsWith('//') ? `https:${input}` : input)
    return url.toString()
  } catch (err) {
    return null
  }
}

const useGltfWithDraco = (input: string | null | undefined) => {
  const validInput = resolveInput(input)
  return useGLTF(validInput ? [validInput] : [], DRACO_DECODER_PATH)[0]
}

interface GltfProcessingOptions {
  cloneMaterials?: boolean
}
interface GltfData {
  model: Object3D | null
  metadata: ModelInfo | null
}

const useProcessedGltf = (
  input: string | null | undefined,
  options: GltfProcessingOptions = {}
): GltfData => {
  const gltf = useGltfWithDraco(input)
  const sceneClone = React.useMemo(() => {
    if (!gltf || !gltf.scene) {
      return null
    }

    const clonedScene = skeletonClone(gltf.scene)
    clonedScene.animations = gltf.animations

    if (options.cloneMaterials) {
      cloneMaterialAndGeometry(clonedScene, gltf.scene)
    }

    return clonedScene
  }, [gltf, options.cloneMaterials])

  return {
    model: sceneClone,
    metadata: sceneClone ? getObject3DInfo(sceneClone) : null,
  }
}

const gltfLoader = new GLTFLoader()
const dracoLoader = new DRACOLoader()
dracoLoader.setDecoderPath(DRACO_DECODER_PATH)
gltfLoader.setDRACOLoader(dracoLoader)
const loadGltfFromArrayBuffer = async (
  arrBuffer: ArrayBuffer | null, options: GltfProcessingOptions = {}
) => {
  if (!arrBuffer) {
    return null
  }
  const loaderPromise = new Promise<GLTF>((resolve, reject) => {
    gltfLoader.parse(arrBuffer, '', (gltf) => {
      resolve(gltf)
    }, (error) => {
      reject(error)
    })
  })

  try {
    const gltf = await loaderPromise
    const clonedScene = skeletonClone(gltf.scene)
    clonedScene.animations = gltf.animations

    if (options.cloneMaterials) {
      cloneMaterialAndGeometry(clonedScene, gltf.scene)
    }

    const metadata = getObject3DInfo(clonedScene)
    metadata.sizeInBytes = arrBuffer.byteLength
    return {
      model: clonedScene,
      metadata,
    }
  } catch (error) {
    console.error('Failed to load GLTF from ArrayBuffer:', error)
    return null
  }
}

// Same as useProcessedGltf but accepts an ArrayBuffer
const useProcessedGltfArrayBuffer = (
  input: ArrayBuffer | null,
  options: GltfProcessingOptions = {}
): GltfData => {
  const gltfData = suspend(loadGltfFromArrayBuffer, [input, options])

  return gltfData
}

export {
  useGltfWithDraco,
  useProcessedGltf,
  useProcessedGltfArrayBuffer,
  loadGltfFromArrayBuffer,
}

export type {
  GltfData,
  GltfProcessingOptions,
}
