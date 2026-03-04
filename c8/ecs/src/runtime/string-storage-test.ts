// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, beforeEach, afterEach, assert} from '@nia/bzl/js/chai-js'

import {globalStringMap} from './string-storage'

// eslint-disable-next-line no-console
const originalWarn = console.warn

describe('String Storage', () => {
  const warnings: unknown[] = []

  beforeEach(() => {
    warnings.length = 0
    // eslint-disable-next-line no-console
    console.warn = warnings.push.bind(warnings)
  })

  afterEach(() => {
    // eslint-disable-next-line no-console
    console.warn = originalWarn
  })

  it('can store and load a string', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)

    const id = storage.store('hello')
    assert.notStrictEqual(id, 0)
    const retrieved = storage.get(id)
    assert.strictEqual(retrieved, 'hello')

    assert.isEmpty(warnings)
  })

  it('returns empty string for id 0', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)

    const retrieved = storage.get(0)
    assert.strictEqual(retrieved, '')
    assert.isEmpty(warnings)
  })

  it('converts numbers to string with a warning', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)

    const id = storage.store(123)
    assert.notEqual(id, 0)
    const retrieved = storage.get(id)
    assert.strictEqual(retrieved, '123')
    assert.deepStrictEqual(warnings, [
      'Non-string value passed to string field, values will be converted to string.',
    ])
  })

  it('converts 0 to string with a warning', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)
    const id = storage.store(0)
    assert.notEqual(id, 0)
    const retrieved = storage.get(id)
    assert.strictEqual(retrieved, '0')
    assert.deepStrictEqual(warnings, [
      'Non-string value passed to string field, values will be converted to string.',
    ])
  })

  it('converts objects to string with a warning', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)

    const id = storage.store({key: 'value'})
    assert.notEqual(id, 0)
    const retrieved = storage.get(id)
    assert.strictEqual(retrieved, '[object Object]')
    assert.deepStrictEqual(warnings, [
      'Non-string value passed to string field, values will be converted to string.',
    ])
  })

  it('only warns once', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)

    storage.store(123)
    storage.store(456)
    storage.store({key: 'value'})

    assert.deepStrictEqual(warnings, [
      'Non-string value passed to string field, values will be converted to string.',
    ])
  })

  it('converts null/undefined to empty string with warning', () => {
    const world = {} as any  // Mock World object
    const storage = globalStringMap(world)
    const nullId = storage.store(null)
    const undefinedId = storage.store(undefined)
    assert.strictEqual(nullId, 0)
    assert.strictEqual(undefinedId, 0)
  })
})
