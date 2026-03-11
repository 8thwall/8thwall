import * as THREE from 'three'

import type {
  Side, MaterialBlending, Material, Resource,
} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {MATERIAL_DEFAULTS} from '@ecs/shared/material-constants'

import {MATERIAL_BLENDING, MATERIAL_SIDE} from './material-constants'
import type {ConfiguredMaterial, ConfiguredTextureResource} from './material-types'

const convertThreeColor = (threeColor: THREE.Color): string => `#${threeColor.getHexString()}`

const convertThreeSide = (threeSide: THREE.Side): Side => {
  if (threeSide === THREE.FrontSide) {
    return 'front'
  } else if (threeSide === THREE.BackSide) {
    return 'back'
  } else {
    return 'double'
  }
}

const convertThreeBlending = (threeBlending: THREE.Blending): MaterialBlending => {
  if (threeBlending === THREE.NormalBlending) {
    return 'normal'
  } else if (threeBlending === THREE.AdditiveBlending) {
    return 'additive'
  } else if (threeBlending === THREE.SubtractiveBlending) {
    return 'subtractive'
  } else if (threeBlending === THREE.MultiplyBlending) {
    return 'multiply'
  } else {
    return 'no'
  }
}

const convertThreeFiltersToStudio = (
  texture: THREE.Texture | undefined
) => {
  if (!texture) {
    return {textureFiltering: 'smooth' as const, mipmaps: true}
  }
  const {minFilter, magFilter} = texture

  const isSharp = magFilter === THREE.NearestFilter

  const mipmaps = minFilter === THREE.NearestMipmapLinearFilter ||
    minFilter === THREE.LinearMipmapLinearFilter ||
    minFilter === THREE.NearestMipmapNearestFilter ||
    minFilter === THREE.LinearMipmapNearestFilter

  return {textureFiltering: isSharp ? 'sharp' as const : 'smooth' as const, mipmaps}
}

// Returns a map of the texture type and the texture id
const mapMaterialTextureToId = (threeMat: THREE.Material): Record<string, number> => {
  if (threeMat instanceof THREE.MeshStandardMaterial) {
    return {
      textureSrc: threeMat.map?.id,
      opacityMap: threeMat.alphaMap?.id,
      roughnessMap: threeMat.roughnessMap?.id,
      metalnessMap: threeMat.metalnessMap?.id,
      normalMap: threeMat.normalMap?.id,
      emissiveMap: threeMat.emissiveMap?.id,
    }
  } else if (threeMat instanceof THREE.MeshBasicMaterial) {
    return {
      textureSrc: threeMat.map?.id,
    }
  }
  return {}
}

const convertThreeMaterial = (
  threeMat: THREE.Material, defaultMaterials = {}
): ConfiguredMaterial => {
  // helper for converting a dataUrl to a default resource
  const defaultRes = (dataUrl: string): ConfiguredTextureResource => {
    if (dataUrl) {
      return {type: 'default', dataUrl}
    }
    return undefined
  }

  const textureIds = mapMaterialTextureToId(threeMat)
  if (threeMat instanceof THREE.MeshStandardMaterial) {
    const {textureFiltering, mipmaps} = convertThreeFiltersToStudio(threeMat.map)

    return {
      type: 'basic',
      color: convertThreeColor(threeMat.color),
      roughness: threeMat.roughness,
      metalness: threeMat.metalness,
      normalScale: threeMat.normalScale.x ?? 1,
      opacity: threeMat.opacity,
      emissiveColor: convertThreeColor(threeMat.emissive),
      emissiveIntensity: threeMat.emissiveIntensity,
      side: convertThreeSide(threeMat.side),
      blending: convertThreeBlending(threeMat.blending),
      depthTest: threeMat.depthTest,
      depthWrite: threeMat.depthWrite,
      wireframe: threeMat.wireframe,
      textureSrc: defaultRes(defaultMaterials[textureIds.textureSrc]),
      opacityMap: defaultRes(defaultMaterials[textureIds.opacityMap]),
      roughnessMap: defaultRes(defaultMaterials[textureIds.roughnessMap]),
      metalnessMap: defaultRes(defaultMaterials[textureIds.metalnessMap]),
      normalMap: defaultRes(defaultMaterials[textureIds.normalMap]),
      emissiveMap: defaultRes(defaultMaterials[textureIds.emissiveMap]),
      // forceTransparent will be ignored if transparency set via other means, so safe to set it
      // direct from material.transparent:
      forceTransparent: threeMat.transparent,
      textureFiltering,
      mipmaps,
    }
  } else if (threeMat instanceof THREE.MeshBasicMaterial) {
    const {textureFiltering, mipmaps} = convertThreeFiltersToStudio(threeMat.map)

    return {
      type: 'unlit',
      color: convertThreeColor(threeMat.color),
      textureSrc: defaultRes(defaultMaterials[textureIds.textureSrc]),
      opacity: threeMat.opacity,
      side: convertThreeSide(threeMat.side),
      blending: convertThreeBlending(threeMat.blending),
      textureFiltering,
      mipmaps,
    }
  }

  return {
    color: '#ffffff',
  }
}

