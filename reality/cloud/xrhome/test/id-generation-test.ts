import {describe, it} from 'mocha'
import {assert} from 'chai'

import {createFixedSeed, createIdSeed} from '../src/client/studio/id-generation'

describe('createIdSeed', () => {
  it('should expose the expected functions', () => {
    const seed = createIdSeed()
    assert.isDefined(seed)
    assert.isFunction(seed.fromId)
  })

  it('forId returns the same ID each time', () => {
    const seed = createIdSeed()
    const id1 = seed.fromId('something')
    const id2 = seed.fromId('something')
    assert.equal(id1, id2)
  })

  it('different seeds return different IDs', () => {
    const seed1 = createIdSeed()
    const seed2 = createIdSeed()

    const id1 = seed1.fromId('something')
    const id2 = seed2.fromId('something')
    assert.notEqual(id1, id2)
  })

  it('generates valid uuidv4 IDs', () => {
    const seed = createIdSeed()
    const id = seed.fromId('something')

    // Check if the ID matches the UUIDv4 format
    const uuidv4Regex = /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89abcd][0-9a-f]{3}-[0-9a-f]{12}$/
    assert.match(id, uuidv4Regex)
  })
})

describe('createFixedSeed', () => {
  it('should expose the expected functions', () => {
    const seed = createFixedSeed('something')
    assert.isDefined(seed)
    assert.isFunction(seed.fromId)
  })

  it('returns the same IDs for the same seed', () => {
    const seed1 = createFixedSeed('something')
    const seed2 = createFixedSeed('something')
    const id1 = seed1.fromId('something else')
    const id2 = seed2.fromId('something else')
    assert.equal(id1, id2)
  })
})
