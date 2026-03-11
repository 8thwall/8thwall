import React, {useLayoutEffect} from 'react'
import {useTranslation} from 'react-i18next'

import * as THREE from 'three'

import {
  DEFAULT_MATERIAL_COLOR, MATERIAL_DEFAULTS, MAX_RECOMMENDED_TEXTURE_SIZE,
} from '@ecs/shared/material-constants'

import {MaterialConfigurator} from './material-configurator'
import {useTextureWithSettings} from '../hooks/use-texture-with-settings'

import {
  convertThreeMaterial, updateThreeMaterial, generateUniqueDataUrls,
  convertConfiguredMaterial, convertConfiguredTextureResource, replaceMaterial,
} from './three-material-util'
import type {ConfiguredMaterial, ConfiguredTextureResource} from './material-types'
import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {useStudioModelPreviewContext} from '../asset-previews/studio-model-preview-context'

interface IThreeMaterialConfigurator {
  threeMat: THREE.Material
  setHasChanged: (hasChanged: boolean) => void
  defaultTextures: Readonly<Record<number, string>>
  defaultOpen?: boolean
  scene: THREE.Object3D
}

const getMatType = materialType => (materialType === 'MeshStandardMaterial' ? 'basic' : 'unlit')

const createMaterial = (newType: ConfiguredMaterial['type']) => {
  if (newType === 'unlit') {
    return new THREE.MeshBasicMaterial()
  }
  return new THREE.MeshStandardMaterial()
}

const useOverrideMaterial = (
  scene: THREE.Object3D,
  currentMat: THREE.Material,
  setCurrentMaterial: (mat: THREE.Material) => void,
  matConfig: ConfiguredMaterial
) => {
  useLayoutEffect(() => {
    const currentMatType = getMatType(currentMat.type)
    if (currentMatType !== matConfig.type) {
      // note: only two types of materials are supported, a change is needed if more are added
      const newMaterial = createMaterial(matConfig.type)
      replaceMaterial(
        scene,
        currentMat as THREE.MeshStandardMaterial | THREE.MeshBasicMaterial,
        newMaterial as THREE.MeshStandardMaterial | THREE.MeshBasicMaterial
      )
      setCurrentMaterial(newMaterial)
    }
  }, [scene, matConfig.type])
}

const useOverrideProperty = (
  material: ConfiguredMaterial | undefined,
  resource: ConfiguredTextureResource | string | undefined,
  threeMat: THREE.Material, property: string
) => {
  const tex = useTextureWithSettings(material, convertConfiguredTextureResource(resource))

  useLayoutEffect(() => {
    const prevTexture = threeMat[property]
    const prevUserData = threeMat.userData
    const isDefault = typeof resource !== 'string' && resource?.type === 'default'
    // If this is a default texture, don't override the material's texture
    // TODO: might need to modify this is if we want to support undo/redo default textures
    if (isDefault) {
      // update the texture filters even if the texture is the default one
      if (tex && threeMat[property]) {
        threeMat[property].minFilter = tex.minFilter
        threeMat[property].magFilter = tex.magFilter
        threeMat[property].needsUpdate = true
      }
      return undefined
    }

    threeMat[property] = tex
    if (tex?.userData?.videoSrc) {
      // NOTE(chloe): Copy video texture's src to material's userData as texture userData will
      // be clobbered by the GLTFExporter upon save.
      if (!threeMat.userData.videos) {
        threeMat.userData.videos = {}
      }
      threeMat.userData.videos[property] = tex?.userData?.videoSrc
    } else {
      delete threeMat.userData?.videos?.[property]
    }
    threeMat.needsUpdate = true

    return () => {
      threeMat[property] = prevTexture
      threeMat.userData = prevUserData
      threeMat.needsUpdate = true
    }
  }, [threeMat, resource, tex, material.textureFiltering, material.mipmaps])
}

const validateTextureResolution = (texture: THREE.Texture | null) => {
  if (!texture || !texture.image) {
    // if a texture is not set, we don't need to check it
    return true
  }

  return !(
    texture.image.width > MAX_RECOMMENDED_TEXTURE_SIZE ||
    texture.image.height > MAX_RECOMMENDED_TEXTURE_SIZE
  )
}

