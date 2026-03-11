import {should, assert} from 'chai'

import {assertRecursiveInclude} from './common/recursive-include'

should()

describe('assertRecursiveInclude', () => {
  it('Succeeds for empty objects', () => {
    assert.doesNotThrow(() => assertRecursiveInclude({}, {}))
  })
  it('Succeeds for objects with the same values', () => {
    assert.doesNotThrow(() => assertRecursiveInclude({example: true}, {example: true}))
  })
  it('Succeeds for nested objects with the same values', () => {
    assert.doesNotThrow(() => assertRecursiveInclude(
      {a: {b: {example: true}}},
      {a: {b: {example: true}}}
    ))
  })
  it('Succeeds when the superset has extra properties ', () => {
    assert.doesNotThrow(() => assertRecursiveInclude(
      {a: {b: {example: true, otherProperty: true}}},
      {a: {b: {example: true}}}
    ))
  })
  it('Succeeds when an array has matching contents', () => {
    assert.doesNotThrow(() => assertRecursiveInclude(
      {a: {b: [1, 2, 3]}},
      {a: {b: [1, 2, 3]}}
    ))
  })
  it('Succeeds when the an array has valid nested contents', () => {
    assert.doesNotThrow(() => assertRecursiveInclude(
      {a: {b: [{p1: 1}, {p2: 2}, {p3: 3, extra: true}]}},
      {a: {b: [{p1: 1}, {p2: 2}, {p3: 3}]}}
    ))
  })
  it('Fails for objects with different values', () => {
    assert.throws(() => assertRecursiveInclude({example: true}, {example: false}))
  })
  it('Fails when called with a non-object values', () => {
    assert.throws(() => assertRecursiveInclude(null, {example: false}))
    assert.throws(() => assertRecursiveInclude({example: false}, null))
    assert.throws(() => assertRecursiveInclude('string', 'string'))
    assert.throws(() => assertRecursiveInclude(123, 123))
    assert.throws(() => assertRecursiveInclude(true, true))
  })
  it('Fails when a nested object has a mismatch', () => {
    assert.throws(() => assertRecursiveInclude(
      {a: {b: {example: true}}},
      {a: {b: {example: false}}}
    ))
  })
  it('Fails when a nested object has a missing property', () => {
    assert.throws(() => assertRecursiveInclude(
      {a: {b: {}}},
      {a: {b: {example: true}}}
    ))
  })
  it('Fails when a nested object has a different type', () => {
    assert.throws(() => assertRecursiveInclude(
      {a: {b: {example: null}}},
      {a: {b: {example: ''}}}
    ))
  })
  it('Fails when an array has different contents', () => {
    assert.throws(() => assertRecursiveInclude(
      {a: {b: [3, 2, 1]}},
      {a: {b: [1, 2, 3]}}
    ))
  })
  it('Fails when the an array has missing properties nested contents', () => {
    assert.throws(() => assertRecursiveInclude(
      {a: {b: [{p1: 1}, {p2: 2}, {}]}},
      {a: {b: [{p1: 1}, {p2: 2}, {p3: 3}]}}
    ))
  })
})
