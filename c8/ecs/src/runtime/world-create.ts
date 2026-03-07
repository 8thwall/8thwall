import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '../shared/scene-graph'
import {makeSceneSyncSystem} from './built-in-systems'
import {runBehaviors} from './registry'
import {createEntity, deleteEntity} from './entity'
import {getParent, setParent, getChildren} from './parent'
import {createTime, updateTime, clearDelta} from './time'
import {getAudioManager} from './audio-manager'
import * as Transform from './transform'
import {asm} from './asm'
import {Position, Scale, Quaternion, ThreeObject} from './components'
import type {Eid} from '../shared/schema'
import {createEvents, flushEvents} from './events'
import {dispatchPhysicsEvents} from './physics-events'
import {createPointerListener, RaycastStage} from './pointer-events'
import type {Scene, WebGLRenderer} from './three-types'
import {createCameraManager} from './camera-manager'
import {createInputListener} from './input-events'
import {createXrManager} from './xr/xr-manager'
import {createInputManager} from './input-manager'
import {events as eventIds} from './event-ids'
import type {BaseWorld, LateWorld, TickMode, World} from './world'
import type {CameraObject} from './camera-manager-types'
import type {PrefabsHandle, SceneHandle, SpacesHandle} from './scene-types'
import {notifyChanged} from './matrix-refresh'
import {spawnScene} from './scene-handle'
import {spawnIntoObject} from './spawn-into-object'
import {raycast, raycastFrom} from './raycast'
import {createTransformManager} from './transform-manager'
import {getInstanceChild, spawnPrefab} from './prefab-entity'
import {createEntityReference} from './entity-impl'
import type {Entity} from './entity-types'
import {createUiEventsRaycaster} from './ui-events-raycaster'
import {getMediaManager} from './media-manager'
import {createEffectsManager} from './effects-manager'

