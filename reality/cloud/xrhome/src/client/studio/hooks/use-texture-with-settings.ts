import * as THREE from 'three'
import type {BasicMaterial, UnlitMaterial, Resource} from '@ecs/shared/scene-graph'
import {MATERIAL_DEFAULTS} from '@ecs/shared/material-constants'

import {useTexture} from './use-texture'
import {useVideoThumbnail} from './use-video-thumbnail'
import type {ConfiguredMaterial} from '../configuration/material-types'

const wrapping = {
  'clamp': THREE.ClampToEdgeWrapping,
  'repeat': THREE.RepeatWrapping,
  'mirroredRepeat': THREE.MirroredRepeatWrapping,
}

const getMinFilter = (type: 'smooth' | 'sharp', mipMap: boolean = true) => {
  if (type === 'smooth') {
    return mipMap ? THREE.LinearMipmapLinearFilter : THREE.LinearFilter
  }
  return mipMap ? THREE.NearestMipmapLinearFilter : THREE.NearestFilter
}

const getMagFilter = (
  type: 'smooth' | 'sharp'
) => (type === 'smooth' ? THREE.LinearFilter : THREE.NearestFilter)

const useTextureWithSettings = (
  material: BasicMaterial | UnlitMaterial | ConfiguredMaterial, url: string | Resource
) => {
  // NOTE(chloe): Attempt to retrieve video thumbnail first. If the provided url is not a video, it
  //  will return null, and the image texture will be used instead.
  const videoTex = useVideoThumbnail(material && url)
  const imageTex = useTexture(material && url)
  const tex = videoTex ?? imageTex
  if (tex && material) {
    const repeatX = material.repeatX ?? MATERIAL_DEFAULTS.repeatX
    const repeatY = material.repeatY ?? MATERIAL_DEFAULTS.repeatY
    const offsetX = material.offsetX ?? MATERIAL_DEFAULTS.offsetX
    const offsetY = material.offsetY ?? MATERIAL_DEFAULTS.offsetY
    const wrap = wrapping[material.wrap ?? MATERIAL_DEFAULTS.wrap]

    // NOTE(dat): Do NOT set needsUpdate to true on every render. This will results in ALL textures
    // being reuploaded in the viewport every time.
    let needsUpdate = false
    if (repeatX !== tex.repeat.x || repeatY !== tex.repeat.y) {
      tex.repeat.set(repeatX, repeatY)
      needsUpdate = true
    }
    if (offsetX !== tex.offset.x || offsetY !== tex.offset.y) {
      tex.offset.set(offsetX, offsetY)
      needsUpdate = true
    }
    if (wrap !== tex.wrapS || wrap !== tex.wrapT) {
      tex.wrapS = wrap
      tex.wrapT = wrap
      needsUpdate = true
    }

    const minFilter = getMinFilter(
      material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
      material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps
    )
    const magFilter = getMagFilter(
      material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering
    )

    if (minFilter !== tex.minFilter || magFilter !== tex.magFilter) {
      tex.minFilter = minFilter
      tex.magFilter = magFilter
      needsUpdate = true
    }

    tex.needsUpdate = needsUpdate
  }

  return tex
}

export {
  useTextureWithSettings,
}
