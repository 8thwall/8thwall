import {resolveSpaceForObject} from '@ecs/shared/object-hierarchy'
import type {BaseGraphObject, Expanse, GraphObject, Space} from '@ecs/shared/scene-graph'
import {deletedDiff, diff, updatedDiff} from 'deep-object-diff'
import type {DeepReadonly} from 'ts-essentials'

type UpdatedPaths = string[][]

// @ts-ignore Inductive definition
type Json = Record<string, Json> | Json[] | string | number | boolean | null

type SpaceChangeRecord = {
  type: 'added' | 'removed' | 'changed'
  name: Space['name']
  changedObjectCount: number
}

type ChangeLog = {
  objectUpdates: Record<GraphObject['id'], UpdatedPaths>
  objectDeletions: Record<GraphObject['id'], GraphObject>
  objectAdditions: Record<GraphObject['id'], GraphObject>
  spaceChanges: Record<Space['id'], SpaceChangeRecord>
  deletedPaths: UpdatedPaths
  updatedPaths: UpdatedPaths
  addedPaths: UpdatedPaths
}

const isObject = (obj: unknown) => obj !== null && typeof obj === 'object'

const getLeafPathsStrong = (srcKey: string, obj: Record<string, Json>): UpdatedPaths => {
  // Subsumes array case, but must be careful since null, etc, have typeof object
  // Also, empty objects should fall into the other bucket, so they can be recorded,
  // since they will return an empty array for subPaths
  if (isObject(obj) && Object.keys(obj).length > 0) {
    const subPaths = Object.entries(obj).map(
      ([k, v]) => getLeafPathsStrong(k, v)
    ).reduce(
      (acc, paths) => [...acc, ...paths], []
    )
    // paths are reverse accumulators
    subPaths.forEach(path => path.push(srcKey))
    return subPaths
  } else {
    return [[srcKey]]
  }
}

// Provides an array of paths to the leaves of a JSON object.
// e.g. { x: {y: 'z'}, a: 'b' } -> [['x','y'], ['a']]
const getLeafPaths = (obj: Json): UpdatedPaths => {
  if (Object.keys(obj).length === 0) {
    return []
  }

  return getLeafPathsStrong('', obj).map(
    (path: string[]) => {
      path.splice(path.length - 1)
      return path.reverse()
    }
  )
}

// Returns a \ b
// eslint-disable-next-line arrow-parens
const setDiff = <T>(a: Set<T>, b: Set<T>): Set<T> => {
  let diffSet: Set<T> = new Set()
  a.forEach((aElem) => {
    if (!b.has(aElem)) {
      diffSet = diffSet.add(aElem)
    }
  })
  return diffSet
}

// eslint-disable-next-line arrow-parens
const setIntersect = <T>(a: Set<T>, b: Set<T>): Set<T> => {
  let intersectSet: Set<T> = new Set()
  a.forEach((aElem) => {
    if (b.has(aElem)) {
      intersectSet = intersectSet.add(aElem)
    }
  })
  return intersectSet
}

// leafPaths called on the objects field of the diff
const getUpdatedObjectIds = (
  changePaths: UpdatedPaths,
  addedObjectIds: Set<string>,
  deletedObjectIds: Set<string>
) => (
  changePaths.reduce((acc, path) => {
    // paranoia
    if (path.length === 0) {
      return acc
    }
    // for the objects field, objId is the key
    const objId = path[0]
    if (!addedObjectIds.has(objId) && !deletedObjectIds.has(objId)) {
      acc.add(objId)
    }
    return acc
  }, new Set())
)

const getChangedSpaces = (
  prevExpanse: DeepReadonly<Expanse>,
  nextExpanse: DeepReadonly<Expanse>,
  addedObjects: Record<BaseGraphObject['id'], GraphObject>,
  deletedObjects: Record<BaseGraphObject['id'], GraphObject>,
  updatedObjects: Record<BaseGraphObject['id'], UpdatedPaths>
): ChangeLog['spaceChanges'] => {
  const addedObjectSpaces = Object.keys(addedObjects).map(
    objId => resolveSpaceForObject(nextExpanse, objId)?.id
  )
  const deletedObjectSpaces = Object.keys(deletedObjects).map(
    objId => resolveSpaceForObject(prevExpanse, objId)?.id
  )

  // Objects can move between spaces, so we want to count this as a change for both,
  // but be careful not to double count them.
  const updatedObjectSpacesUncounted = Object.keys(updatedObjects).flatMap(
    (objId) => {
      const beforeSpace = resolveSpaceForObject(prevExpanse, objId)
      const afterSpace = resolveSpaceForObject(nextExpanse, objId)
      if (beforeSpace?.id === afterSpace?.id) {
        return [beforeSpace?.id]
      }
      return [beforeSpace?.id, afterSpace?.id]
    }
  )
  const startingSpaces = new Set(Object.keys(prevExpanse.spaces ?? {}))
  const endingSpaces = new Set(Object.keys(nextExpanse.spaces ?? {}))

  const addedSpaces = setDiff(endingSpaces, startingSpaces)
  const removedSpaces = setDiff(startingSpaces, endingSpaces)

  // TODO(Carson): Include check for space properties having changed,
  // not just objects after adding chips to space level configurator

  const changedSpaces = [
    ...addedObjectSpaces,
    ...deletedObjectSpaces,
    ...updatedObjectSpacesUncounted,
  ].filter(
    spaceId => Boolean(spaceId) && !addedSpaces.has(spaceId) && !removedSpaces.has(spaceId)
  )

  const changedSpacesCounter = changedSpaces.reduce((acc, spaceId) => {
    acc[spaceId] = (acc[spaceId] || 0) + 1
    return acc
  }, {} as Record<string, number>)

  const spaceChanges: ChangeLog['spaceChanges'] = {}

  addedSpaces.forEach((spaceId) => {
    const spaceName = nextExpanse.spaces[spaceId].name
    spaceChanges[spaceId] = {type: 'added', name: spaceName, changedObjectCount: 0}
  })

  removedSpaces.forEach((spaceId) => {
    const spaceName = prevExpanse.spaces[spaceId].name
    spaceChanges[spaceId] = {type: 'removed', name: spaceName, changedObjectCount: 0}
  })

  changedSpaces.forEach((spaceId) => {
    const spaceName = nextExpanse.spaces[spaceId].name
    spaceChanges[spaceId] = {
      type: 'changed',
      name: spaceName,
      changedObjectCount: changedSpacesCounter[spaceId] || 0,
    }
  })

  return spaceChanges
}

