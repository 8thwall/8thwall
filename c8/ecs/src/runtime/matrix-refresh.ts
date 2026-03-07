import THREE from './three'
import type {RelaxedObject3D, Scene} from './three-types'

/*

When manual matrix mode is on, each entity that has its transform changed is expected to call
notifyChanged.

This will cause the entity's three object and its parent chain to be marked for matrix world update.

Each frame, right before render, we traverse the scene depth-first and update world matrices of
objects marked as matrixWorldNeedsUpdate.

If an object doesn't have userData.childMatrixNeedsUpdate set, we won't traverse its
children to update their world matrices unless an ancestor already updated in this frame.

e.g. if we have the following scene
 Scene ─┬▶ A ─┬▶ C ─┬▶ E
        │     └▶ G  └▶ F
        └▶ B ─▶ D
and mark C.

Scene and A will be set to childMatrixNeedsUpdate: true
C will be marked as matrixWorldNeedsUpdate: true

When we iterate the scene, the following will happen:

Scene: I don't need an update, but a child does, so I'll iterate children
  └▶ A: No self or parent update, no change, but child needs update, so I'll iterate children
  │  └▶ C: I need update, so I'll update my world matrix, then iterate children since I changed
  │  │ └▶ E/F: My parent updated so I'll update
  │  └▶ G: Noop - No self or parent update, no children have changed
  └▶ B: Noop - No self or parent update, and no children have changed

All in all, this can dramatically reduce the amount of matrix updates required.

Instead of visiting all 8 objects and doing 8 matrix multiplications, we only
visit 7 (skipping D), and only do 3 matrix multiplies (Skipping Scene, A, B, and G).
*/

const isMatrixModeManual = (scene: Scene) => (
  !scene.matrixWorldAutoUpdate && !scene.matrixAutoUpdate
)

const baseUpdateMatrixWorld = (THREE.Object3D as any).prototype.updateMatrixWorld
if (!baseUpdateMatrixWorld) {
  throw new Error('Expected Object3D to have updateMatrixWorld')
}

const refreshMatrix = (object: RelaxedObject3D, parentChanged: boolean) => {
  let selfChanged = parentChanged
  if (object.matrixAutoUpdate) {
    // NOTE(christoph): this.matrixWorldNeedsUpdate = true is set internally
    object.updateMatrix()
  }

  if (parentChanged || object.matrixWorldNeedsUpdate) {
    object.matrixWorldNeedsUpdate = false
    if (object.parent === null) {
      object.matrixWorld.copy(object.matrix)
    } else {
      object.matrixWorld.multiplyMatrices(object.parent.matrixWorld, object.matrix)
    }
    selfChanged = true
  }

  if (selfChanged || object.userData.childMatrixNeedsUpdate) {
    object.userData.childMatrixNeedsUpdate = false
    for (const child of object.children) {
      // NOTE(christoph): If an object overrides the default implementation of Object3D's
      // updateMatrixWorld, it might do other things beyond updating the world matrix, so we
      // need to call it here.
      const hasDefaultMatrixUpdate = child.updateMatrixWorld === baseUpdateMatrixWorld
      if (hasDefaultMatrixUpdate) {
        refreshMatrix(child, selfChanged)
      } else {
        // NOTE(christoph): The custom updateMatrixWorld function doesn't clear or care about
        // childMatrixNeedsUpdate, so we just call it directly. We pass true to force a full
        // refresh, since any number of child objects may need updates.
        child.updateMatrixWorld(true)
      }
    }
  }
}

const maybeRefreshWorldMatrices = (scene: Scene) => {
  if (!isMatrixModeManual(scene)) {
    return
  }
  refreshMatrix(scene, false)
}

const notifyChildChanged = (object: RelaxedObject3D | null) => {
  if (!object) {
    return
  }

  // NOTE(christoph): DO NOT add an early out for childMatrixNeedsUpdate here.
  // We need to propagate all the way to the root in case a custom updateMatrixWorld
  // doesn't clear the parent bit.
  object.userData.childMatrixNeedsUpdate = true
  notifyChildChanged(object.parent)
}

const notifyChanged = (object: RelaxedObject3D) => {
  object.matrixWorldNeedsUpdate = true
  notifyChildChanged(object.parent)
}

const addChild = (parent: RelaxedObject3D, child: RelaxedObject3D) => {
  parent.add(child)
  notifyChanged(child)
}

function notifySelfChanged(this: RelaxedObject3D) {
  notifyChanged(this)
}

export {
  refreshMatrix,
  notifyChanged,
  addChild,
  maybeRefreshWorldMatrices,
  notifySelfChanged,
}
