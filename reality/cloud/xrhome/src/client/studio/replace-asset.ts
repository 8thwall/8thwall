import type {SceneGraph} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

/* eslint-disable arrow-parens */

const replaceAsset = <T>(
  target: DeepReadonly<T>,
  oldPath: string,
  newPath: string
): DeepReadonly<T> => {
  if (target === oldPath) {
    return newPath as DeepReadonly<T>
  }

  if (!target || typeof target !== 'object') {
    return target
  }

  // NOTE(christoph): Currently there are no arrays that contain asset paths in the scene graph.
  if (Array.isArray(target)) {
    return target
  }

  let modified = false
  const newObj = {...target}
  Object.keys(target).forEach(key => {
    const value = target[key]
    const newValue = replaceAsset(value, oldPath, newPath)
    if (newValue !== value) {
      modified = true
      newObj[key] = newValue
    }
  })

  if (!modified) {
    return target
  }

  return newObj
}

const replaceAssetInScene = (
  scene: DeepReadonly<SceneGraph>,
  oldPath: string,
  newPath: string
): DeepReadonly<SceneGraph> => replaceAsset(scene, oldPath, newPath)

export {
  replaceAssetInScene,
}
