import {InstancedMesh, Material, Mesh, Object3D, SkinnedMesh, Texture} from 'three'

import {GLTFExporter} from 'three/examples/jsm/exporters/GLTFExporter'
import {Document, JSONDocument, NodeIO} from '@gltf-transform/core'
import {KHRONOS_EXTENSIONS} from '@gltf-transform/extensions'

import type {ModelInfo} from '../../editor/asset-preview-types'

const traverseTwo = (
  object1: Object3D,
  object2: Object3D,
  callback: (object1: Object3D, object2: Object3D) => void
) => {
  if (!object1 || !object2) {
    // eslint-disable-next-line no-console
    console.error('One of the objects in scene is null or undefined')
    return
  }

  try {
    callback(object1, object2)
  } catch (error) {
    // eslint-disable-next-line no-console
    console.error('Error during traverseTwo callback execution: ', error)
    return
  }

  const hasSameChildren = object1.children.length === object2.children.length
  for (let i = 0; i < object1.children.length; i++) {
    if (hasSameChildren) {
      traverseTwo(object1.children[i], object2.children[i], callback)
    } else {
      // Continue traversing the objects if we find a matching child with same name
      const object2Child = object2.children.find(child => child.name === object1.children[i].name)
      if (object2Child) {
        traverseTwo(object1.children[i], object2Child, callback)
      }
    }
  }
}

const containsTexture = (scene: Object3D) => {
  let hasTexture = false
  scene.traverse((object: Object3D) => {
    if (object instanceof Mesh) {
      const materials = Array.isArray(object.material) ? object.material : [object.material]
      materials.forEach((material: Material) => {
        Object.keys(material).forEach((key) => {
          if (material[key] instanceof Texture) {
            hasTexture = true
          }
        })
      })
    }
  })
  return hasTexture
}

// Assumes oldScene and newScene have the same tree structure
function replaceMaterial(sceneToChange: Object3D, sceneToClone: Object3D) {
  traverseTwo(sceneToChange, sceneToClone, (object1, object2) => {
    if (object1 instanceof Mesh && object2 instanceof Mesh) {
      object1.material = object2.material.clone()
      object1.material.userData = {...object2.material.userData}
      object1.material.needsUpdate = true
    }
  })
}

const PROPERTIES_THAT_ARE_TEXTURES = [
  'alphaMap',
  'anisotrophyMap',
  'aoMap',
  'bumpMap',
  'clearcoatMap',
  'clearcoatRoughnessMap',
  'displacementMap',
  'emmissiveMap',
  'envMap',
  'iridescenceMap',
  'iridescenceThicknessMap',
  'lightMap',
  'map',
  'normalMap',
  'roughnessMap',
  'sheenColorMap',
  'sheenRoughnessMap',
  'specularColorMap',
  'specularIntensityMap',
  'specularMap',
  'thicknessMap',
  'transmissionMap',
]
// Goes through the object3D and sums up the total number of geometry points e.g. vertices, ...
const getObject3DInfo = (obj: Object3D): ModelInfo => {
  const info: ModelInfo = {
    vertices: 0,
    triangles: 0,
    maxTextureSize: 0,
  }

  const {children} = obj

  if (obj instanceof Mesh || obj instanceof SkinnedMesh || obj instanceof InstancedMesh) {
    if (obj.geometry.attributes.position) {
      info.vertices = obj.geometry.attributes.position.count
    }
    if (obj.geometry.index) {
      info.triangles = obj.geometry.index.count / 3
    }
    const materials = Array.isArray(obj.material) ? obj.material : [obj.material]
    materials.forEach((material) => {
      PROPERTIES_THAT_ARE_TEXTURES.forEach((property) => {
        if (material[property] && material[property].image) {
          const maxSize = Math.max(material[property].image.width, material[property].image.height)
          info.maxTextureSize = Math.max(info.maxTextureSize, maxSize)
        }
      })
      if (material.map && material.map.image) {
        info.maxTextureSize = Math.max(
          info.maxTextureSize, material.map.image.width, material.map.image.height
        )
      }
    })
  }

  children.forEach((child) => {
    const childInfo = getObject3DInfo(child)
    info.vertices += childInfo.vertices
    info.triangles += childInfo.triangles
    info.maxTextureSize = Math.max(info.maxTextureSize, childInfo.maxTextureSize)
  })

  info.isDraco = !!obj.userData?.draco
  return info
}

const replaceMesh = (sceneToChange: Object3D, sceneToClone: Object3D) => {
  traverseTwo(sceneToChange, sceneToClone, (object1, object2) => {
    if (object1 instanceof Mesh && object2 instanceof Mesh) {
      object1.geometry = object2.geometry.clone()
      object1.geometry.needsUpdate = true
    }
  })
}

const getGltfTransformDocument = (
  sceneToExport: Object3D, options: Object = {binary: true}
) => new Promise<Document>((
  resolve, reject
) => {
  const exporter = new GLTFExporter()
  exporter.parse(sceneToExport, async (result) => {
    const io = new NodeIO().registerExtensions(KHRONOS_EXTENSIONS)
    if (result instanceof ArrayBuffer) {
      // GLB is exported as binary
      const binToExport = new Uint8Array(result)
      const glbDocument = await io.readBinary(binToExport)
      if (!glbDocument) {
        throw new Error('Failed to transform ThreeJS glb to gltf-transform glb')
      }
      resolve(glbDocument)
    } else {
      // GLTF is exported as JSON
      const glbDocument = await io.readJSON(result as JSONDocument)
      if (!glbDocument) {
        throw new Error('Failed to transform ThreeJS glb to gltf-transform glb')
      }
      resolve(glbDocument)
    }
  }, (e) => { reject(e) }, options)
})

const cloneMaterialAndGeometry = (targetScene: Object3D, sourceScene: Object3D) => {
  traverseTwo(targetScene, sourceScene, (object1, object2) => {
    if (object1 instanceof Mesh && object2 instanceof Mesh) {
      object1.geometry = object2.geometry.clone()
      object1.material = object2.material.clone()
      object1.geometry.needsUpdate = true
      object1.material.needsUpdate = true
    }
  })
}

export {
  traverseTwo,
  containsTexture,
  replaceMaterial,
  getObject3DInfo,
  replaceMesh,
  getGltfTransformDocument,
  cloneMaterialAndGeometry,
}
