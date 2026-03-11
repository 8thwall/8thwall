import {describe, it, assert} from '@nia/bzl/js/chai-js'

import {fromAttribute, fromAttributes, toAttribute, toAttributes}
  from './typed-attributes'

describe('toAttribute', () => {
  it('Handles booleans', () => {
    assert.deepEqual(toAttribute(true), {BOOL: true})
    assert.deepEqual(toAttribute(false), {BOOL: false})
  })
  it('Handles strings', () => {
    assert.deepEqual(toAttribute('my-string'), {S: 'my-string'})
  })
  it('Handles numbers', () => {
    assert.deepEqual(toAttribute(0), {N: '0'})
    assert.deepEqual(toAttribute(11235), {N: '11235'})
    assert.deepEqual(toAttribute(11.25), {N: '11.25'})
  })
  it('Handles maps', () => {
    assert.deepEqual(toAttribute({hello: 'world'}), {M: {hello: {S: 'world'}}})
  })
  it('Handles string sets', () => {
    assert.deepEqual(
      toAttribute(['hello', 'world']),
      {SS: ['hello', 'world']}
    )
  })
  it('Handles number sets', () => {
    assert.deepEqual(
      toAttribute([0, 1, 2.3]),
      {NS: ['0', '1', '2.3']}
    )
  })
  it('Handles nested maps', () => {
    assert.deepEqual(
      toAttribute({this: {is: {a: {nested: 'map'}}}}),
      {M: {this: {M: {is: {M: {a: {M: {nested: {S: 'map'}}}}}}}}}
    )
  })
  it('Handles string set nested in map', () => {
    assert.deepEqual(
      toAttribute({nested: ['string', 'set']}),
      {M: {nested: {SS: ['string', 'set']}}}
    )
  })
  it('Handles number set nested in map', () => {
    assert.deepEqual(
      toAttribute({nested: [0, 1, 2.3]}),
      {M: {nested: {NS: ['0', '1', '2.3']}}}
    )
  })
  it('Handles list of maps', () => {
    assert.deepEqual(
      toAttribute([{hello: 'world'}, {foo: 'bar'}]),
      {
        L: [
          {M: {hello: {S: 'world'}}},
          {M: {foo: {S: 'bar'}}},
        ],
      }
    )
  })
  it('Handles list of anything', () => {
    assert.deepEqual(
      toAttribute([{hello: 'world'}, ['foo', 'bar'], [{map: {inside: {list: 'of-lists'}}}]]),
      {
        L: [
          {M: {hello: {S: 'world'}}},
          {SS: ['foo', 'bar']},
          {L: [{M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}}]},
        ],
      }
    )
  })
})

describe('fromAttribute', () => {
  it('Handles booleans', () => {
    assert.strictEqual(fromAttribute({BOOL: true}), true)
    assert.strictEqual(fromAttribute({BOOL: false}), false)
  })
  it('Handles strings', () => {
    assert.strictEqual(fromAttribute({S: 'my-string'}), 'my-string')
  })
  it('Handles numbers', () => {
    assert.strictEqual(fromAttribute({N: '0'}), 0)
    assert.strictEqual(fromAttribute({N: '11235'}), 11235)
    assert.strictEqual(fromAttribute({N: '11.25'}), 11.25)
  })
  it('Handles maps', () => {
    assert.deepEqual(fromAttribute({M: {hello: {S: 'world'}}}), {hello: 'world'})
  })
  it('Handles string sets', () => {
    assert.deepEqual(
      fromAttribute({SS: ['hello', 'world']}),
      ['hello', 'world']
    )
  })
  it('Handles number sets', () => {
    assert.deepEqual(
      fromAttribute({NS: ['0', '1', '2.3']}),
      [0, 1, 2.3]
    )
  })
  it('Handles nested maps', () => {
    assert.deepEqual(
      fromAttribute({M: {this: {M: {is: {M: {a: {M: {nested: {S: 'map'}}}}}}}}}),
      {this: {is: {a: {nested: 'map'}}}}
    )
  })
  it('Handles string set nested in map', () => {
    assert.deepEqual(
      fromAttribute({M: {nested: {SS: ['string', 'set']}}}),
      {nested: ['string', 'set']}
    )
  })
  it('Handles number set nested in map', () => {
    assert.deepEqual(
      fromAttribute({M: {nested: {NS: ['0', '1', '2.3']}}}),
      {nested: [0, 1, 2.3]}
    )
  })
  it('Handles list of maps', () => {
    assert.deepEqual(
      fromAttribute({
        L: [
          {M: {hello: {S: 'world'}}},
          {M: {foo: {S: 'bar'}}},
        ],
      }),
      [{hello: 'world'}, {foo: 'bar'}]
    )
  })
  it('Handles list of anything', () => {
    assert.deepEqual(
      fromAttribute({
        L: [
          {M: {hello: {S: 'world'}}},
          {SS: ['foo', 'bar']},
          {L: [{M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}}]},
        ],
      }),
      [{hello: 'world'}, ['foo', 'bar'], [{map: {inside: {list: 'of-lists'}}}]]
    )
  })
})

describe('toAttributes', () => {
  it('Transforms the values of an object', () => {
    assert.deepEqual(
      toAttributes({
        string: 'my-string',
        number: 777,
        boolean: true,
        map: {hello: 'world'},
        stringSet: ['hello', 'world'],
        numberSet: [0, 1, 2.3],
        nested: {map: {inside: {list: 'of-lists'}}},
        listOfMaps: [{hello: 'world'}, {foo: 'bar'}],
        listOfAnything: [{hello: 'world'}, ['foo', 'bar'], [{map: {inside: {list: 'of-lists'}}}]],
      }),
      {
        string: {S: 'my-string'},
        number: {N: '777'},
        boolean: {BOOL: true},
        map: {M: {hello: {S: 'world'}}},
        stringSet: {SS: ['hello', 'world']},
        numberSet: {NS: ['0', '1', '2.3']},
        nested: {M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}},
        listOfMaps: {
          L: [
            {M: {hello: {S: 'world'}}},
            {M: {foo: {S: 'bar'}}},
          ],
        },
        listOfAnything: {
          L: [
            {M: {hello: {S: 'world'}}},
            {SS: ['foo', 'bar']},
            {L: [{M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}}]},
          ],
        },
      }
    )
  })
})

describe('fromAttributes', () => {
  it('Transforms the values of an object', () => {
    assert.deepEqual(
      fromAttributes({
        string: {S: 'my-string'},
        number: {N: '777'},
        boolean: {BOOL: true},
        map: {M: {hello: {S: 'world'}}},
        stringSet: {SS: ['hello', 'world']},
        numberSet: {NS: ['0', '1', '2.3']},
        nested: {M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}},
        listOfMaps: {
          L: [
            {M: {hello: {S: 'world'}}},
            {M: {foo: {S: 'bar'}}},
          ],
        },
        listOfAnything: {
          L: [
            {M: {hello: {S: 'world'}}},
            {SS: ['foo', 'bar']},
            {L: [{M: {map: {M: {inside: {M: {list: {S: 'of-lists'}}}}}}}]},
          ],
        },
      }),
      {
        string: 'my-string',
        number: 777,
        boolean: true,
        map: {hello: 'world'},
        stringSet: ['hello', 'world'],
        numberSet: [0, 1, 2.3],
        nested: {map: {inside: {list: 'of-lists'}}},
        listOfMaps: [{hello: 'world'}, {foo: 'bar'}],
        listOfAnything: [{hello: 'world'}, ['foo', 'bar'], [{map: {inside: {list: 'of-lists'}}}]],
      }
    )
  })
})
