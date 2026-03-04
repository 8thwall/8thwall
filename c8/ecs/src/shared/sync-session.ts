import type {DeepReadonly} from 'ts-essentials'

import type {Ecs} from '../runtime/index'
import type {World} from '../runtime/world'
import type {GraphObject, SceneGraph} from './scene-graph'
import type {Eid} from './schema'
import type {
  DebugEditMessage, DebugMessage, DebugStream, StateUpdateMessage, DebugState,
  DocRefreshMessage, ResetLevel, BaseEditMessage,
} from './debug-messaging'
import {bytesToString, stringToBytes} from './data'
import type {SceneDoc} from './crdt.js'
import type {IApplication} from './application-type'
import {getTransformMap, getTransformUpdates, TransformMap} from './transform-map'
import {worldToSceneGraph} from './world-to-scene-graph'
import {isBaseGraphObject, isPrefab} from './object-hierarchy'

const clearEntity = (ecs: Ecs, world: World, eid: Eid) => {
  ecs.listAttributes().forEach((name) => {
    // NOTE(christoph): This is the component that makes an entity an "object" - if it gets removed
    // this will do the delete object cleanup
    if (name === 'three-object') {
      return
    }
    const attribute = ecs.getAttribute(name)
    if (attribute.has(world, eid)) {
      attribute.remove(world, eid)
    }
  })
}

const filterEphemeral = (graph: DeepReadonly<SceneGraph>) => {
  const filteredObjects = Object.values(graph.objects).filter(o => !o.ephemeral)
  return {
    ...graph,
    objects: Object.fromEntries(filteredObjects.map(o => [o.id, o])),
  }
}

const FRAME_SKIP_COUNT = 5
const DEBUG_ACTOR_ID = 'de'  // Arbitrary hex digits

