import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {UnlitMaterial} from '../components'
import {makeSystemHelper} from './system-helper'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import {
  removeMaterial, loadTexture, setMaterialColor, type TextureId, MATERIAL_BLENDING,
  maybeRemoveStaleFromVideoManager,
  getMinFilter, getMagFilter,
} from './material-systems-helpers'
import type {MaterialBlending, TextureFiltering} from '../../shared/scene-graph'
import {getVideoManager} from '../video-manager'
import {sides} from '../three-side'

const makeUnlitMaterialSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(UnlitMaterial)
  const videos = new Map<Eid, Set<TextureId>>()
  const abortControllers = new Map<Eid, AbortController>()

  const applyMaterial = (eid: Eid) => {
    abortControllers.get(eid)?.abort()
    const abortController = new AbortController()
    abortControllers.set(eid, abortController)

    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh) {
      let material: THREE_TYPES.MeshBasicMaterial
      if (object.material?.userData.unlit) {
        material = object.material
      } else {
        material = new THREE.MeshBasicMaterial()
        material.userData.disposable = true
        material.userData.unlit = true
        object.material = material
      }

      const {
        r, g, b, textureSrc, opacity, side, blending, opacityMap,
        repeatX, repeatY, offsetX, offsetY, wrap, depthTest, depthWrite, wireframe,
        forceTransparent, textureFiltering, mipmaps,
      } = UnlitMaterial.get(world, eid)

      const threeSide = sides[side as 'front' | 'back' | 'double']

      setMaterialColor(material.color, r, g, b)
      material.side = threeSide
      material.transparent = opacity < 1 || !!opacityMap || forceTransparent
      material.opacity = opacity
      material.blending = MATERIAL_BLENDING[blending as MaterialBlending] ?? THREE.NormalBlending
      const minFilter = getMinFilter(textureFiltering as TextureFiltering, mipmaps)
      const magFilter = getMagFilter(textureFiltering as TextureFiltering)

      material.wireframe = wireframe
      material.depthTest = depthTest
      material.depthWrite = depthWrite
      material.visible = true

      maybeRemoveStaleFromVideoManager(world, eid, material, 'map', textureSrc, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'alphaMap', opacityMap, videos)

      Promise.all([
        loadTexture(material, textureSrc, 'map', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, THREE.SRGBColorSpace),
        loadTexture(material, opacityMap, 'alphaMap', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, undefined),
      ]).then((textures) => {
        if (abortController.signal.aborted) {
          return
        }

        const videoTextures = textures.filter(
          (texture): texture is THREE_TYPES.VideoTexture => texture instanceof THREE.VideoTexture
        )
        getVideoManager(world).addVideo(eid, videoTextures)

        if (videoTextures.length) {
          const existingVideos = videos.get(eid) || new Set<TextureId>()
          videoTextures.forEach(texture => existingVideos.add(texture.uuid))
          videos.set(eid, existingVideos)
        }
      })
    }
  }

  const handleMaterialRemoval = (eid: Eid) => {
    abortControllers.get(eid)?.abort()
    abortControllers.delete(eid)

    const textures = videos.get(eid)
    textures?.forEach((textureId) => {
      getVideoManager(world).maybeRemoveVideo(eid, textureId)
    })
    videos.delete(eid)
    removeMaterial(eid, world)
  }

  return () => {
    exit(world).forEach(handleMaterialRemoval)
    enter(world).forEach(applyMaterial)
    changed(world).forEach(applyMaterial)
  }
}

export {
  makeUnlitMaterialSystem,
}
