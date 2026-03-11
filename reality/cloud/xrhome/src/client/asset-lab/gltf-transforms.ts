import * as THREE from 'three'
import type {Document, Transform} from '@gltf-transform/core'

const smoothNormals: Transform = async (document: Document): Promise<void> => {
  const meshes = document.getRoot().listMeshes()

  // Create or reuse a buffer for normals
  let normalBuffer = document.getRoot().listBuffers()[0]
  if (!normalBuffer) {
    normalBuffer = document.createBuffer()
    document.getRoot().listBuffers().push(normalBuffer)
  }

  for (const mesh of meshes) {
    for (const primitive of mesh.listPrimitives()) {
      const position = primitive.getAttribute('POSITION')
      // eslint-disable-next-line no-continue
      if (!position) continue

      const positionArray = position.getArray()!
      const indices = primitive.getIndices()?.getArray()

      // Create a map of vertex positions to their normals
      const vertexNormals = new Map<string, THREE.Vector3>()
      const v1 = new THREE.Vector3()
      const v2 = new THREE.Vector3()
      const v3 = new THREE.Vector3()
      const edge1 = new THREE.Vector3()
      const edge2 = new THREE.Vector3()
      const normal = new THREE.Vector3()

      // Calculate face normals and accumulate them for each vertex
      if (indices) {
        for (let i = 0; i < indices.length; i += 3) {
          const i1 = indices[i] * 3
          const i2 = indices[i + 1] * 3
          const i3 = indices[i + 2] * 3

          // Get vertices
          v1.set(
            positionArray[i1],
            positionArray[i1 + 1],
            positionArray[i1 + 2]
          )
          v2.set(
            positionArray[i2],
            positionArray[i2 + 1],
            positionArray[i2 + 2]
          )
          v3.set(
            positionArray[i3],
            positionArray[i3 + 1],
            positionArray[i3 + 2]
          )

          // Calculate face normal using cross product
          edge1.subVectors(v2, v1)
          edge2.subVectors(v3, v1)
          normal.crossVectors(edge1, edge2).normalize()

          // Weight normal by triangle area
          const weight = edge1.cross(edge2).length() * 0.5
          // eslint-disable-next-line semi-style
          normal.multiplyScalar(weight);

          // Add weighted normal to each vertex
          [i1, i2, i3].forEach((idx) => {
            const key = `${positionArray[idx]},${positionArray[idx + 1]},${positionArray[idx + 2]}`
            const existing = vertexNormals.get(key)
            if (existing) {
              existing.add(normal)
            } else {
              vertexNormals.set(key, normal.clone())
            }
          })
        }
      } else {
        // Handle non-indexed geometry
        for (let i = 0; i < positionArray.length; i += 9) {
          // Get vertices
          v1.set(
            positionArray[i],
            positionArray[i + 1],
            positionArray[i + 2]
          )
          v2.set(
            positionArray[i + 3],
            positionArray[i + 4],
            positionArray[i + 5]
          )
          v3.set(
            positionArray[i + 6],
            positionArray[i + 7],
            positionArray[i + 8]
          )

          // Calculate face normal using cross product
          edge1.subVectors(v2, v1)
          edge2.subVectors(v3, v1)
          normal.crossVectors(edge1, edge2).normalize()

          // Weight normal by triangle area
          const weight = edge1.cross(edge2).length() * 0.5
          normal.multiplyScalar(weight)

          // Add weighted normal to each vertex
          for (let j = 0; j < 9; j += 3) {
            // eslint-disable-next-line max-len
            const key = `${positionArray[i + j]},${positionArray[i + j + 1]},${positionArray[i + j + 2]}`
            const existing = vertexNormals.get(key)
            if (existing) {
              existing.add(normal)
            } else {
              vertexNormals.set(key, normal.clone())
            }
          }
        }
      }

      // Normalize all accumulated normals
      for (const vertexNormal of vertexNormals.values()) {
        vertexNormal.normalize()
      }

      // Create new normal array
      const normalArray = new Float32Array(positionArray.length)

      // Fill normal array
      if (indices) {
        for (let i = 0; i < indices.length; i++) {
          const idx = indices[i] * 3
          // eslint-disable-next-line max-len
          const key = `${positionArray[idx]},${positionArray[idx + 1]},${positionArray[idx + 2]}`
          const vertexNormal = vertexNormals.get(key)!
          normalArray[idx] = vertexNormal.x
          normalArray[idx + 1] = vertexNormal.y
          normalArray[idx + 2] = vertexNormal.z
        }
      } else {
        for (let i = 0; i < positionArray.length; i += 3) {
          const key = `${positionArray[i]},${positionArray[i + 1]},${positionArray[i + 2]}`
          const vertexNormal = vertexNormals.get(key)!
          normalArray[i] = vertexNormal.x
          normalArray[i + 1] = vertexNormal.y
          normalArray[i + 2] = vertexNormal.z
        }
      }

      // Set or update the NORMAL attribute
      const normalAttribute = primitive.getAttribute('NORMAL')
      if (normalAttribute) {
        normalAttribute.setArray(normalArray)
      } else {
        primitive.setAttribute('NORMAL', document.createAccessor()
          .setArray(normalArray)
          .setType('VEC3')
          .setBuffer(normalBuffer))
      }
    }
  }
}

