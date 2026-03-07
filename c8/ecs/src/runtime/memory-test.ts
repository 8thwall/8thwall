// @package(npm-ecs)
// @attr(externalize_npm = 1)

import {describe, it, assert} from '@nia/bzl/js/chai-js'

import {
  getDefaults, getSchemaAlignment, getSchemaSize, toOrderedSchema,
} from './memory'

describe('toOrderedSchema', () => {
  it('returns empty array for undefined schema', () => {
    assert.deepEqual(toOrderedSchema(), [])
  })
  it('returns empty array for empty schema', () => {
    assert.deepEqual(toOrderedSchema({}), [])
  })
  it('properly lays out fields in memory', () => {
    assert.deepEqual(toOrderedSchema({
      f1: 'f32',
      f2: 'f64',
    }), [['f2', 'f64', 0], ['f1', 'f32', 8]])
  })
  it('ensures each field is properly aligned', () => {
    assert.deepEqual(toOrderedSchema({
      f1: 'f32',
      f2: 'f64',
      f3: 'ui8',
      f4: 'ui8',
      f5: 'ui8',
    }), [
      ['f2', 'f64', 0],
      ['f1', 'f32', 8],
      ['f3', 'ui8', 12],
      ['f4', 'ui8', 13],
      ['f5', 'ui8', 14],
    ])
  })
})

describe('getSchemaSize', () => {
  it('returns 1 for empty schema', () => {
    assert.deepEqual(getSchemaSize([]), 1)
  })
  it('properly lays out fields in memory', () => {
    assert.deepEqual(getSchemaSize([['f2', 'f64', 0], ['f1', 'f32', 8]]), 12)
  })
  it('ensures each field is properly aligned', () => {
    assert.deepEqual(getSchemaSize([
      ['f2', 'f64', 0],
      ['f1', 'f32', 8],
      ['f3', 'ui8', 12],
      ['f4', 'ui8', 13],
      ['f5', 'ui8', 14],
    ]), 15)
  })
  it('handles padding bytes properly', () => {
    assert.deepEqual(getSchemaSize([
      ['f2', 'ui8', 0],
      // Empty 3 bytes between here
      ['f1', 'f32', 4],
    ]), 8)
  })
})

describe('getSchemaAlignment', () => {
  it('returns 1 for empty schema', () => {
    assert.deepEqual(getSchemaAlignment([]), 1)
  })
  it('aligns ui8 to 1 byte', () => {
    assert.deepEqual(getSchemaAlignment([['f1', 'ui8', 0], ['f2', 'ui8', 1]]), 1)
  })
  it('aligns eid to 8 bytes', () => {
    assert.deepEqual(getSchemaAlignment([['f1', 'eid', 0], ['f2', 'eid', 4]]), 8)
  })
  it('aligns mixed fields to largest field', () => {
    assert.deepEqual(getSchemaAlignment([['f2', 'f64', 0], ['f1', 'f32', 8]]), 8)
  })
})

describe('getDefaults', () => {
  it('handles empty schema', () => {
    assert.deepEqual(getDefaults(), {})
  })
  it('returns the correct values for each type', () => {
    assert.deepEqual(getDefaults({
      f1: 'f32',
      f2: 'f64',
      f3: 'i32',
      f4: 'ui8',
      f5: 'ui32',
      f6: 'eid',
      f7: 'string',
    }), {
      f1: 0,
      f2: 0,
      f3: 0,
      f4: 0,
      f5: 0,
      f6: BigInt(0),
      f7: '',
    })
  })
})