// Convert a configured texture resource to a regular resource
// (Strip out the 'default' type)
const convertConfiguredTextureResource = (
  resource: ConfiguredTextureResource | string | undefined
): Resource | string | undefined => {
  if (typeof resource === 'string' || resource?.type !== 'default') {
    return resource
  }

  if (resource?.type === 'default') {
    return resource.dataUrl
  }

  return undefined
}

// Convert a configured material to a regular material
const convertConfiguredMaterial = (material: ConfiguredMaterial): Material => {
  if (material.type === 'basic') {
    return {
      ...material,
      type: 'basic',
      color: material?.color || '0xffffff',
      textureSrc: convertConfiguredTextureResource(material.textureSrc),
      roughnessMap: convertConfiguredTextureResource(material.roughnessMap),
      metalnessMap: convertConfiguredTextureResource(material.metalnessMap),
      opacityMap: convertConfiguredTextureResource(material.opacityMap),
      normalMap: convertConfiguredTextureResource(material.normalMap),
      emissiveMap: convertConfiguredTextureResource(material.emissiveMap),
      textureFiltering: material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
      mipmaps: material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps,
    }
  } else if (material.type === 'unlit') {
    return {
      type: 'unlit',
      color: material?.color || '0xffffff',
      textureSrc: convertConfiguredTextureResource(material.textureSrc),
      opacity: material.opacity ?? 1,
      side: material.side ?? 'front',
      blending: material.blending ?? 'normal',
      textureFiltering: material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
      mipmaps: material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps,
      repeatX: material?.repeatX,
      repeatY: material?.repeatY,
      offsetX: material?.offsetX,
      offsetY: material?.offsetY,
      wrap: material?.wrap,
      forceTransparent: material?.forceTransparent,
    }
  }

  return undefined
}

const updateThreeMaterial = (threeMat: THREE.Material, material: DeepReadonly<Material>) => {
  if (threeMat instanceof THREE.MeshStandardMaterial && material.type === 'basic') {
    if (material.color !== undefined) {
      threeMat.color.set(material.color)
    }
    threeMat.roughness = material.roughness ?? threeMat.roughness
    threeMat.metalness = material.metalness ?? threeMat.metalness

    // note(alan): gltf is imported as (normalScale, -normalScale), we are following the same
    // pattern to avoid flipping the normal map normals
    if (material.normalScale !== undefined) {
      threeMat.normalScale.set(material.normalScale, -material.normalScale)
    } else {
      threeMat.normalScale = new THREE.Vector2(material.normalScale, -material.normalScale)
    }

    if (material.opacity !== undefined) {
      threeMat.opacity = material.opacity

      const prevTransparent = threeMat.transparent
      threeMat.transparent =
        material.opacity < 1 || !!material.opacityMap || !!material.forceTransparent

      // Transparency changes require the material be recompiled
      threeMat.needsUpdate = prevTransparent !== threeMat.transparent
    }

    if (material.emissiveColor !== undefined) {
      threeMat.emissive.set(material.emissiveColor)
    }
    threeMat.emissiveIntensity = material.emissiveIntensity ?? threeMat.emissiveIntensity
    threeMat.side = MATERIAL_SIDE[material.side] ?? threeMat.side
    threeMat.blending = MATERIAL_BLENDING[material.blending] ?? threeMat.blending
    threeMat.depthTest = material.depthTest ?? threeMat.depthTest
    threeMat.depthWrite = material.depthWrite ?? threeMat.depthWrite
    threeMat.wireframe = material.wireframe ?? threeMat.wireframe
    threeMat.needsUpdate = true
    return
  }

  if (threeMat instanceof THREE.MeshBasicMaterial && material.type === 'unlit') {
    if (material.color !== undefined) {
      threeMat.color.set(material.color)
    }
    threeMat.opacity = material.opacity ?? threeMat.opacity
    threeMat.transparent = material.opacity < 1
    threeMat.side = MATERIAL_SIDE[material.side] ?? threeMat.side
    threeMat.blending = MATERIAL_BLENDING[material.blending] ?? threeMat.blending
    threeMat.needsUpdate = true
    return
  }
  // eslint-disable-next-line no-console
  console.error(`Unsupported material type: ${material.type}`)
}