const getAdjustPbr = (settings: {metalness: number, roughness: number}): Transform => {
  const adjustPbr: Transform = (document) => {
    const materials = document.getRoot().listMaterials()
    for (const material of materials) {
      material.setMetallicFactor(settings.metalness)
      material.setRoughnessFactor(settings.roughness)
    }
  }
  return adjustPbr
}

const getAdjustBrightness = (settings: {textureBrightness: number}): Transform => {
  const adjustBrightness: Transform = async (document) => {
    // TODO (tri) clean up this looping await logic
    const textures = document.getRoot().listTextures()
    for (const texture of textures) {
      // eslint-disable-next-line no-continue
      if (!texture.getMimeType()?.startsWith('image/')) continue

      // eslint-disable-next-line no-await-in-loop
      const imageData = await texture.getImage()
      // eslint-disable-next-line no-continue
      if (!imageData) continue

      // TODO (tri) clean up this looping await logic
      // Create ImageBitmap from the image data
      // eslint-disable-next-line no-await-in-loop
      const imageBitmap = await createImageBitmap(
        new Blob([imageData], {type: texture.getMimeType()!})
      )

      // Create canvas and draw image
      const canvas = new OffscreenCanvas(imageBitmap.width, imageBitmap.height)
      const ctx = canvas.getContext('2d')!
      ctx.drawImage(imageBitmap, 0, 0)

      // Get image data and adjust brightness
      const pixels = ctx.getImageData(0, 0, canvas.width, canvas.height)
      const {data} = pixels

      for (let i = 0; i < data.length; i += 4) {
        // eslint-disable-next-line no-continue
        if (data[i + 3] === 0) continue  // Skip fully transparent pixels

        // Adjust RGB values
        data[i] = Math.min(255, Math.round(data[i] * settings.textureBrightness))          // R
        data[i + 1] = Math.min(255, Math.round(data[i + 1] * settings.textureBrightness))  // G
        data[i + 2] = Math.min(255, Math.round(data[i + 2] * settings.textureBrightness))  // B
      }

      // Put the modified data back
      ctx.putImageData(pixels, 0, 0)

      // Convert canvas to blob and update texture
      // eslint-disable-next-line no-await-in-loop
      const blob = await canvas.convertToBlob({type: 'image/png'})
      // eslint-disable-next-line no-await-in-loop
      const buffer = await blob.arrayBuffer()
      texture.setImage(new Uint8Array(buffer))
      texture.setMimeType('image/png')
    }
  }
  return adjustBrightness
}

export {
  smoothNormals,
  getAdjustPbr,
  getAdjustBrightness,
}
