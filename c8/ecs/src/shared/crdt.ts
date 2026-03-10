import type {DeepReadonly} from 'ts-essentials'

import * as Automerge from '@repo/c8/ecs/src/shared/automerge'

import type {
  GraphObject, PrefabInstanceChildren, PrefabInstanceDeletions, SceneGraph, Space, Spaces,
} from './scene-graph'
import {stringToBytes} from './data'

type RawSceneDoc = Automerge.Doc<SceneGraph>

type SceneDoc = {
  update: (updater: (prev: DeepReadonly<SceneGraph>) => DeepReadonly<SceneGraph>) => void
  updateAt: (
    heads: string[], updater: (prev: DeepReadonly<SceneGraph>) => DeepReadonly<SceneGraph>
  ) => Automerge.Heads | null
  updateWithoutTransform: (
    updater: (prev: DeepReadonly<SceneGraph>) => DeepReadonly<SceneGraph>
  ) => void
  distill: () => SceneGraph
  save: () => Uint8Array
  raw: () => RawSceneDoc
  clone: (actorId: string) => SceneDoc
  merge: (other: SceneDoc) => void
  rawUpdate: (updater: (prev: RawSceneDoc) => void) => void
  // TODO(christoph): Explore more about versioned timestamps rather than document references
  getChanges: (from: RawSceneDoc) => Uint8Array[]
  applyChanges: (diff: Uint8Array[]) => void
  getVersionId: () => string
  resetToVersionId: (hash: string) => void
  getView: (hash: string) => RawSceneDoc
}

// NOTE(christoph): This doc is the initial doc for all empty scenes.
// This is the recommended way to set up an initial document structure.
// https://automerge.org/docs/cookbook/modeling-data/#setting-up-an-initial-document-structure
// eslint-disable-next-line max-len
const SCENE_DOC_INIT = 'hW9KgzZTQdMAYwEC3t4Bk0NXbHEaS/pXx1M4w8G+zCHXCsXZd1wj5aTLqIOPoRMGAQIDAhMCIwJAAlYCBxUJIQIjAjQBQgJWAoABAn8AfwF/AX8AfwB/B38Hb2JqZWN0c38AfwEBfwB/AH8AAA=='

const distillScene = (doc: RawSceneDoc): SceneGraph => JSON.parse(JSON.stringify(doc))

// NOTE(christoph): Properties that are set to true here are updated directly, rather than
// being handled by a custom updater function. If we need to merge multiple separate updates to
// the property (e.g. one person changes audio pitch and another person changes volume), we need
// a custom updater function.
const UPDATE_DECISION: DeepReadonly<Record<keyof GraphObject, 'direct' | 'merge' | false>> = {
  // Never changes
  id: 'direct',  // NOTE(cindyhu): id should only be updated when deduplicating a bad merge
  ephemeral: false,

  // Properties that are updated directly
  name: 'direct',
  disabled: 'direct',
  persistent: 'direct',
  position: 'direct',
  rotation: 'direct',
  scale: 'direct',
  parentId: 'direct',
  hidden: 'direct',
  order: 'direct',
  prefab: 'direct',

  // Properties that are objects that get merged together
  light: 'merge',
  audio: 'merge',
  videoControls: 'merge',
  ui: 'merge',
  gltfModel: 'merge',
  splat: 'merge',
  collider: 'merge',
  shadow: 'merge',
  camera: 'merge',
  face: 'merge',
  material: 'merge',
  geometry: 'merge',
  location: 'merge',
  map: 'merge',
  mapTheme: 'merge',
  mapPoint: 'merge',
  imageTarget: 'merge',

  // Custom implement
  components: false,
  instanceData: false,
} as const

const DIRECT_UPDATES = (Object.keys(UPDATE_DECISION) as Array<keyof GraphObject>)
  .filter(key => UPDATE_DECISION[key] === 'direct')

const MERGED_UPDATES = (Object.keys(UPDATE_DECISION) as Array<keyof GraphObject>)
  .filter(key => UPDATE_DECISION[key] === 'merge')

