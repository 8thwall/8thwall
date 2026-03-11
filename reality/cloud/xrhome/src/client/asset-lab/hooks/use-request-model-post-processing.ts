import * as THREE from 'three'
import {useEffect, useState} from 'react'
import {NodeIO, Transform, TypedArray} from '@gltf-transform/core'
import {
  draco, dedup, instance, palette, flatten, join, weld, resample, prune, sparse, simplify,
  textureCompress,
} from '@gltf-transform/functions'
import {MeshoptSimplifier} from 'meshoptimizer'

import type {Vec3Tuple} from '@ecs/shared/scene-graph'

import {smoothNormals, getAdjustPbr, getAdjustBrightness} from '../gltf-transforms'
import {useArrayBufferFetch} from '../../studio/hooks/use-fetch'
import assetLabActions from '../asset-lab-actions'
import assetConverterActions from '../../studio/actions/asset-converter-actions'
import useActions from '../../common/use-actions'
import useCurrentAccount from '../../common/use-current-account'
import {getOptimizedMeshUrl} from '../../../shared/genai/constants'
import {urlToUuid} from '../generate-request'
import type {AssetGenerationMeshMetadata, PostProcessingSettings} from '../types'
import {useSelector} from '../../hooks'

const io = new NodeIO()

const getDocLastPositionArray = async (gltf: ArrayBuffer): Promise<TypedArray | null> => {
  const document = await io.readBinary(new Uint8Array(gltf))

  // Apply rotation and scale to all meshes in the scene
  let positionArray: TypedArray | null = null
  document.getRoot().listMeshes().forEach((mesh) => {
    mesh.listPrimitives().forEach((primitive) => {
      const positions = primitive.getAttribute('POSITION')
      if (positions) {
        positionArray = positions.getArray()!
      }
    })
  })
  return positionArray
}

const applyPostProcessing = async (
  gltfExport: ArrayBuffer,
  position: Vec3Tuple,
  rotation: Vec3Tuple,
  scale: Vec3Tuple,
  settings: PostProcessingSettings
): Promise<Uint8Array> => {
  const document = await io.readBinary(new Uint8Array(gltfExport))

  // Apply rotation and scale to all meshes in the scene
  document.getRoot().listMeshes().forEach((mesh) => {
    mesh.listPrimitives().forEach((primitive) => {
      const positions = primitive.getAttribute('POSITION')
      if (!positions) {
        // No positions attribute in glb?. Nothing to transform.
        return
      }

      const positionArray = positions.getArray()!
      const transformMatrix = new THREE.Matrix4()
      // Create rotation matrix (convert degrees to radians)
      const rotMatrix = new THREE.Matrix4()
      rotMatrix.makeRotationFromEuler(new THREE.Euler(
        (rotation[0] * Math.PI) / 180,
        (rotation[1] * Math.PI) / 180,
        (rotation[2] * Math.PI) / 180
      ))

      // Create scale matrix
      const scaleMatrix = new THREE.Matrix4()
      scaleMatrix.makeScale(scale[0], scale[1], scale[2])

      // Create translation matrix for pivot offset
      const pivotMatrix = new THREE.Matrix4()
      pivotMatrix.makeTranslation(position[0], position[1], position[2])

      // Combine transformations: first apply pivot offset, then rotation and scale
      transformMatrix.multiply(pivotMatrix)
      transformMatrix.multiply(rotMatrix)
      transformMatrix.multiply(scaleMatrix)

      // Apply transformation to positions
      for (let i = 0; i < positionArray.length; i += 3) {
        const vertex = new THREE.Vector3(
          positionArray[i],
          positionArray[i + 1],
          positionArray[i + 2]
        )
        vertex.applyMatrix4(transformMatrix)
        positionArray[i] = vertex.x
        positionArray[i + 1] = vertex.y
        positionArray[i + 2] = vertex.z
      }
      positions.setArray(positionArray)

      // Also update normals if they exist
      const normals = primitive.getAttribute('NORMAL')
      if (normals) {
        const normalArray = normals.getArray()!
        const normalMatrix = new THREE.Matrix3().getNormalMatrix(transformMatrix)

        for (let i = 0; i < normalArray.length; i += 3) {
          const normal = new THREE.Vector3(
            normalArray[i],
            normalArray[i + 1],
            normalArray[i + 2]
          )
          normal.applyMatrix3(normalMatrix).normalize()
          normalArray[i] = normal.x
          normalArray[i + 1] = normal.y
          normalArray[i + 2] = normal.z
        }
        normals.setArray(normalArray)
      }
    })
  })

  // Create an array of transformations to apply
  const transforms: Transform[] = []

  // First optimize the mesh data
  if (settings.enableOptimizations) {
    transforms.push(
      prune(),
      dedup(),
      weld(),
      resample(),
      instance({min: 5}),
      flatten(),
      join(),
      palette({min: 5}),
      sparse({ratio: 0.2})
    )
  }

  // Apply texture resizing if enabled
  if (settings.enableTextureResize) {
    transforms.push(
      textureCompress({
        slots: /^(?!normalTexture).*$/,
        resize: [settings.textureSize, settings.textureSize],
      })
    )
  }

  // Apply texture brightness adjustment if enabled and not at default value
  if (settings.enableTextureBrightness && settings.textureBrightness !== 1.0) {
    transforms.push(getAdjustBrightness(settings))
  }

  // Apply PBR material adjustments if enabled
  if (settings.enablePBR) {
    transforms.push(getAdjustPbr(settings))
  }

  // Apply geometry simplification if enabled
  if (settings.enableSimplify) {
    transforms.push(
      simplify({
        simplifier: MeshoptSimplifier,
        ratio: settings.simplifyRatio,
        error: 0.01,
        lockBorder: false,
      })
    )
  }

  // Apply Draco compression last for best results
  if (settings.enableDraco) {
    transforms.push(draco())
  }

  // Apply smooth shading if enabled
  if (settings.enableSmoothShading) {
    transforms.push(smoothNormals)
  }

  // Apply all transformations
  if (transforms.length > 0) {
    await document.transform(...transforms)
  }

  return io.writeBinary(document)
}

