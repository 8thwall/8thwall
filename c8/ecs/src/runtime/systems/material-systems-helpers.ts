import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import type {TextureWrap} from '../../shared/scene-graph'
import {assets} from '../assets'
import {wrapping} from '../three-wrapping'
import {createVideoTexture} from '../video-texture'
import {SUPPORTED_VIDEO_FORMATS} from '../../shared/video-constants'
import {disposeTexture, maybeDisposeMaterial} from '../dispose'
import {getVideoManager} from '../video-manager'

const getMinFilter = (key: 'smooth' | 'sharp', mipMap: boolean = true) => {
  if (key === 'smooth') {
    return mipMap ? THREE.LinearMipmapLinearFilter : THREE.LinearFilter
  } else {
    return mipMap ? THREE.NearestMipmapLinearFilter : THREE.NearestFilter
  }
}

const replaceUnsupportedVideoFilter = (filter: THREE_TYPES.MinificationTextureFilter) => {
  if (filter === THREE.NearestMipmapLinearFilter || filter === THREE.NearestFilter) {
    return THREE.NearestFilter
  } else {
    return THREE.LinearFilter
  }
}

const getMagFilter = (key: 'smooth' | 'sharp') => (
  key === 'smooth' ? THREE.LinearFilter : THREE.NearestFilter
)

const setMaterialColor = (color: THREE_TYPES.Color, r: number, g: number, b: number) => {
  color.setRGB(
    r / 255,
    g / 255,
    b / 255
  )
  color.convertSRGBToLinear()
}

const invisibleMaterial = new THREE.MeshBasicMaterial()
invisibleMaterial.userData.disposable = true

const removeMaterial = (eid: Eid, world: World) => {
  const object = world.three.entityToObject.get(eid)
  if (object instanceof THREE.Mesh) {
    maybeDisposeMaterial(object.material)
    // Clear out the userData to stop any texture loading
    object.material.userData = {}
    // TODO(christoph): Technically switching from one material to another would
    // be clobbered here
    object.material = invisibleMaterial
  }
}

type TextureId = string

// TextureProperty will be the union of all properties of T that are of type THREE.Texture|null
type TextureProperty<T> = {
  [K in keyof T]: T[K] extends (THREE_TYPES.Texture | null) ? K : never
}[keyof T]

const textureLoader = new THREE.TextureLoader()

const MATERIAL_BLENDING = {
  'no': THREE.NoBlending,
  'normal': THREE.NormalBlending,
  'additive': THREE.AdditiveBlending,
  'subtractive': THREE.SubtractiveBlending,
  'multiply': THREE.MultiplyBlending,
} as const

const loadTexture = <T extends THREE_TYPES.Material>(
  material: T, src: string, property: TextureProperty<T>,
  repeatX: number, repeatY: number, offsetX: number, offsetY: number, wrap: string,
  minificationFilter: THREE_TYPES.MinificationTextureFilter,
  magnificationFilter: THREE_TYPES.MagnificationTextureFilter,
  colorSpace?: THREE_TYPES.ColorSpace
): Promise<THREE_TYPES.Texture | null> => new Promise((resolve) => {
    if (material.userData[property as string] === src) {
      const texture = material[property] as THREE_TYPES.Texture
      if (!texture) {
        resolve(null)
        return
      }
      let needUpdate = false
      if (texture.repeat) {
        texture.repeat.set(repeatX, repeatY)
        needUpdate = true
      }
      if (texture.offset) {
        texture.offset.set(offsetX, offsetY)
        needUpdate = true
      }
      const textureWrap = wrapping[wrap as TextureWrap]
      if (textureWrap) {
        texture.wrapS = textureWrap
        texture.wrapT = textureWrap
        needUpdate = true
      }

      const correctedMinFilter = texture instanceof THREE.VideoTexture
        ? replaceUnsupportedVideoFilter(minificationFilter)
        : minificationFilter

      if (texture.minFilter !== correctedMinFilter) {
        texture.minFilter = correctedMinFilter
        needUpdate = true
      }

      if (texture.magFilter !== magnificationFilter) {
        texture.magFilter = magnificationFilter
        needUpdate = true
      }

      texture.needsUpdate = needUpdate
      resolve(null)
      return
    }

    material.userData[property as string] = src
    if (src) {
      assets.load({url: src}).then((asset) => {
        // If the material has changed since the texture was requested or is being removed,
        // don't apply it
        if (material.userData[property as string] !== src) {
          resolve(null)
          return
        }

        // Clean-up existing texture
        disposeTexture(material[property] as THREE_TYPES.Texture)

        const isVideo = SUPPORTED_VIDEO_FORMATS.includes(asset.data.type)
        const texture = isVideo
          ? createVideoTexture(asset.localUrl)
          : textureLoader.load(asset.localUrl)
        texture.userData.src = src
        texture.userData.textureKey = property
        texture.repeat.set(repeatX, repeatY)
        texture.offset.set(offsetX, offsetY)
        const textureWrap = wrapping[wrap as TextureWrap]
        texture.wrapS = textureWrap
        texture.wrapT = textureWrap
        // NOTE(chloe): Video textures don't play nice with mipmap filtering, so we disable them.
        texture.minFilter = isVideo
          ? replaceUnsupportedVideoFilter(minificationFilter)
          : minificationFilter
        texture.magFilter = magnificationFilter
        // ts couldn't resolve the type of material[property] although it always is a texture
        material[property] = texture as any
        material.needsUpdate = true
        if (colorSpace) {
          texture.colorSpace = colorSpace
        }
        resolve(texture)
      })
    } else {
      disposeTexture(material[property] as THREE_TYPES.Texture)
      material[property] = null as any
      material.needsUpdate = true
      resolve(null)
    }
  })

// Maybe remove material's texture from Video Manager if it holds a video element and if the given
// src for this texture map is now stale.
const maybeRemoveStaleFromVideoManager = <T extends THREE_TYPES.Material>(
  world: World, eid: Eid, material: T, property: TextureProperty<T>, src: string,
  videos: Map<Eid, Set<TextureId>>
) => {
  const texture = material[property] as THREE_TYPES.Texture
  const isVideoTexture = texture && texture instanceof THREE.VideoTexture

  if (isVideoTexture && material.userData[property as string] !== src) {
    getVideoManager(world).maybeRemoveVideo(eid, texture.uuid)
    videos.get(eid)?.delete(texture.uuid)
  }
}

export type {
  TextureId,
}

export {
  setMaterialColor,
  loadTexture,
  removeMaterial,
  MATERIAL_BLENDING,
  getMagFilter,
  getMinFilter,
  maybeRemoveStaleFromVideoManager,
}