const configEquals = (a: unknown, b: unknown): boolean => {
  if (a === b) {
    return true
  }

  if (Array.isArray(a) && Array.isArray(b)) {
    if (a.length !== b.length) {
      return false
    }
    for (let i = 0; i < a.length; i++) {
      if (a[i] !== b[i]) {
        return false
      }
    }
    return true
  }

  // TODO(christoph): Make this more optimized/type safe?
  if (typeof a === 'object' && typeof b === 'object') {
    return JSON.stringify(a) === JSON.stringify(b)
  }

  return false
}

const objectSafeClone = (item: unknown) => JSON.parse(JSON.stringify(item))

// NOTE(christoph): The paradigm here is that target is the automerge proxy object to mutate,
// while base and next are object references that are compared to determine what to update.
// Using base as an object reference comparison allows optimizing away portions of the scene
// that haven't changed.
const applyDirectUpdate = <K extends keyof GraphObject>(
  target: GraphObject,
  key: K,
  base: DeepReadonly<GraphObject[K]>,
  next: DeepReadonly<GraphObject[K]>
) => {
  if (configEquals(base, next)) {
    return
  }
  if (next === undefined) {
    delete target[key]
  } else {
    target[key] = objectSafeClone(next) as GraphObject[K]
  }
}

const applyObjectMergeUpdate = <T extends {}>(
  target: T,
  base: DeepReadonly<T>,
  next: DeepReadonly<T>
) => {
  if (base === next) {
    return
  }

  const baseKeys = Object.keys(base) as Array<keyof typeof base>
  const nextKeys = Object.keys(next) as Array<keyof typeof base>

  const deletedKeys = new Set()

  baseKeys.forEach((key) => {
    if (next[key] === undefined) {
      deletedKeys.add(key)
      delete target[key as keyof T]
    }
  })

  nextKeys.forEach((key) => {
    if (!deletedKeys.has(key) && !configEquals(base[key], next[key])) {
      // NOTE(christoph): This is a safe key to index on, and assigning a deep readonly reference
      // is safe because automerge won't mutate it. (see "live object references" test)
      // @ts-ignore
      target[key] = objectSafeClone(next[key])
    }
  })
}

const applyPropertyMergeUpdate = <K extends keyof GraphObject>(
  target: GraphObject,
  key: K,
  base: DeepReadonly<GraphObject[K]>,
  next: DeepReadonly<GraphObject[K]>
) => {
  if (base === next) {
    return
  }

  if (!next) {
    delete target[key]
    return
  }

  if (!base) {
    target[key] = objectSafeClone(next)
    return
  }

  // NOTE(christoph): Not seeing a good way to assert that target[key] is an object, but if
  //  UPDATE_DECISION is setup correctly, this should be fine.
  // @ts-ignore
  applyObjectMergeUpdate(target[key], base, next)
}

type Components = GraphObject['components']
const applyComponentsUpdate = (
  components: Components,
  base: DeepReadonly<Components>,
  next: DeepReadonly<Components>
) => {
  if (base === next) {
    return
  }
  const newIds = new Set<string>(Object.keys(next))
  const deletedIds = new Set<string>()
  const maybeChangedIds = new Set<string>()
  Object.keys(components).forEach((id) => {
    newIds.delete(id)
    if (next[id]) {
      maybeChangedIds.add(id)
    } else {
      deletedIds.add(id)
    }
  })

  deletedIds.forEach((id) => {
    delete components[id]
  })

  newIds.forEach((id) => {
    if (next[id]) {
      components[id] = objectSafeClone(next[id])
    }
  })

  maybeChangedIds.forEach((id) => {
    const prevComponent = base[id]
    const nextComponent = next[id]
    if (prevComponent !== nextComponent) {
      applyObjectMergeUpdate(
        components[id].parameters,
        prevComponent.parameters,
        nextComponent.parameters
      )
    }
  })
}

