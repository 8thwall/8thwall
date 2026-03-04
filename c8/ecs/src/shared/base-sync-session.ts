import type {
  DebugMessage, DebugStream, StateUpdateMessage, DebugState, BaseEditMessage,
} from './debug-messaging'
import {stringToBytes} from './data'
import type {IApplication} from './application-type'
import type {SceneDoc} from './crdt'

const createBaseSyncSession = (
  debugId: string,
  application: IApplication,
  stream: DebugStream,
  initialDebugState: DebugState,
  originalDoc: SceneDoc
) => {
  let debounceUpdate_: ReturnType<typeof setTimeout>

  const doc = originalDoc.clone('')

  let changesReceived: Uint8Array[] = []
  let activeSpace_: string | undefined = initialDebugState.activeSpaceId
  let paused_: boolean = !!initialDebugState.paused

  const refreshActiveSpace = () => {
    if (!activeSpace_) {
      return
    }
    const world = application.getWorld()
    if (!world) {
      return
    }
    const active = world.spaces.getActiveSpace()?.id
    if (active === activeSpace_) {
      return
    }
    try {
      world.spaces.loadSpace(activeSpace_)
    } catch (err) {
      // NOTE(christoph): If the active space is not in the current scene graph, it can throw.
      // We'll try again the next time the scene graph updates.
    }
  }

  const handleBaseEdit = (msg: BaseEditMessage) => {
    if (msg.debugId !== debugId) {
      return
    }
    changesReceived.push(...msg.diffs.map(stringToBytes))
    clearTimeout(debounceUpdate_)
    debounceUpdate_ = setTimeout(() => {
      try {
        doc.applyChanges(changesReceived)
        const raw = doc.raw()
        application.getScene().updateBaseObjects(raw.objects)
        application.getScene().updateDebug(raw)
        // NOTE(christoph): This handles a case where a new space is created and loaded immediately
        refreshActiveSpace()
        changesReceived = []
      } catch (err) {
        // eslint-disable-next-line no-empty
      }
    }, 100)
  }

  const refreshPaused = () => {
    const world = application.getWorld()
    if (!world) {
      return
    }
    const currentTickMode = world.getTickMode()
    const goalTickMode = paused_ ? 'partial' : 'full'

    if (currentTickMode !== goalTickMode) {
      world.setTickMode(goalTickMode)
    }
  }

  const handleStateUpdate = (msg: StateUpdateMessage) => {
    if (msg.debugId !== debugId) {
      return
    }

    if (msg.state.activeSpaceId) {
      activeSpace_ = msg.state.activeSpaceId
    }

    if (typeof msg.state.paused === 'boolean') {
      paused_ = msg.state.paused
    }

    refreshActiveSpace()
    refreshPaused()
  }

  const reinitialize = async () => {
    const raw = {...doc.raw()}
    if (activeSpace_) {
      raw.entrySpaceId = activeSpace_
    }
    await application.init(raw)
    refreshPaused()
  }

  const handleMessage = (msg: DebugMessage) => {
    switch (msg.action) {
      case 'ECS_BASE_EDIT':
        handleBaseEdit(msg)
        break
      case 'ECS_STATE_UPDATE': {
        handleStateUpdate(msg)
        break
      }
      case 'ECS_RESET_SCENE': {
        reinitialize()
        break
      }
      default:
    }
  }

  const detach = () => {
    stream.cancelListen(handleMessage)
    clearTimeout(debounceUpdate_)
  }

  const attach = async () => {
    detach()
    stream.listen(handleMessage)
    await reinitialize()
    stream.send({
      action: 'ECS_ATTACH_CONFIRM',
      debugId,
    })
  }

  return {
    attach,
    detach,
    debugId,
  }
}

export {
  createBaseSyncSession,
}
