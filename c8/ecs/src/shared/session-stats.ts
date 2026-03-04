import type {Mesh, MeshStandardMaterial, Texture} from 'three'

import type {World} from '../runtime'
import THREE from '../runtime/three'
import type {IApplication} from './application-type'

type SessionStats = {
  triangles: number
  points: number
  drawCalls: number
  textures: number
  textureMaxSize: number
  shaders: number
  geometries: number
  entities: number
  modelSize: number
}

const calculateAdditionalWorldStats = (world: World) => {
  const entities = world.allEntities.size
  let textureMaxSize = 0
  let modelSize = 0

  const processMap = (texture: Texture | null) => {
    if (texture?.image?.width && texture?.image?.height) {
      textureMaxSize = Math.max(textureMaxSize, texture.image.width, texture.image.height)
    }
  }

  const seenMaterial = new Set<MeshStandardMaterial>()
  const processMaterial = (mat: MeshStandardMaterial) => {
    if (mat && !seenMaterial.has(mat)) {
      seenMaterial.add(mat)

      processMap(mat.map)
      processMap(mat.alphaMap)
      processMap(mat.roughnessMap)
      processMap(mat.metalnessMap)
      processMap(mat.normalMap)
      processMap(mat.emissiveMap)
    }
  }

  const processMesh = (mesh: Mesh) => {
    if (mesh.material instanceof Array) {
      mesh.material.forEach(mat => processMaterial(mat as MeshStandardMaterial))
    } else {
      processMaterial(mesh.material as MeshStandardMaterial)
    }

    if (mesh.geometry && mesh.geometry instanceof THREE.BufferGeometry) {
      Object.keys(mesh.geometry.attributes).forEach((key) => {
        const attr = mesh.geometry.attributes[key]
        modelSize += attr.array.byteLength
      })

      if (mesh.geometry.index) {
        modelSize += mesh.geometry.index.array.byteLength
      }
    }
  }

  for (const entity of world.allEntities) {
    const obj = world.three.entityToObject.get(entity)!
    obj.traverse((child) => {
      if (child instanceof THREE.Mesh) {
        processMesh(child)
      }
    })
  }

  return {entities, textureMaxSize, modelSize}
}

const retrieveCurrentSessionStats = (application: IApplication): SessionStats | null => {
  const world = application.getWorld()
  if (!world) {
    return null
  }

  const {renderer} = world.three
  const stats = renderer.info

  const {triangles} = stats.render
  const {points} = stats.render
  const drawCalls = stats.render.calls
  const {textures} = stats.memory
  const shaders = stats.programs?.length ?? 0
  const {geometries} = stats.memory

  return {
    triangles,
    points,
    drawCalls,
    textures,
    shaders,
    geometries,
    ...calculateAdditionalWorldStats(world),
  }
}

export {
  retrieveCurrentSessionStats,
}
