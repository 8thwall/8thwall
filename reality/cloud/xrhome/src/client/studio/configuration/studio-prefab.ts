import type {StudioStateContext} from '../studio-state-context'
import type {SceneContext} from '../scene-context'
import {deleteObjects} from './delete-object'

const handleDeletePrefab = (ctx: SceneContext, stateCtx: StudioStateContext, prefabId: string) => {
  const instancesExist = Object.values(ctx.scene.objects).some(
    object => object.instanceData?.instanceOf === prefabId
  )
  if (instancesExist) {
    stateCtx.update(p => ({...p, objectToBeErased: prefabId}))
    return
  }
  ctx.updateScene(s => deleteObjects(s, [prefabId]))
}

export {
  handleDeletePrefab,
}