const createWorldDebugger = (
  debugId: string, ecs: Ecs, application: IApplication, stream: DebugStream,
  initialDebugState: DebugState, originalDoc: SceneDoc,
  loadSceneDoc: (bytes: Uint8Array, actorId: string) => SceneDoc,
  enableTransformMap: boolean, resetLevel: ResetLevel
) => {
  let skipCount_ = 0
  let frame_: ReturnType<typeof requestAnimationFrame>
  let debounceUpdate_: ReturnType<typeof setTimeout>
  const ephemeralUpdatesById_ = new Map<string, DeepReadonly<GraphObject>>()
  const ephemeralIdIndices = new Map<Eid, number>()
  const ephemeralIdCounter = {value: 0}

  let doc = originalDoc.clone('')
  let lastSent = doc.raw()
  let changesToSend: Uint8Array[] = []
  let changesReceived: Uint8Array[] = []
  let prevTransformMap: TransformMap = enableTransformMap ? getTransformMap(doc.raw()) : {}
  let debugActiveSpaceId = initialDebugState.activeSpaceId || doc.raw().entrySpaceId

  const baseDoc = originalDoc.clone('')

  const world = application.getWorld()

  // TODO(cindyhu): implement soft reset
  switch (resetLevel) {
    case 'none':
      break
    default: {
      application.getScene().remove()
      application.getScene().updateDebug(doc.raw())
      application.getScene().updateBaseObjects(baseDoc.raw().objects)

      if (
        debugActiveSpaceId &&
        debugActiveSpaceId !== world.spaces.getActiveSpace()?.id
      ) {
        application.getScene().loadSpace(debugActiveSpaceId)
      }

      ephemeralIdCounter.value = 0
      ephemeralIdIndices.clear()
      break
    }
  }

  const dispatchUpdate = () => {
    const newScene = worldToSceneGraph(
      ecs, application, ephemeralIdIndices, ephemeralIdCounter, baseDoc.raw(), doc.raw()
    )
    if (enableTransformMap) {
      const newTransformMap = getTransformMap(newScene)
      const {updatedTransforms, deletedIds} = getTransformUpdates(prevTransformMap, newTransformMap)
      doc.updateWithoutTransform(() => newScene)
      stream.send({
        action: 'ECS_TRANSFORM_UPDATE',
        debugId,
        updatedTransforms,
        deletedIds,
      }, true)
      prevTransformMap = newTransformMap
    } else {
      doc.update(() => newScene)
    }
    changesToSend.push(...doc.getChanges(lastSent))
    lastSent = doc.raw()
    const worldActiveSpaceId = world.spaces.getActiveSpace()?.id
    const spaceNeedsUpdate = worldActiveSpaceId && worldActiveSpaceId !== debugActiveSpaceId
    if (changesToSend.length || spaceNeedsUpdate) {
      stream.send({
        action: 'ECS_WORLD_UPDATE',
        debugId,
        diffs: changesToSend.map(bytesToString),
        spaceId: spaceNeedsUpdate ? worldActiveSpaceId : '',
      }, true)
      changesToSend = []
      debugActiveSpaceId = worldActiveSpaceId || ''
    }
  }

  const handleDebugEdit = (msg: DebugEditMessage) => {
    if (msg.debugId !== debugId) {
      return
    }
    changesReceived.push(...msg.diffs.map(stringToBytes))
    msg.ephemeralUpdates?.forEach((update) => {
      ephemeralUpdatesById_.set(update.id, update)
    })
    clearTimeout(debounceUpdate_)
    debounceUpdate_ = setTimeout(() => {
      try {
        changesToSend.push(...doc.getChanges(lastSent))
        doc.applyChanges(changesReceived)
        application.getScene().updateDebug(filterEphemeral(doc.raw()))
        lastSent = doc.raw()
        changesReceived = []
        ephemeralUpdatesById_.forEach((update) => {
          try {
            if (!isBaseGraphObject(update) || isPrefab(update)) {
              return
            }
            const eid = BigInt(update.id)
            clearEntity(ecs, world, eid)
            // TODO(Dale): Handle prefabs before spawnIntoObject is called
            world.spawnIntoObject(eid, update, application.getScene().graphIdToEid)
          } catch (err) {
          // eslint-disable-next-line no-console
            console.warn(err)
          }
        })
        ephemeralUpdatesById_.clear()
      } catch (err) {
        // eslint-disable-next-line no-empty
      }
    }, 100)
  }

  const handleBaseEdit = (msg: BaseEditMessage) => {
    if (msg.debugId !== debugId) {
      return
    }
    baseDoc.applyChanges(msg.diffs.map(stringToBytes))
    application.getScene().updateBaseObjects(baseDoc.raw().objects)
  }

  const handleLoadSpace = (spaceId?: string) => {
    if (!spaceId) {
      return
    }
    debugActiveSpaceId = spaceId
    world.spaces.loadSpace(spaceId)
  }

  const handleDocRefresh = async (msg: DocRefreshMessage) => {
    if (msg.debugId !== debugId) {
      return
    }
    const refreshedDoc = loadSceneDoc(stringToBytes(msg.doc), DEBUG_ACTOR_ID)
    doc = refreshedDoc.clone('')
    lastSent = doc.raw()
    changesToSend = []
  }

  const pauseOrResume = (paused?: boolean) => {
    if (typeof paused !== 'boolean') {
      return
    }

    if (paused) {
      world.setTickMode('partial')
    } else {
      world.setTickMode('full')
    }
  }

  const handleStateUpdate = (msg: StateUpdateMessage) => {
    if (msg.debugId !== debugId) {
      return
    }

    const {paused, activeSpaceId} = msg.state
    pauseOrResume(paused)
    handleLoadSpace(activeSpaceId)
  }

  const handleMessage = (msg: DebugMessage) => {
    switch (msg.action) {
      case 'ECS_DEBUG_EDIT':
        handleDebugEdit(msg)
        break
      case 'ECS_BASE_EDIT':
        handleBaseEdit(msg)
        break
      case 'ECS_STATE_UPDATE': {
        handleStateUpdate(msg)
        break
      }
      case 'ECS_DOC_REFRESH': {
        handleDocRefresh(msg)
        break
      }
      default:
    }
  }

  const handleFrame = () => {
    frame_ = requestAnimationFrame(handleFrame)

    if (skipCount_ < FRAME_SKIP_COUNT) {
      skipCount_++
    } else {
      skipCount_ = 0
      dispatchUpdate()
    }
  }

  const detach = () => {
    cancelAnimationFrame(frame_)
    stream.cancelListen(handleMessage)
    clearTimeout(debounceUpdate_)
  }

  const attach = () => {
    detach()
    dispatchUpdate()
    pauseOrResume(initialDebugState?.paused)
    frame_ = requestAnimationFrame(handleFrame)
    stream.listen(handleMessage)
  }

  return {
    attach,
    detach,
    debugId,
  }
}

type WorldDebugger = ReturnType<typeof createWorldDebugger>

export {
  createWorldDebugger,
  worldToSceneGraph,
  DEBUG_ACTOR_ID,
}

export type {
  WorldDebugger,
}
