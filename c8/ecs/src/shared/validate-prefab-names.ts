import {getPrefabs} from './object-hierarchy'
import type {SceneGraph} from './scene-graph'

const validatePrefabNames = (scene: SceneGraph): boolean => {
  if (!scene.objects) {
    return true
  }
  const prefabNames = new Set<string>()
  const prefabs = getPrefabs(scene)
  return prefabs.every(({name}) => {
    if (!name || prefabNames.has(name)) {
      return false
    }
    prefabNames.add(name)
    return true
  })
}

export {
  validatePrefabNames,
}