const SETTINGS: PostProcessingSettings = {
  enableDraco: true,
  enableOptimizations: true,
  enableZip: false,
  enableSimplify: false,
  simplifyRatio: 0.5,
  enableTextureResize: true,
  textureSize: 1024,
  enableTextureBrightness: false,
  textureBrightness: 1.0,
  enablePBR: true,
  metalness: 0.0,
  roughness: 1.0,
  enableSmoothShading: true,
}

const TRELLIS_OPTIMIZATION_SETTINGS = {
  metalness: 0.25,
  roughness: 0.75,
}

const HUNYUAN_OPTIMIZATION_SETTINGS = {
  enablePBR: false,
}

const getOptimizationSettings = (model: string): PostProcessingSettings => {
  if (model.includes('trellis')) {
    return {
      ...SETTINGS,
      ...TRELLIS_OPTIMIZATION_SETTINGS,
    }
  }
  if (model.includes('hunyuan')) {
    return {
      ...SETTINGS,
      ...HUNYUAN_OPTIMIZATION_SETTINGS,
    }
  }
  return SETTINGS
}

const POSITION: Vec3Tuple = [0, 0, 0]  // Default position
const ROTATION: Vec3Tuple = [0, 0, 0]  // Default rotation
const SCALE: Vec3Tuple = [1, 1, 1]  // Default scale

type ProcessingStatus = 'unprocessed' | 'pending' | 'completed' | 'error'

const makeOptimizerMetadata = (
  settings: PostProcessingSettings,
  position: Vec3Tuple,
  rotation: Vec3Tuple,
  scale: Vec3Tuple
): AssetGenerationMeshMetadata => ({
  optimizer: {
    completed: true,
    settings,
    transform: {
      position,
      rotation,
      scale,
    },
    timestampMs: Date.now(),
  },
})

