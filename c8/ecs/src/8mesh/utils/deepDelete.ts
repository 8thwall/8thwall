// @ts-nocheck
import UpdateManager from '../components/core/UpdateManager'

/** Recursively erase THE CHILDREN of the passed object */
function deepDelete(object3D) {
  object3D.children.forEach((child) => {
    if (child.children.length > 0) deepDelete(child)
    object3D.remove(child)
    UpdateManager.disposeOf(child)
    if (child.material) {
      if (Array.isArray(child.material)) {
        for (const material of child.material) {
          material.dispose()
        }
      } else {
        child.material.dispose()
      }
    }

    if (child.geometry) child.geometry.dispose()
  })

  object3D.children = []
}

export default deepDelete