const ThreeMaterialConfigurator: React.FC<IThreeMaterialConfigurator> = (
  {threeMat, setHasChanged, defaultTextures, defaultOpen, scene}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])

  const [currentMaterial, setCurrentMaterial] = React.useState<THREE.Material>(threeMat)
  // React needs to know when the material has changed (or the UI won't refresh)
  const [configuredMat, setConfiguredMat] = React.useState<ConfiguredMaterial>(
    convertThreeMaterial(threeMat, defaultTextures)
  )

  React.useEffect(() => {
    const matType = getMatType(currentMaterial.type)
    if (matType === configuredMat.type) {
      updateThreeMaterial(currentMaterial, convertConfiguredMaterial(configuredMat))
    }
  }, [configuredMat, currentMaterial])

  useOverrideMaterial(scene, currentMaterial, setCurrentMaterial, configuredMat)
  useOverrideProperty(configuredMat, configuredMat?.textureSrc, threeMat, 'map')
  useOverrideProperty(configuredMat, configuredMat?.opacityMap, threeMat, 'alphaMap')
  useOverrideProperty(configuredMat, configuredMat?.roughnessMap, threeMat, 'roughnessMap')
  useOverrideProperty(configuredMat, configuredMat?.metalnessMap, threeMat, 'metalnessMap')
  useOverrideProperty(configuredMat, configuredMat?.normalMap, threeMat, 'normalMap')
  useOverrideProperty(configuredMat, configuredMat?.emissiveMap, threeMat, 'emissiveMap')
  const sceneGraphMaterial = convertConfiguredMaterial(configuredMat)

  const matName = threeMat.name ||
    t('asset_configurator.asset_material_configurator.unnamed_material')

  return (
    <MaterialConfigurator
      title={matName}
      material={sceneGraphMaterial}
      defaultOpen={defaultOpen}
      isAsset
      onChange={(e) => {
        // Keep a reference to the new material for setting the three material
        // (React state doesn't always update immediately)
        const newMat = e(sceneGraphMaterial) as ConfiguredMaterial
        if (newMat.type !== configuredMat.type) {
          // set a new configuredMaterial based on shared values between material types.
          setConfiguredMat({
            type: newMat.type,
            color: newMat.color ?? DEFAULT_MATERIAL_COLOR,
            // We make sure the previous texture is an Asset or a Default resource
            textureSrc: typeof newMat.textureSrc === 'string'
              ? {type: 'default', dataUrl: newMat.textureSrc}
              : newMat.textureSrc,
            opacity: newMat.opacity ?? MATERIAL_DEFAULTS.opacity,
            blending: newMat.blending,
            side: newMat.side,
            textureFiltering: newMat.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
            mipmaps: newMat.mipmaps ?? MATERIAL_DEFAULTS.mipmaps,
            wrap: newMat.wrap,
            repeatX: newMat.repeatX,
            repeatY: newMat.repeatY,
            offsetX: newMat.offsetX,
            offsetY: newMat.offsetY,
            forceTransparent: newMat.forceTransparent,
            // Set Basic properties if changing from Unlit to Basic
            roughness: newMat.roughness ?? configuredMat.roughness,
            roughnessMap: convertConfiguredTextureResource(
              newMat.roughnessMap ?? configuredMat.roughnessMap
            ),
            metalness: newMat.metalness ?? configuredMat.metalness,
            metalnessMap: convertConfiguredTextureResource(
              newMat.metalnessMap ?? configuredMat.metalnessMap
            ),
            normalScale: newMat.normalScale ?? configuredMat.normalScale,
            normalMap: convertConfiguredTextureResource(
              newMat.normalMap ?? configuredMat.normalMap
            ),
            emissiveIntensity: newMat.emissiveIntensity ?? configuredMat.emissiveIntensity,
            emissiveColor: newMat.emissiveColor ?? configuredMat.emissiveColor,
            emissiveMap: convertConfiguredTextureResource(
              newMat.emissiveMap ?? configuredMat.emissiveMap
            ),
          })
          setHasChanged(true)
          return
        }
        if (newMat.type === 'basic' || newMat.type === 'unlit') {
          Object.entries(newMat).forEach(([key, value]) => {
            if (value === sceneGraphMaterial[key]) {
              newMat[key] = configuredMat[key]
            }
          })
        }
        setConfiguredMat(newMat)
        setHasChanged(true)
      }}
    />
  )
}

interface IAssetMaterialsConfigurator {
  previewScene: THREE.Object3D
  materialsArray: THREE.Material[]
  setHasChanged: (hasChanged: boolean) => void
}

const AssetMaterialsConfigurator: React.FC<IAssetMaterialsConfigurator> = (
  {previewScene, materialsArray, setHasChanged}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const {setShowTextureSizeWarning} = useStudioModelPreviewContext()

  const textures = React.useMemo(() => (
    generateUniqueDataUrls(materialsArray)), [materialsArray])

  React.useEffect(() => {
    // NOTE(alan): This can't be moved into a useMemo for some weird reason related to load order
    // of the gltf scene.

    for (let i = 0; i < materialsArray.length; i++) {
      const material = materialsArray[i] as any
      const warning =
        !validateTextureResolution(material.map) ||
        !validateTextureResolution(material.alphaMap) ||
        !validateTextureResolution(material.roughnessMap) ||
        !validateTextureResolution(material.metalnessMap) ||
        !validateTextureResolution(material.normalMap) ||
        !validateTextureResolution(material.emissiveMap)

      setShowTextureSizeWarning(warning)

      // No need to check the rest of the textures if one is already too big
      if (warning) {
        break
      }
    }

    return () => {
      setShowTextureSizeWarning(false)
    }
  }, [materialsArray])

  if (materialsArray.length === 0) {
    return null
  }

  return (
    <FloatingTraySection
      title={t('asset_configurator.asset_material_configurator.title')}
      borderBottom={false}
    >
      {materialsArray.map((mat, i) => (
        <ThreeMaterialConfigurator
          key={mat.uuid}
          threeMat={mat}
          setHasChanged={changed => setHasChanged(changed)}
          defaultTextures={textures}
          defaultOpen={i === 0}
          scene={previewScene}
        />
      ))}
    </FloatingTraySection>
  )
}

export {
  AssetMaterialsConfigurator,
}