const createWorld = (
  scene: Scene,
  renderer: WebGLRenderer,
  camera: CameraObject
): World => {
  const _id = asm.createWorld()
  const time = createTime()

  const allEntities = new Set([])

  const eidToEntity = new Map<Eid, Entity>()

  let loop: ReturnType<typeof requestAnimationFrame>

  let tickMode: TickMode = 'full'
  let running = false
  let destroyed = false
  const postRenderCallbacks: (() => void)[] = []

  const raycastStages: RaycastStage[] = []

  const insertRaycastStage = (stage: RaycastStage, idx: number) => {
    if (idx < 0 || idx > raycastStages.length) {
      throw new Error('Invalid index for raycast stage')
    }
    raycastStages.splice(idx, 0, stage)
  }

  const baseWorld: BaseWorld = {
    _id,
    time,
    allEntities,
    eidToEntity,
    three: {
      scene,
      renderer,
      activeCamera: camera,
      entityToObject: new Map(),
      setMatrixUpdateMode: (mode) => {
        switch (mode) {
          case 'auto':
            scene.matrixWorldAutoUpdate = true
            scene.matrixAutoUpdate = true
            break
          case 'manual':
            scene.matrixWorldAutoUpdate = false
            scene.matrixAutoUpdate = false
            break
          default:
            throw new Error(`Unknown matrix update mode: ${mode}`)
        }
      },
      notifyChanged,
    },
    insertRaycastStage,
    scene,
  }

  const world = baseWorld as World

  insertRaycastStage({
    getCamera: () => world.three.activeCamera,
    scene: world.three.scene,
    includeWorldPosition: true,
  }, 0)

  const {audioControls, internalMedia} = getMediaManager(world)
  const audioManager = getAudioManager(world)
  const events = createEvents(world)
  const pointerListener = createPointerListener(raycastStages, events, renderer.domElement)
  const cameraManager = createCameraManager(world, audioManager.listener, events)
  const inputListener = createInputListener(events, renderer.domElement)
  const xrManager = createXrManager(world, events)
  const inputManager = createInputManager(inputListener.api)
  const transformManager = createTransformManager(world)
  const eventRaycaster = createUiEventsRaycaster(world, raycastStages, events, renderer.domElement)
  const effectsManager = createEffectsManager(world)

  asm.initWithComponents(
    world._id,
    Position.forWorld(world).id,
    Scale.forWorld(world).id,
    Quaternion.forWorld(world).id,
    ThreeObject.forWorld(world).id
  )

  const registerPostRender = (cb: () => void) => {
    postRenderCallbacks.push(cb)
  }

  const syncScene = makeSceneSyncSystem(world, registerPostRender)

  const pipeline = () => {
    runBehaviors(world)
    const progressedPhysics = asm.progressWorld(world._id, world.time.delta)
    if (progressedPhysics) {
      dispatchPhysicsEvents(world, events)
    }
    syncScene()
  }

  const zeroTimeFrame = () => {
    clearDelta(time)
    pipeline()
  }

  const processTick = (dt: number) => {
    if (tickMode === 'full') {
      updateTime(time, dt)
      xrManager.tick()
      flushEvents(events)
      inputListener.handleGamepadLoop()
      inputManager.handleActions()
      pipeline()
    } else if (tickMode === 'partial') {
      clearDelta(time)
      xrManager.drawPausedBackground()
      syncScene()
    } else if (tickMode === 'zero') {
      zeroTimeFrame()
    }
  }

  const frame = (dt: number) => {
    if (destroyed) {
      throw new Error('World was destroyed.')
    }
    processTick(dt)
    if (running) {
      loop = requestAnimationFrame(frame)
    }
  }

  const start = () => {
    cancelAnimationFrame(loop)
    loop = requestAnimationFrame(frame)
    running = true
  }

  const stop = () => {
    cancelAnimationFrame(loop)
    running = false
  }

  const tick = (dt = 0) => {
    if (destroyed) {
      throw new Error('World was destroyed.')
    }
    if (running) {
      throw new Error('World is running, cannot progress manually.')
    }
    processTick(dt)
  }

  const tock = () => {
    postRenderCallbacks.forEach((cb) => {
      cb()
    })

    if (tickMode === 'full') {
      xrManager.tock()
    }
  }
  const loadScene = (graph: DeepReadonly<SceneGraph>, callback?: (handle: SceneHandle) => void) => {
    zeroTimeFrame()  // Allow queries to initialize before spawning
    const handle = spawnScene(world, graph)
    if (callback) {
      callback(handle)
    }
    // TODO(christoph): Remove this after switching to synchronous rendering
    zeroTimeFrame()  // Sync ecs state with threejs state
    return handle
  }

  const setTickMode = (mode: TickMode) => {
    tickMode = mode
    if (mode === 'full') {
      internalMedia.setStudioPause(false)
    } else {
      internalMedia.setStudioPause(true)
    }
  }

  const getEntity = (eid: Eid): Entity => {
    if (!eid) {
      throw new Error('Entity ID was not provided')
    }
    const existing = world.eidToEntity.get(eid)
    if (existing) {
      return existing
    }
    if (!world.allEntities.has(eid)) {
      throw new Error(`Entity ${eid} does not exist`)
    }
    const entity = createEntityReference(world, eid)
    eidToEntity.set(eid, entity)
    return entity
  }

  const destroy = () => {
    if (destroyed) {
      return
    }
    inputListener.detach()
    inputManager.detach()
    cameraManager.detach()
    pointerListener.detach()
    xrManager.detach()
    effectsManager.detach()
    world.allEntities.forEach(eid => deleteEntity(world, eid))
    zeroTimeFrame()  // Sync to flush the observers + callbacks and perform any system cleanup
    internalMedia.destroy()
    asm.deleteWorld(world._id)
    destroyed = true
    stop()
  }

  let sceneHook: SpacesHandle & PrefabsHandle

  const lateWorld: LateWorld = {
    start,
    stop,
    tick,
    tock,
    destroy,
    loadScene,
    createEntity: (prefabNameOrEid?: string | Eid) => {
      if (!prefabNameOrEid) {
        return createEntity(world)
      }

      if (!sceneHook) {
        throw new Error('No scene hook set')
      }
      // const eid = sceneHook.getPrefab(prefabNameOrEid)
      const eid = typeof prefabNameOrEid === 'string'
        ? sceneHook.getPrefab(prefabNameOrEid)
        : prefabNameOrEid
      if (!eid) {
        throw new Error(`Prefab ${prefabNameOrEid} not found`)
      }
      return spawnPrefab(world, eid)
    },
    deleteEntity: (eid: Eid) => deleteEntity(world, eid),
    getInstanceEntity: (instanceEid: Eid, prefabChildEid: Eid) => (
      getInstanceChild(world, instanceEid, prefabChildEid)
    ),
    getTickMode: () => tickMode,
    setTickMode: (mode: TickMode) => setTickMode(mode),
    spawnIntoObject: spawnIntoObject.bind(null, world),
    setScale: Transform.setScale.bind(null, world),
    setPosition: Transform.setPosition.bind(null, world),
    setQuaternion: Transform.setQuaternion.bind(null, world),
    setTransform: Transform.setTransform.bind(null, world),
    getWorldTransform: Transform.getWorldTransform.bind(null, world),
    normalizeQuaternion: Transform.normalizeQuaternion.bind(null, world),
    setParent: setParent.bind(null, world),
    getParent: getParent.bind(null, world),
    getChildren: getChildren.bind(null, world),
    raycast: raycast.bind(null, world),
    raycastFrom: raycastFrom.bind(null, world),
    audio: audioControls,
    getEntity,
    camera: cameraManager,
    events,
    pointer: pointerListener,
    transform: transformManager,
    input: {
      ...inputListener.api,
      ...inputManager.api,
      attach: () => {
        eventRaycaster.attach()
        inputListener.attach()
        inputManager.attach()
      },
      detach: () => {
        eventRaycaster.detach()
        inputListener.detach()
        inputManager.detach()
      },
    },
    xr: xrManager,
    setSceneHook: (hook: SpacesHandle & PrefabsHandle) => {
      sceneHook = hook
    },
    spaces: {
      loadSpace: (spaceIdOrName: string) => {
        if (!sceneHook) {
          throw new Error('No scene hook set')
        }
        sceneHook.loadSpace(spaceIdOrName)
        events.dispatch(events.globalId, eventIds.ACTIVE_SPACE_CHANGE)
      },
      listSpaces: () => {
        if (!sceneHook) {
          throw new Error('No scene hook set')
        }
        return sceneHook.listSpaces()
      },
      getActiveSpace: () => {
        if (!sceneHook) {
          throw new Error('No scene hook set')
        }
        return sceneHook.getActiveSpace()
      },
    },
    effects: effectsManager,
  }

  return Object.assign(world, lateWorld)
}

export {
  createWorld,
}