const mergeInstanceDeletions = (
  target: {deletions?: PrefabInstanceDeletions},
  base: DeepReadonly<PrefabInstanceDeletions>,
  next: DeepReadonly<PrefabInstanceDeletions> | undefined
) => {
  if (base === next) {
    return
  }

  if (!next) {
    if (target.deletions) {
      target.deletions = {}
    }
    return
  }

  const targetDeletions = target.deletions

  if (!targetDeletions) {
    target.deletions = objectSafeClone(next)
    return
  }

  applyObjectMergeUpdate(targetDeletions, base, next)
}

const mergeInstanceChildren = (
  target: {children?: PrefabInstanceChildren},
  base: DeepReadonly<PrefabInstanceChildren>,
  next: DeepReadonly<PrefabInstanceChildren> | undefined
) => {
  if (base === next) {
    return
  }

  if (!next) {
    if (target.children) {
      target.children = {}
    }
    return
  }

  const targetChildren = target.children

  if (!targetChildren) {
    target.children = objectSafeClone(next)
    return
  }

  applyObjectMergeUpdate(targetChildren, base, next)
}

const applyInstanceDataUpdate = (
  target: GraphObject,
  base: DeepReadonly<GraphObject>,
  next: DeepReadonly<GraphObject>
) => {
  if (base.instanceData === next.instanceData) {
    return
  }
  if (target.instanceData && base.instanceData && next.instanceData) {
    if (!target.instanceData) {
      target.instanceData = objectSafeClone(next.instanceData)
    }
    mergeInstanceDeletions(
      target.instanceData!,
      base.instanceData.deletions || {},
      next.instanceData.deletions
    )
    mergeInstanceChildren(
      target.instanceData!,
      base.instanceData.children || {},
      next.instanceData.children
    )
  } else if (next.instanceData) {
    target.instanceData = objectSafeClone(next.instanceData)
  } else {
    delete target.instanceData
  }
}

const applyObjectUpdate = (
  target: GraphObject,
  base: DeepReadonly<GraphObject>,
  next: DeepReadonly<GraphObject>,
  exclude: Array<keyof GraphObject>
) => {
  DIRECT_UPDATES.forEach((key) => {
    if (!exclude.includes(key)) {
      applyDirectUpdate(target, key, base[key], next[key])
    }
  })
  MERGED_UPDATES.forEach((key) => {
    if (!exclude.includes(key)) {
      applyPropertyMergeUpdate(target, key, base[key], next[key])
    }
  })
  applyComponentsUpdate(target.components, base.components, next.components)

  applyInstanceDataUpdate(target, base, next)
}

type ObjectMap = SceneGraph['objects']
const applyObjectsUpdate = (
  target: ObjectMap,
  base: DeepReadonly<ObjectMap>,
  next: DeepReadonly<ObjectMap>,
  exclude: Array<keyof GraphObject>
) => {
  if (base === next) {
    return
  }

  const objectIds = new Set([...Object.keys(target), ...Object.keys(next)])

  objectIds.forEach((objectId) => {
    const baseObject = base[objectId]
    const nextObject = next[objectId]
    if (baseObject !== nextObject) {
      if (baseObject) {
        if (nextObject) {
          applyObjectUpdate(target[objectId], baseObject, nextObject, exclude)
        } else {
          delete target[objectId]
        }
      } else if (nextObject) {
        target[objectId] = objectSafeClone(nextObject)
      }
    }
  })
}

