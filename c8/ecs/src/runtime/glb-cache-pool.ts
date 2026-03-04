import type {DeepReadonly} from 'ts-essentials'

import type {Eid} from '../shared/schema'

import type * as THREE_TYPES from './three-types'
import THREE from './three'

type PartialMutableGLTF = {
  // mutable fields
  animations: THREE_TYPES.AnimationClip[]
  scene: THREE_TYPES.Group
  scenes: THREE_TYPES.Group[]
  userData: THREE_TYPES.GLTF['userData']
  // ignored fields
  cameras: THREE_TYPES.Camera[]
  // immutable fields
  asset: DeepReadonly<THREE_TYPES.GLTF['asset']>
  parser: DeepReadonly<THREE_TYPES.GLTF['parser']>
};

type CacheEntry = {
  gltf: DeepReadonly<THREE_TYPES.GLTF>
  eidSet: Set<Eid>
}

const cloneScene = (scene: DeepReadonly<THREE_TYPES.Group>): THREE_TYPES.Group => {
  const clonedScene = THREE.skeletonClone(scene as THREE_TYPES.Group)
  clonedScene.animations = scene.animations.map(clip => clip.clone())
  const group = new THREE.Group()
  group.add(clonedScene)
  return group
}

const createInstance = (gltf: DeepReadonly<THREE_TYPES.GLTF>): PartialMutableGLTF => {
  const {scene} = gltf

  return {
    animations: gltf.animations.map(a => a.clone()),
    scene: cloneScene(scene),
    scenes: gltf.scenes.map(s => cloneScene(s)),
    cameras: [],  // we don't support gltf cameras
    asset: gltf.asset,
    parser: gltf.parser,
    userData: {},
  }
}
interface IGlbCache {
  get(url: string, eid: Eid): PartialMutableGLTF | undefined
  put(url: string, eid: Eid, gltf: DeepReadonly<THREE_TYPES.GLTF>): void
  has(url: string): boolean
  remove(url: string, eid: Eid): void
}

const CreateGlbCache = (): IGlbCache => {
  const gltfCache = new Map<string, CacheEntry>()

  const put = (url: string, eid: Eid, gltf: DeepReadonly<THREE_TYPES.GLTF>) => {
    const entry = gltfCache.get(url) || {
      gltf,
      eidSet: new Set<Eid>(),
    }

    entry.eidSet.add(eid)
    gltfCache.set(url, entry)
  }

  const remove = (url: string, eid: Eid): number => {
    const entry = gltfCache.get(url)
    if (!entry || !entry.eidSet) {
      return 0
    }
    entry.eidSet.delete(eid)
    const count = entry.eidSet.size

    if (count === 0) {
      gltfCache.delete(url)
      return 0
    }

    return count
  }

  const get = (url: string, eid: Eid): PartialMutableGLTF | undefined => {
    const entry = gltfCache.get(url)
    if (!entry || !entry.gltf) {
      return undefined
    }

    entry.eidSet.add(eid)
    return createInstance(entry.gltf)
  }

  return {
    get,
    put,
    has: (url: string): boolean => gltfCache.has(url),
    remove,
  }
}

export type {
  IGlbCache,
  PartialMutableGLTF,
}

export {
  CreateGlbCache,
}
