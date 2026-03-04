import type {DeepReadonly} from 'ts-essentials'

import type {BaseGraphObject, SceneGraph} from '../shared/scene-graph'
import type {Eid} from '../shared/schema'

import type {PrefabsHandle, SceneHandle, SpacesHandle} from './scene-types'
import type {Time} from './time'
import type {AudioControls} from './audio-controls-types'
import type {Mat4, Vec3Source} from './math/math'
import type {Object3D, Scene, WebGLRenderer} from './three-types'
import type {CameraManager, CameraObject} from './camera-manager-types'
import type {InputApi} from './input-types'
import type {XrManager} from './xr/xr-manager-types'
import type {Events} from './events-types'
import type {PointerApi, RaycastStage} from './pointer-events-types'
import type {IntersectionResult} from './raycast-types'
import type {TransformManager} from './transform-manager-types'

// @attr[](srcs = "world-attribute.ts")
// @attr[](srcs = "entity-types.ts")

// @inliner-skip-next
import type {Entity} from './entity-types'
import type {EffectsManager} from './effects-manager-types'

type MatrixUpdateMode = 'auto' | 'manual'

interface ThreeState {
  renderer: WebGLRenderer
  activeCamera: CameraObject
  entityToObject: Map<Eid, Object3D>
  scene: Scene
  /**
   * By default, 'manual' uses more efficient matrix update logic, but requires you to call
   * `world.three.notifyChanged` after moving or reparenting raw three.js objects.
   * If it's preferred have all matrices recalculated on each frame, set to 'auto'.
   */
  setMatrixUpdateMode(mode: MatrixUpdateMode): void
  /**
   * When in manual matrix update mode, call notifyChanged after moving or reparenting
   * raw three.js objects.
   */
  notifyChanged: (object: Object3D) => void
}

interface BaseWorld {
  _id: number
  time: Time
  allEntities: Set<Eid>
  eidToEntity: Map<Eid, Entity>
  three: ThreeState
  insertRaycastStage: (stage: RaycastStage, idx: number) => void
  /** @deprecated */
  scene: Scene
}

type TickMode = 'partial' | 'full' | 'zero'

interface LateWorld {
  start: () => void
  stop: () => void
  tick: (dt?: number) => void
  tock: () => void
  getTickMode: () => TickMode
  setTickMode: (tickMode: TickMode) => void
  destroy: () => void
  loadScene: (
    scene: DeepReadonly<SceneGraph>, callback?: (handle: SceneHandle) => void
  ) => SceneHandle
  createEntity: (prefabNameOrEid?: string | Eid) => Eid
  deleteEntity: (eid: Eid) => void
  getInstanceEntity: (instanceEid: Eid, prefabChildEid: Eid) => Eid
  spawnIntoObject: (
    eid: Eid, object: DeepReadonly<BaseGraphObject>, graphIdToEid: Map<string, Eid>
  ) => void
  setScale: (eid: Eid, x: number, y: number, z: number) => void
  setPosition: (eid: Eid, x: number, y: number, z: number) => void
  setQuaternion: (eid: Eid, x: number, y: number, z: number, w: number) => void
  setTransform: (eid: Eid, transform: Mat4) => void
  getWorldTransform: (eid: Eid, transform: Mat4) => void
  normalizeQuaternion: (eid: Eid) => void
  setParent: (eid: Eid, parent: Eid) => void
  getParent: (eid: Eid) => Eid
  getChildren: (eid: Eid) => Generator<Eid>
  raycast: (
    origin: Vec3Source,
    direction: Vec3Source,
    near?: number,
    far?: number
  ) => IntersectionResult[]
  raycastFrom: (eid: Eid, near?: number, far?: number) => IntersectionResult[]
  audio: AudioControls
  camera: CameraManager
  pointer: PointerApi
  events: Events
  getEntity: (eid: Eid) => Entity
  transform: TransformManager
  input: InputApi
  xr: XrManager
  setSceneHook: (hook: SpacesHandle & PrefabsHandle) => void
  spaces: SpacesHandle
  effects: EffectsManager
}

interface World extends BaseWorld, LateWorld {

}

export type {
  BaseWorld,
  LateWorld,
  World,
  InputApi,
  TickMode,
  MatrixUpdateMode,
}