const applySpaceUpdate = (
  target: Space,
  base: DeepReadonly<Space>,
  next: DeepReadonly<Space>
) => {
  if (base === next) {
    return
  }
  if (next.name !== base.name) {
    target.name = next.name
  }
  if (!configEquals(next.sky, base.sky)) {
    if (next.sky) {
      target.sky = objectSafeClone(next.sky)
    } else {
      delete target.sky
    }
  }
  if (!configEquals(next.fog, base.fog)) {
    if (next.fog) {
      target.fog = objectSafeClone(next.fog)
    } else {
      delete target.fog
    }
  }
  if (!configEquals(next.reflections, base.reflections)) {
    if (next.reflections) {
      target.reflections = objectSafeClone(next.reflections)
    } else {
      delete target.reflections
    }
  }
  if (next.activeCamera !== base.activeCamera) {
    if (next.activeCamera) {
      target.activeCamera = next.activeCamera
    } else {
      delete target.activeCamera
    }
  }
  if (!configEquals(base.includedSpaces, next.includedSpaces)) {
    if (next.includedSpaces) {
      target.includedSpaces = objectSafeClone(next.includedSpaces)
    } else {
      delete target.includedSpaces
    }
  }
}

const applySpacesUpdate = (
  target: Spaces,
  base: DeepReadonly<Spaces>,
  next: DeepReadonly<Spaces>
) => {
  if (base === next) {
    return
  }

  const spaceIds = new Set([
    ...Object.keys(base),
    ...Object.keys(next),
  ])

  spaceIds.forEach((spaceId) => {
    const baseSpace = base[spaceId]
    const nextSpace = next[spaceId]
    if (baseSpace !== nextSpace) {
      if (baseSpace) {
        if (nextSpace) {
          applySpaceUpdate(target[spaceId], baseSpace, nextSpace)
        } else {
          delete target[spaceId]
        }
      } else if (nextSpace) {
        target[spaceId] = objectSafeClone(nextSpace)
      }
    }
  })
}

const applySceneUpdate = (
  target: SceneGraph,
  base: DeepReadonly<SceneGraph>,
  next: DeepReadonly<SceneGraph>,
  exclude: Array<keyof GraphObject>
) => {
  if (target.activeCamera !== next.activeCamera) {
    if (next.activeCamera) {
      target.activeCamera = next.activeCamera
    } else {
      delete target.activeCamera
    }
  }
  if (target.activeMap !== next.activeMap) {
    if (next.activeMap) {
      target.activeMap = next.activeMap
    } else {
      delete target.activeMap
    }
  }
  if (target.entrySpaceId !== next.entrySpaceId) {
    if (next.entrySpaceId) {
      target.entrySpaceId = next.entrySpaceId
    } else {
      delete target.entrySpaceId
    }
  }

  // NOTE(Johnny) We want more granular merges in the future for inputs and sky.
  if (!configEquals(base.inputs, next.inputs)) {
    if (next.inputs) {
      target.inputs = objectSafeClone(next.inputs)
    } else {
      delete target.inputs
    }
  }
  if (!configEquals(base.sky, next.sky)) {
    if (next.sky) {
      target.sky = objectSafeClone(next.sky)
    } else {
      delete target.sky
    }
  }
  if (!configEquals(next.reflections, base.reflections)) {
    if (next.reflections) {
      target.reflections = objectSafeClone(next.reflections)
    } else {
      delete target.reflections
    }
  }
  if (!configEquals(base.runtimeVersion, next.runtimeVersion)) {
    if (next.runtimeVersion) {
      target.runtimeVersion = objectSafeClone(next.runtimeVersion)
    } else {
      delete target.runtimeVersion
    }
  }
  applyObjectsUpdate(target.objects, base.objects, next.objects, exclude)

  if (base.spaces !== next.spaces) {
    if (next.spaces) {
      if (base.spaces && target.spaces) {
        applySpacesUpdate(target.spaces, base.spaces, next.spaces)
      } else {
        target.spaces = objectSafeClone(next.spaces)
      }
    } else {
      delete target.spaces
    }
  }
}

const maybeFixDuplicatedString = (merged: string, yours: string | null, theirs: string | null) => {
  if (yours === null || merged === theirs) {
    return theirs
  }
  if (theirs === null || merged === yours) {
    return yours
  }
  return yours
}

type Json = string | number | boolean | null | undefined | {[key: string]: Json} | Json[]

