/*
  An asset pointer represents a single asset or grouping of assets, which look like:
    {type: 'single', path: string}
    or
    {type: 'bundle', path: string, main?: string, files: Record<string, string>}

  Example pointers can be found at:
    xrhome/test/asset-pointer-test.ts
*/

interface SingleAssetPointer {
  type: 'single'
  path: string
}

interface BundleAssetPointer {
  type: 'bundle'
  path: string
  main?: string
  files: Record<string, string>
}

type AssetPointer = SingleAssetPointer | BundleAssetPointer

type PointerOrString = AssetPointer | string

const isValid = (pointer: any): pointer is AssetPointer => (
  !!pointer && typeof pointer === 'object' && typeof pointer.type !== 'undefined'
)

const isSingle = (pointer: any): pointer is SingleAssetPointer => (
  isValid(pointer) && pointer.type === 'single'
)

const isBundle = (pointer: any): pointer is BundleAssetPointer => (
  isValid(pointer) && pointer.type === 'bundle'
)

const getBundleId = (pointer: AssetPointer): string | null => {
  if (isBundle(pointer)) {
    const match = pointer.path.match(/\/assets\/bundles\/bundle-([a-z0-9]*)\/$/)
    if (match) {
      const [, bundleId] = match
      return bundleId
    }
  }
  return null
}

// Take a string and return a valid asset pointer, or null if not valid
const parse = (pointerString: string): AssetPointer | null => {
  if (!pointerString || typeof pointerString !== 'string') {
    return null
  }

  if (pointerString[0] === '{') {
    try {
      return JSON.parse(pointerString) as AssetPointer
    } catch (err) {
      return null
    }
  }

  return {type: 'single', path: pointerString}
}

// Take an asset pointer and return a string, or null if not valid
const serialize = (pointer: AssetPointer) => {
  if (!isValid(pointer)) {
    return null
  }

  if (isSingle(pointer)) {
    return pointer.path
  }

  return JSON.stringify(pointer)
}

// Get the path of either the main file, or if not set, the enclosing folder for a bundle
const getPath = (pointer: AssetPointer) => {
  if (isSingle(pointer)) {
    return pointer.path
  }

  if (isBundle(pointer)) {
    return pointer.path + (pointer.main || '')
  }

  return null
}

// Get the path of a single file that is denoted as the main file of the bundle
const getMainPath = (pointer: AssetPointer) => {
  if (!isSingle(pointer) && !(isBundle(pointer) && pointer.main)) {
    return null
  }

  return getPath(pointer)
}

// Get the path of the file pointed to by the pointer, as long as the pointer only has one file
const getSinglePath = (pointer: AssetPointer) => {
  if (!isSingle(pointer)) {
    return null
  }
  return getPath(pointer)
}

type PointerFunction<T, A extends any[]> = (pointer: AssetPointer, ...args: A) => T

// Allow for pointer object or string to be passed into the specified functions by parsing first
const maybeParseBefore = <T, A extends any[]>(functionTakingPointer: PointerFunction<T, A>) => (
  pointerOrString: PointerOrString,
  ...rest: A
) => {
  if (!pointerOrString) {
    return null
  }

  let parsedPointer: AssetPointer | null
  if (isValid(pointerOrString)) {
    parsedPointer = pointerOrString
  } else {
    parsedPointer = parse(pointerOrString)
    if (!parsedPointer) {
      return null
    }
  }

  return functionTakingPointer(parsedPointer, ...rest)
}

const wrappedGetPath = maybeParseBefore(getPath)
const wrappedGetMainPath = maybeParseBefore(getMainPath)
const wrappedGetSinglePath = maybeParseBefore(getSinglePath)

export {
  isValid,
  isSingle,
  isBundle,
  getBundleId,
  parse,
  serialize,
  wrappedGetPath as getPath,
  wrappedGetMainPath as getMainPath,
  wrappedGetSinglePath as getSinglePath,
}

export type {
  AssetPointer,
  SingleAssetPointer,
  BundleAssetPointer,
  PointerOrString,
  PointerFunction,
}
