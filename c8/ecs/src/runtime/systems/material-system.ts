import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Material} from '../components'
import {makeSystemHelper} from './system-helper'
import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import type {MaterialBlending, TextureFiltering} from '../../shared/scene-graph'
import {sides} from '../three-side'
import {
  type TextureId, setMaterialColor, MATERIAL_BLENDING, maybeRemoveStaleFromVideoManager,
  loadTexture, removeMaterial, getMinFilter, getMagFilter,
} from './material-systems-helpers'
import {getVideoManager} from '../video-manager'

const makeMaterialSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Material)
  const videos = new Map<Eid, Set<TextureId>>()
  const abortControllers = new Map<Eid, AbortController>()

  const applyMaterial = (eid: Eid) => {
    abortControllers.get(eid)?.abort()
    const abortController = new AbortController()
    abortControllers.set(eid, abortController)

    const object = world.three.entityToObject.get(eid)
    if (object instanceof THREE.Mesh) {
      let material: THREE_TYPES.MeshStandardMaterial
      if (object.material instanceof THREE.MeshPhysicalMaterial) {
        material = object.material
      } else {
        material = new THREE.MeshPhysicalMaterial()
        material.userData.disposable = true
        object.material = material
      }

      const {
        r, g, b, textureSrc, roughness, metalness, opacity, roughnessMap, metalnessMap, side,
        normalScale, emissiveIntensity, emissiveR, emissiveG, emissiveB, opacityMap, normalMap,
        emissiveMap, blending, repeatX, repeatY, offsetX, offsetY, wrap,
        depthTest, depthWrite, wireframe, forceTransparent, textureFiltering, mipmaps,
      } = Material.get(world, eid)

      const threeSide = sides[side as 'front' | 'back' | 'double']

      setMaterialColor(material.color, r, g, b)
      material.side = threeSide
      material.transparent = opacity < 1 || !!opacityMap || forceTransparent
      material.opacity = opacity
      material.roughness = roughness
      material.metalness = metalness
      material.normalScale.set(normalScale, normalScale)
      material.emissiveIntensity = emissiveIntensity
      setMaterialColor(material.emissive, emissiveR, emissiveG, emissiveB)
      material.blending = MATERIAL_BLENDING[blending as MaterialBlending] ?? THREE.NormalBlending
      material.wireframe = wireframe
      material.depthTest = depthTest
      material.depthWrite = depthWrite

      maybeRemoveStaleFromVideoManager(world, eid, material, 'map', textureSrc, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'roughnessMap', roughnessMap, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'metalnessMap', metalnessMap, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'normalMap', normalMap, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'alphaMap', opacityMap, videos)
      maybeRemoveStaleFromVideoManager(world, eid, material, 'emissiveMap', emissiveMap, videos)
      const magFilter = getMagFilter(textureFiltering as TextureFiltering)
      const minFilter = getMinFilter(textureFiltering as TextureFiltering, mipmaps)
      Promise.all([
        loadTexture(material, textureSrc, 'map', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, THREE.SRGBColorSpace),
        loadTexture(material, roughnessMap, 'roughnessMap', repeatX, repeatY, offsetX, offsetY,
          wrap, minFilter, magFilter, undefined),
        loadTexture(material, metalnessMap, 'metalnessMap', repeatX, repeatY, offsetX, offsetY,
          wrap, minFilter, magFilter, undefined),
        loadTexture(material, normalMap, 'normalMap', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, undefined),
        loadTexture(material, opacityMap, 'alphaMap', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, undefined),
        loadTexture(material, emissiveMap, 'emissiveMap', repeatX, repeatY, offsetX, offsetY, wrap,
          minFilter, magFilter, THREE.SRGBColorSpace),
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
  makeMaterialSystem,
}
