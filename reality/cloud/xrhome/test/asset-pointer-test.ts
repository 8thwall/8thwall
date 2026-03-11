import {describe, it} from 'mocha'
import chai from 'chai'

import {
  isValid, isSingle, isBundle, parse, serialize, getPath, getMainPath, getSinglePath,
} from '../src/shared/asset-pointer'

chai.should()
const {assert} = chai

/* ////////// Example pointers ////////// */

// Single pointer - Points to a single normal asset which is just stored as the path

const singlePointer = {
  type: 'single',
  path: '/assets/blah.png',
}

const singlePointerString = '/assets/blah.png'

const singlePath = '/assets/blah.png'

// GLTF pointer - Points to multiple files at a shared path with a main file which is the gltf

const gltfPointer = {
  type: 'bundle',
  path: '/assets/bundle-ezj4at6c55/',
  main: 'palm_tree_02.gltf',
  files: {
    'palm_tree_02.gltf': '/assets/palm_tree-abc.gltf',
    'palm_tree_02.bin': '/assets/palm_tree-def.bin',
  },
}

const gltfPointerString = [
  '{"type":"bundle","path":"/assets/bundle-ezj4at6c55/","main":"palm_tree_02.gltf",',
  '"files":{"palm_tree_02.gltf":"/assets/palm_tree-abc.gltf","palm_tree_02.bin":',
  '"/assets/palm_tree-def.bin"}}',
].join('')

const gltfPath = '/assets/bundle-ezj4at6c55/palm_tree_02.gltf'

// Cubemap pointer - Points to multiple files, without a main file

const cubemapPointer = {
  type: 'bundle',
  assetType: 'cubemap',
  path: '/assets/789/',
  files: {
    'negz.png': '/assets/negz-123.png',
    'posz.png': '/assets/posz-456.png',
    'negx.png': '/assets/negx-789.png',
  },
}

const cubemapPath = '/assets/789/'

/* ////////// Tests ////////// */

describe('Asset Pointers isValid', () => {
  it('Rejects bad types', () => {
    assert.isFalse(isValid(null))
    assert.isFalse(isValid(undefined))
    assert.isFalse(isValid(singlePointerString))
    assert.isFalse(isValid(gltfPointerString))
  })

  it('Succeeds on good types', () => {
    assert.isTrue(isValid(singlePointer))
    assert.isTrue(isValid(gltfPointer))
    assert.isTrue(isValid(cubemapPointer))
  })
})

describe('Asset Pointers isSingle', () => {
  it('False for non-singles', () => {
    assert.isFalse(isSingle(gltfPointer))
    assert.isFalse(isSingle(cubemapPointer))
  })

  it('True for single', () => {
    assert.isTrue(isSingle(singlePointer))
  })
})

describe('Asset Pointers isBundle', () => {
  it('False for non-bundles', () => {
    assert.isFalse(isBundle(singlePointer))
  })

  it('True for single', () => {
    assert.isTrue(isBundle(gltfPointer))
    assert.isTrue(isBundle(cubemapPointer))
  })
})

describe('Asset Pointers parse', () => {
  it('Rejects with null', () => {
    assert.isNull(parse(null))
    assert.isNull(parse(undefined))
    assert.isNull(parse(''))
    assert.isNull(parse({}))
  })

  it('Succeeds on good types', () => {
    assert.deepEqual(singlePointer, parse(singlePointerString))
    assert.deepEqual(gltfPointer, parse(gltfPointerString))
  })
})

describe('Asset Pointers serialize', () => {
  it('Rejects with null', () => {
    assert.isNull(serialize(null))
    assert.isNull(serialize(undefined))
    assert.isNull(serialize(''))
    assert.isNull(serialize({}))
  })

  it('Succeeds on good types', () => {
    assert.deepEqual(serialize(singlePointer), singlePointerString)
    assert.deepEqual(serialize(gltfPointer), gltfPointerString)
  })
})

describe('Asset Pointers getPath', () => {
  it('Returns null for bad args', () => {
    assert.isNull(getPath(null))
    assert.isNull(getPath(undefined))
    assert.isNull(getPath(''))
    assert.isNull(getPath({}))
    assert.isNull(getPath('{}'))
  })

  it('Works for pointers', () => {
    assert.deepEqual(getPath(singlePointer), singlePath)
    assert.deepEqual(getPath(gltfPointer), gltfPath)
    assert.deepEqual(getPath(cubemapPointer), cubemapPath)
  })

  it('Works equally for pointer strings', () => {
    assert.deepEqual(getPath(singlePointerString), singlePath)
    assert.deepEqual(getPath(gltfPointerString), gltfPath)
  })
})

describe('Asset Pointers getMainPath', () => {
  it('Returns null for bad args', () => {
    assert.isNull(getMainPath(null))
    assert.isNull(getMainPath(undefined))
    assert.isNull(getMainPath(''))
    assert.isNull(getMainPath({}))
    assert.isNull(getMainPath('{}'))
  })

  it('Null for bundle without main', () => {
    assert.isNull(getMainPath(cubemapPointer))
  })

  it('Works for pointers', () => {
    assert.deepEqual(getMainPath(singlePointer), singlePath)
    assert.deepEqual(getMainPath(gltfPointer), gltfPath)
  })

  it('Works equally for pointer strings', () => {
    assert.deepEqual(getMainPath(singlePointerString), singlePath)
    assert.deepEqual(getMainPath(gltfPointerString), gltfPath)
  })
})

describe('Asset Pointers getSinglePath', () => {
  it('Returns null for bad args', () => {
    assert.isNull(getSinglePath(null))
    assert.isNull(getSinglePath(undefined))
    assert.isNull(getSinglePath(''))
    assert.isNull(getSinglePath({}))
    assert.isNull(getSinglePath('{}'))
  })

  it('Null for non-single pointers', () => {
    assert.isNull(getSinglePath(cubemapPointer))
    assert.isNull(getSinglePath(gltfPointer), gltfPath)
  })

  it('Works for single pointers', () => {
    assert.deepEqual(getSinglePath(singlePointer), singlePath)
  })

  it('Works equally for single pointer strings', () => {
    assert.deepEqual(getSinglePath(singlePointerString), singlePath)
  })
})
