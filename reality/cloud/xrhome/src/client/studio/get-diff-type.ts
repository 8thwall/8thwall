import type {Expanse} from '@ecs/shared/scene-graph'

import type {DeepReadonly, Primitive} from 'ts-essentials'

import {isObject, type ChangeLog} from './get-change-log'

type Added<T> = {
  type: 'added'
  before: DeepReadonly<T>
  after: DeepReadonly<T>
}

type Removed<T> = {
  type: 'removed'
  before: DeepReadonly<T>
  after: DeepReadonly<T>
}

type ChangedDirectly<T> = {
  type: 'changedDirectly'
  before: DeepReadonly<T>
  after: DeepReadonly<T>
}

type SubfieldsChanged<T> = {
  type: 'subfieldsChanged'
  before: DeepReadonly<T>
  after: DeepReadonly<T>
}

type Unchanged<T> = {
  type: 'unchanged'
  before: DeepReadonly<T>
  after: DeepReadonly<T>
}

type DiffType<T> = Added<T> | Removed<T> | ChangedDirectly<T> | SubfieldsChanged<T> | Unchanged<T>

const isPrefixOf = (maybePrefix: Readonly<string[]>, path: Readonly<string[]>): boolean => {
  if (maybePrefix.length === 0) return true
  if (path.length < maybePrefix.length) return false
  else {
    // TODO(Carson): Should early exit, but check this function if performance issues
    const existsMismatch = maybePrefix.some((field, index) => {
      if (path[index] !== field) {
        return true
      }
      return false
    })
    return !existsMismatch
  }
}

const isStrictPrefixOf = (maybePrefix: Readonly<string[]>, path: Readonly<string[]>): boolean => (
  maybePrefix.length < path.length && isPrefixOf(maybePrefix, path)
)

const arrayEq = (a: DeepReadonly<string[]>, b: DeepReadonly<string[]>): boolean => {
  if (a.length !== b.length) {
    return false
  }
  return a.every((value, index) => value === b[index])
}

const getFromPath = (path: DeepReadonly<string[]>, obj: Object): unknown => {
  if (path.length === 0) {
    return obj
  }
  if (obj === undefined) {
    return obj
  }
  return path.reduce((current: any, key) => {
    if (current == null || typeof current !== 'object' || !(key in current)) {
      return undefined
    }
    return current[key]
  }, obj)
}

function getDiffTypeDirect<T>(
  before: DeepReadonly<T>,
  after: DeepReadonly<T>
): DiffType<T> {
  if (before === after) {
    return {
      type: 'unchanged',
      before,
      after,
    }
  }
  if (before === undefined && after !== undefined) {
    return {
      type: 'added',
      before,
      after,
    }
  }
  if (before !== undefined && after === undefined) {
    return {
      type: 'removed',
      before,
      after,
    }
  } else {
    return {
      type: 'changedDirectly',
      before,
      after,
    }
  }
}

const getDiffType = (
  // eslint-disable-next-line arrow-parens
  changeLog: DeepReadonly<ChangeLog>,
  beforeScene: DeepReadonly<Expanse>,
  afterScene: DeepReadonly<Expanse>,
  path: string[],
  // If you want to default a non-primitive value,
  // just use renderDiff on the object itself.
  undefinedDefault?: DeepReadonly<Primitive>
): DiffType<unknown> => {
  const {deletedPaths, addedPaths, updatedPaths} = changeLog

  // SubFieldChanged iff the path is a subpath of a updated, deleted, or added path,
  // since these are consolidated in the change log. This is because the field itself
  // can not have been directly changed or deleted, since it is an object if it has
  // subfields.
  const subUpdated = updatedPaths.some(updatedPath => isStrictPrefixOf(path, updatedPath))
  const subDeleted = deletedPaths.some(deletedPath => isStrictPrefixOf(path, deletedPath))
  const subAdded = addedPaths.some(addedPath => isStrictPrefixOf(path, addedPath))

  const subfieldsChanged = subUpdated || subDeleted || subAdded

  const beforeFromPath = getFromPath(path, beforeScene)
  const afterFromPath = getFromPath(path, afterScene)

  const before = beforeFromPath === undefined ? undefinedDefault : beforeFromPath
  const after = afterFromPath === undefined ? undefinedDefault : afterFromPath

  if (subfieldsChanged) {
    return {
      type: 'subfieldsChanged',
      before,
      after,
    }
  }

  const isContinuationOfAdded = addedPaths.some(
    addedPath => isStrictPrefixOf(addedPath, path)
  )
  const isContinuationOfDeleted = deletedPaths.some(
    deletedPath => isStrictPrefixOf(deletedPath, path)
  )

  // In some cases with the defaults, we may swap directly from an undefined value to its
  // represented value. E.g. a number field being undefined "means" it is 0, but then it
  // actually is set to 0. Actually comparing the values is then the best way to determine
  // if it is a change or not. We don't want to do this for objects, since it is expensive,
  // and also does not match the behavior we want with bubbling up the diff chip. For the
  // same reason we don't handle continuations of added or deleted paths, since they should
  // bubble up to the shortest path.
  if (
    !isContinuationOfAdded && !isContinuationOfDeleted && !isObject(before) && !isObject(after)
  ) {
    return getDiffTypeDirect(
      before,
      after
    )
  }

  // If the path is a subpath of a deleted or added path, then it will return unchanged,
  // since we want to display the chip as high as possible.
  if (addedPaths.some(addedPath => arrayEq(path, addedPath))) {
    return {
      type: 'added',
      before,
      after,
    }
  }
  if (deletedPaths.some(deletedPath => arrayEq(path, deletedPath))) {
    return {
      type: 'removed',
      before,
      after,
    }
  }
  if (updatedPaths.some(updatedPath => arrayEq(path, updatedPath))) {
    return {
      type: 'changedDirectly',
      before,
      after,
    }
  }
  return {
    type: 'unchanged',
    before,
    after,
  }
}

export {
  isPrefixOf,
  isStrictPrefixOf,
  getFromPath,
  getDiffType,
  getDiffTypeDirect,
}

export type {
  DiffType,
  Added,
  Removed,
  ChangedDirectly,
  SubfieldsChanged,
  Unchanged,
}
