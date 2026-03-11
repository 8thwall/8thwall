import type {SceneContext} from '../scene-context'

const validateIds = (sceneCtx: SceneContext, ids: string[]) => {
  const invalidIds = ids.filter(id => !sceneCtx.scene.objects[id])
  if (invalidIds.length > 0) {
    throw new Error(`These ids do not exist in the scene: ${invalidIds.join(', ')}`)
  }
  return ids
}

export {
  validateIds,
}