const findMaterialsInThreeScene = (scene: THREE.Object3D) => {
  const materials = new Set<THREE.Material>()

  scene?.traverse((node) => {
    if (node instanceof THREE.Mesh) {
      if (node.material instanceof Array) {
        node.material.forEach(m => materials.add(m))
      } else {
        materials.add(node.material)
      }
    }
  })

  return materials
}

type textureToDataURI = Record<number, string>
// Takes a map of string:ImageBitmap and parses it to a Data URI set.
// See https://developer.mozilla.org/en-US/docs/Web/URI/Schemes/data.
const generateUniqueDataUrls = (
  materialArray: THREE.Material[]
): textureToDataURI => {
  if (!materialArray.length) {
    return []
  }
  const dataUrls: textureToDataURI = {}
  const canvas = document.createElement('canvas')
  const ctx = canvas.getContext('2d')
  const sizeFactor = 0.1

  const parseTexture = (texture: THREE.Texture | undefined) => {
    if (!texture || !texture.image || dataUrls[texture.id]) {
      return
    }
    canvas.width = texture.image.width * sizeFactor
    canvas.height = texture.image.height * sizeFactor
    ctx.drawImage(texture.image, 0, 0, canvas.width, canvas.height)
    dataUrls[texture.id] = canvas.toDataURL()
  }

  materialArray.forEach((material) => {
    if (material instanceof THREE.MeshStandardMaterial) {
      const {map, alphaMap, roughnessMap, metalnessMap, normalMap, emissiveMap} = material
      parseTexture(map)
      parseTexture(alphaMap)
      parseTexture(roughnessMap)
      parseTexture(metalnessMap)
      parseTexture(normalMap)
      parseTexture(emissiveMap)
    } else if (material instanceof THREE.MeshBasicMaterial) {
      const {map} = material
      parseTexture(map)
    }
  })

  canvas.remove()

  return dataUrls
}

const replaceMaterial = (
  scene: THREE.Object3D,
  currentMaterial: THREE.MeshStandardMaterial | THREE.MeshBasicMaterial,
  newMaterial: THREE.MeshStandardMaterial | THREE.MeshBasicMaterial
): THREE.Material => {
  newMaterial.name = currentMaterial.name
  newMaterial.color = new THREE.Color(currentMaterial.color || 'rgb(255, 255, 255)')
  newMaterial.map = currentMaterial.map ? new THREE.Texture().copy(currentMaterial.map) : undefined
  newMaterial.opacity = currentMaterial.opacity
  newMaterial.transparent = currentMaterial.transparent
  newMaterial.needsUpdate = true

  scene.traverse((node) => {
    if (node instanceof THREE.Mesh && node.material === currentMaterial) {
      node.material = newMaterial
    }
  })

  return newMaterial
}

export {
  convertThreeMaterial,
  convertConfiguredTextureResource,
  updateThreeMaterial,
  findMaterialsInThreeScene,
  convertConfiguredMaterial,
  mapMaterialTextureToId,
  generateUniqueDataUrls,
  replaceMaterial,
}