// NOTE(Carson): This function is necessary, since the default diff library's getAdded
// has the behavior where it returns paths to the entire object, not the
// smallest paths, so e.g. {x: {y: 'z'}} -> [['x', 'y']] instead of [['x']]
// For scene diff, we want the consolidated paths, so we can show the chips
// at the top level, not the leaf level.
const getConsolidatedAdditionPaths = (start: object, end: object): UpdatedPaths => {
  if (!isObject(start) || !isObject(end)) {
    return []
  }

  const startKeys = new Set(Object.keys(start))
  const endKeys = new Set(Object.keys(end))
  const addedKeys = Array.from(setDiff(endKeys, startKeys))
  const sharedKeys = Array.from(setIntersect(endKeys, startKeys))
  const addedPaths = addedKeys.map(key => [key])
  const sharedPaths = sharedKeys.flatMap((key) => {
    const startObj = start[key]
    const endObj = end[key]
    const keyAdditionPaths = getConsolidatedAdditionPaths(startObj, endObj)
    return keyAdditionPaths.map(path => [key, ...path])
  })
  return [...addedPaths, ...sharedPaths]
}

const clone = (x: Json) => JSON.parse(JSON.stringify(x))

const getChangeLog = (
  oldExpanse: DeepReadonly<Expanse>,
  newExpanse: DeepReadonly<Expanse>
): ChangeLog => {
  const {objects: oldObjects} = oldExpanse
  const {objects: newObjects} = newExpanse

  const objectDiff = diff(oldObjects, newObjects)
  const objectDiffPaths = getLeafPaths(objectDiff)

  const prevObjectIds = new Set(Object.keys(oldExpanse.objects))
  const nextObjectIds = new Set(Object.keys(newExpanse.objects))
  const addedObjectIdSet = setDiff(nextObjectIds, prevObjectIds)
  const deletedObjectIdSet = setDiff(prevObjectIds, nextObjectIds)
  const updatedObjectIds = getUpdatedObjectIds(
    objectDiffPaths, addedObjectIdSet, deletedObjectIdSet
  )
  const addedObjectIds = Array.from(addedObjectIdSet)
  const deletedObjectIds = Array.from(deletedObjectIdSet)

  const addMap: Record<BaseGraphObject['id'], GraphObject> = Object.fromEntries(
    addedObjectIds.map(
      id => [id, clone(newObjects[id])]
    )
  )
  const deleteMap: Record<BaseGraphObject['id'], GraphObject> = Object.fromEntries(
    deletedObjectIds.map(
      id => [id, clone(oldObjects[id])]
    )
  )

  const updateMap: Record<BaseGraphObject['id'], UpdatedPaths> = objectDiffPaths.reduce(
    (acc, path) => {
      const [oId, ...rest] = path
      if (path.length === 0 || !updatedObjectIds.has(oId)) {
        return acc
      }

      // first entry in the object path is the object id
      if (!acc[oId]) {
        acc[oId] = []
      }
      acc[oId].push(rest)
      return acc
    }, {}
  )

  const deletedPaths = getLeafPaths(deletedDiff(oldExpanse, newExpanse))
  const updatedPaths = getLeafPaths(updatedDiff(oldExpanse, newExpanse))
  const addedPaths = getConsolidatedAdditionPaths(oldExpanse, newExpanse)
  const spaceChanges = getChangedSpaces(
    oldExpanse, newExpanse, addMap, deleteMap, updateMap
  )

  return {
    // For left panel tags
    // TODO(Carson): Include or remove objects with different parent ids?
    objectAdditions: addMap,
    objectDeletions: deleteMap,
    objectUpdates: updateMap,
    spaceChanges,
    // Right panel tags
    deletedPaths,
    updatedPaths,
    addedPaths,
  }
}

export {
  getLeafPaths,
  getChangeLog,
  isObject,
}

export type {
  ChangeLog,
  UpdatedPaths,
}
