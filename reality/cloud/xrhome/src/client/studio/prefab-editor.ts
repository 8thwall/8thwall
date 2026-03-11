import type {SceneInterface} from './scene-interface-context'
import type {StudioStateContext} from './studio-state-context'

const PREFAB_FOCUS_TIMEOUT = 25
const ISOLATION_ZOOM_FACTOR = 2

const enterPrefabEditor = (
  stateCtx: StudioStateContext, sceneInterfaceCtx: SceneInterface, prefabId: string
) => {
  stateCtx.update(p => ({...p, selectedPrefab: prefabId}))
  stateCtx.setSelection(prefabId)
  setTimeout(() => {
    sceneInterfaceCtx.focusObject(prefabId, true, ISOLATION_ZOOM_FACTOR)
  }, PREFAB_FOCUS_TIMEOUT)
}

export {
  enterPrefabEditor,
}
