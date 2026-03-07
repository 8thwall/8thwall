import THREE from './three'
import type {BufferGeometry, Material, Mesh, RelaxedObject3D, Texture} from './three-types'
import {deleteVideoTexture} from './video-texture'

const disposeTexture = (texture: Texture) => {
  if (!texture) {
    return
  }
  if (texture instanceof THREE.VideoTexture) {
    deleteVideoTexture(texture)
  } else {
    texture.dispose()
  }
}

const maybeDisposeMaterial = (material: Material | Material[]) => {
  if (!material) {
    return
  }
  if (Array.isArray(material)) {
    material.forEach(maybeDisposeMaterial)
  } else {
    if (!material.userData.disposable) {
      return
    }
    Object.keys(material).forEach((key) => {
      const texture = material[key as keyof typeof material]
      if (texture instanceof THREE.Texture) {
        disposeTexture(texture)
      }
    })
    material.dispose()
  }
}

const maybeDisposeGeometry = (geometry: BufferGeometry) => {
  if (!geometry || !geometry.userData.disposable) {
    return
  }
  geometry.dispose()
}

const disposeSingleObject = (object: RelaxedObject3D) => {
  if (object instanceof THREE.Mesh) {
    maybeDisposeGeometry(object.geometry)
    maybeDisposeMaterial(object.material)
  }
}

const markAsDisposable = (object: RelaxedObject3D) => {
  if (object instanceof THREE.Mesh) {
    if (object.material) {
      if (Array.isArray(object.material)) {
        object.material.forEach((material) => {
          material.userData.disposable = true
        })
      } else {
        object.material.userData.disposable = true
      }
    }
    if (object.geometry) {
      object.geometry.userData.disposable = true
    }
  }
}

// NOTE(johnny): Only use this on objects that are self-contained. We don't want to accidentally
// set objects as disposable from other systems.
const markAsDisposableRecursively = (object: RelaxedObject3D) => {
  object.traverse((child) => {
    markAsDisposable(child)
  })
}

const disposeObject = (object: RelaxedObject3D | Mesh) => object.traverse(disposeSingleObject)

export {
  disposeObject,
  disposeTexture,
  maybeDisposeMaterial,
  maybeDisposeGeometry,
  markAsDisposable,
  markAsDisposableRecursively,
}