const fixStringDuplication = <T extends Json>(merged: T, yours: T | null, theirs: T | null) => {
  switch (typeof merged) {
    case 'string':
      return maybeFixDuplicatedString(
        merged,
        typeof yours === 'string' ? yours : null,
        typeof theirs === 'string' ? theirs : null
      )
    case 'object': {
      if (!merged) {
        return merged
      }
      if (Array.isArray(merged)) {
        return merged.map((item, i): typeof merged[number] => fixStringDuplication(item,
          Array.isArray(yours) ? yours?.[i] : null,
          Array.isArray(theirs) ? theirs?.[i] : null))
      }
      const newData = {} as any
      Object.keys(merged).forEach((key) => {
        newData[key] = fixStringDuplication(
          merged[key],
          (typeof yours === 'object' && !Array.isArray(yours)) ? yours?.[key] : null,
          (typeof theirs === 'object' && !Array.isArray(theirs)) ? theirs?.[key] : null
        )
      })
      return newData
    }
    default:
      return merged
  }
}

const createWrappedDoc = (_initialDoc: RawSceneDoc): SceneDoc => {
  let doc_ = _initialDoc

  return {
    distill: () => distillScene(doc_),
    save: () => Automerge.save(doc_),
    raw: () => doc_,
    clone: (actorId: string) => createWrappedDoc(
      Automerge.clone(doc_, {actor: actorId || undefined})
    ),
    merge: (other: SceneDoc) => {
      const yours = doc_
      const theirs = other.raw()
      doc_ = Automerge.merge(doc_, theirs)
      const deduplicated = fixStringDuplication(
        doc_ as any as Json,
        yours as any as Json,
        theirs as any as Json
      )
      doc_ = Automerge.change(doc_, (d) => {
        applySceneUpdate(d, distillScene(doc_), deduplicated, [])
      })
    },
    update: (updater) => {
      const previousScene = distillScene(doc_)
      const nextScene = updater(previousScene)
      doc_ = Automerge.change(doc_, (d) => {
        applySceneUpdate(d, previousScene, nextScene, [])
      })
    },
    updateAt: (heads, updater) => {
      const previousView = Automerge.view(doc_, heads)
      const previousScene = distillScene(previousView)
      const nextScene = updater(previousScene)
      const {newDoc, newHeads} = Automerge.changeAt(doc_, heads, (d) => {
        applySceneUpdate(d, previousScene, nextScene, [])
      })
      doc_ = newDoc
      return newHeads
    },
    updateWithoutTransform: (updater) => {
      const previousScene = distillScene(doc_)
      const nextScene = updater(previousScene)
      doc_ = Automerge.change(doc_, (d) => {
        applySceneUpdate(d, previousScene, nextScene, ['position', 'rotation', 'scale'])
      })
    },
    rawUpdate: (updater) => {
      doc_ = Automerge.change(doc_, updater)
    },
    getChanges: from => Automerge.getChanges(from, doc_),
    applyChanges: (changes) => {
      if (changes.length === 0) {
        return
      }
      doc_ = Automerge.applyChanges(doc_, changes).at(-1)!
    },
    getVersionId: () => {
      const heads = Automerge.getHeads(doc_)
      if (heads.length !== 1) {
        throw new Error('Expected exactly one head')
      }
      return heads[0]
    },
    resetToVersionId: (hash: string) => {
      const view = Automerge.view(doc_, [hash])  // Read only view
      doc_ = Automerge.clone(view)
    },
    getView: (hash: string) => Automerge.view(doc_, [hash]),
  }
}

const loadSceneDoc = (bytes: Uint8Array, actorId: string): SceneDoc => (
  createWrappedDoc(Automerge.load(bytes, {actor: actorId || undefined}))
)

const createEmptySceneDoc = (actorId: string) => {
  const raw = Automerge.load<SceneGraph>(
    stringToBytes(SCENE_DOC_INIT), {actor: actorId || undefined}
  )
  return createWrappedDoc(raw)
}

export {
  createEmptySceneDoc,
  loadSceneDoc,
  fixStringDuplication,
}

export type {
  SceneDoc,
  RawSceneDoc,
  Json,
}