// Return a new processed mesh every time `apply` is called.
// The input mesh is loaded once and is not modified on each `apply` call.
const useGlbPostProcessing = (inputMeshUrl: string | null) => {
  const rawMeshBufferFetch = useArrayBufferFetch(inputMeshUrl)
  const [processingStatus, setProcessingStatus] = useState<ProcessingStatus>('unprocessed')

  const apply = async (
    settings: PostProcessingSettings,
    position: Vec3Tuple,
    rotation: Vec3Tuple,
    scale: Vec3Tuple
  ) => {
    if (!inputMeshUrl || !rawMeshBufferFetch || !rawMeshBufferFetch.data) {
      return null
    }
    setProcessingStatus('pending')

    try {
      const processedFile = applyPostProcessing(
        rawMeshBufferFetch.data, position, rotation, scale, settings
      )
      setProcessingStatus('completed')
      return processedFile
    } catch (error) {
      setProcessingStatus('error')
    }
    return null
  }

  return {
    readyToApply: !!inputMeshUrl && !!rawMeshBufferFetch?.data,
    apply,
    processingStatus,
    setProcessingStatus,
    gltf: rawMeshBufferFetch?.data,
  }
}

const makeGlbFile = (glb: Uint8Array): File => {
  const convertedBlob = new Blob([glb], {type: 'model/gltf-binary'})
  return new File([convertedBlob], 'optimized.glb', {type: 'model/gltf-binary'})
}

// This hook downloads the mesh, performs post-processing then reupload the optimized mesh to S3.
// This is done only once regardless of whether any settings/offset is changed. The processed mesh
// URL is returned and its data need to be fetched again for rendering.
const useRequestModelPostProcessing = (
  resultMeshUrl: string | null,
  settings: PostProcessingSettings = SETTINGS,
  position: Vec3Tuple = POSITION,
  rotation: Vec3Tuple = ROTATION,
  scale: Vec3Tuple = SCALE
) => {
  const {
    readyToApply, apply, processingStatus, setProcessingStatus,
  } = useGlbPostProcessing(resultMeshUrl)

  const [processedMeshUrl, setProcessedMeshUrl] = useState<string | null>(null)
  const {getMeshUploadSignedUrl, updateAssetGenerationMetadata} = useActions(assetLabActions)
  const {uploadFileToS3} = useActions(assetConverterActions)
  const {uuid: accountUuid} = useCurrentAccount()

  const assetGenerationUuid = urlToUuid(resultMeshUrl)
  const assetGen = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])

  // Perform post-processing on the result mesh if it exists
  useEffect(() => {
    if (!readyToApply) {
      return
    }
    const loadPromise = async () => {
      const signedUrlRes = await getMeshUploadSignedUrl(accountUuid, assetGenerationUuid)
      if (signedUrlRes.status === 'already_exists') {
        // If the file already exists, we don't need to upload it again
        setProcessedMeshUrl(getOptimizedMeshUrl(accountUuid, assetGenerationUuid,
          (assetGen?.metadata as AssetGenerationMeshMetadata)?.optimizer?.timestampMs))
        setProcessingStatus('completed')
        return
      }

      const glb = await apply(settings, position, rotation, scale)
      const processedFile = makeGlbFile(glb)
      await uploadFileToS3(processedFile, signedUrlRes.signedUrl)
      const optimizerMetadata = makeOptimizerMetadata(settings, position, rotation, scale)
      await updateAssetGenerationMetadata(assetGenerationUuid, optimizerMetadata)
      setProcessedMeshUrl(getOptimizedMeshUrl(
        accountUuid, assetGenerationUuid, optimizerMetadata?.optimizer?.timestampMs
      ))
    }

    try {
      loadPromise()
    } catch (error) {
      setProcessingStatus('error')
      setProcessedMeshUrl(null)
    }
  }, [readyToApply, settings, position, rotation, scale])

  return {
    postProcessing: processingStatus === 'pending',
    processedMeshUrl,
  }
}

export {
  POSITION,
  ROTATION,
  SCALE,
  SETTINGS,
  TRELLIS_OPTIMIZATION_SETTINGS,
  HUNYUAN_OPTIMIZATION_SETTINGS,
  getOptimizationSettings,
  makeGlbFile,
  makeOptimizerMetadata,
  useGlbPostProcessing,
  useRequestModelPostProcessing,
  getDocLastPositionArray,
}
