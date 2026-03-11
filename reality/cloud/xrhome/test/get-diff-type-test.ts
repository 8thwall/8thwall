import {describe, it} from 'mocha'
import {assert} from 'chai'

import type {Expanse, GraphObject} from '@ecs/shared/scene-graph'

import {getChangeLog} from '../src/client/studio/get-change-log'
import {
  getDiffType,
  getFromPath,
  isPrefixOf,
  SubfieldsChanged,
} from '../src/client/studio/get-diff-type'

const makeObject = (id: string, extra?: Partial<GraphObject>): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  geometry: null,
  material: null,
  components: {},
  ...extra,
})

const beforeOrigin: Expanse = {
  objects: {
    a: makeObject('a', {name: 'old'}),
  },
}

const afterMove: Expanse = {
  ...beforeOrigin,
  objects: {
    ...beforeOrigin.objects,
    a: {
      ...beforeOrigin.objects.a,
      position: [1, 1, 1],
    },
  },
}

describe('getDiffType', () => {
  describe('isSubpath', () => {
    it('Double empty', () => {
      assert.isTrue(isPrefixOf([], []))
    })
    it('Basic subpath', () => {
      assert.isTrue(isPrefixOf(['a'], ['a', 'b']))
    })
    it('Basic non-subpath', () => {
      assert.isFalse(isPrefixOf(['a', 'b'], ['a']))
    })
    it('Subpath with empty', () => {
      assert.isTrue(isPrefixOf([], ['a', 'b']))
    })
    it('Big subpath', () => {
      assert.isTrue(isPrefixOf(['a'], ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']))
    })
    it('Big non-subpath', () => {
      assert.isFalse(isPrefixOf(['s', 'b', 'c'], ['a', 'b', 'c', 'd', 'e']))
    })
    it('Empty not subpath', () => {
      assert.isFalse(isPrefixOf(['b'], []))
    })
    it('Empty strings', () => {
      assert.isTrue(isPrefixOf([''], ['']))
    })
    it('Empty string not subpath', () => {
      assert.isFalse(isPrefixOf(['a'], ['']))
    })
    it('Empty string not subpath two', () => {
      assert.isFalse(isPrefixOf([''], ['a', 'b', 'c']))
    })
    it('Many empty strings', () => {
      assert.isFalse(isPrefixOf(['', '', '', ''], ['']))
    })
    it('Repeated names', () => {
      assert.isTrue(isPrefixOf(['a', 'a', 'a'], ['a', 'a', 'a']))
    })
    it('Repeated names non-subpath', () => {
      assert.isFalse(isPrefixOf(['a', 'a', 'a', 'a'], ['a', 'a', 'a']))
    })
  })

  describe('getFromPath', () => {
    it('Empty path', () => {
      const obj = {a: 1, b: 2}
      assert.deepStrictEqual(getFromPath([], obj), obj)
    })
    it('Single path', () => {
      const obj = {a: 1, b: 2}
      assert.deepStrictEqual(getFromPath(['a'], obj), 1)
    })
    it('Nested path', () => {
      const obj = {a: {b: {c: 3}}}
      assert.deepStrictEqual(getFromPath(['a', 'b', 'c'], obj), 3)
    })
    it('Non-existent path', () => {
      const obj = {a: 1, b: 2}
      assert.deepStrictEqual(getFromPath(['c'], obj), undefined)
    })
    it('Deeply nested path', () => {
      const obj = {a: {b: {c: {d: {e: 5}}}}}
      assert.deepStrictEqual(getFromPath(['a', 'b', 'c', 'd', 'e'], obj), 5)
    })
    it('Deep non-existent path', () => {
      const obj = {a: {b: {c: 3}}}
      assert.deepStrictEqual(getFromPath(['a', 'b', 'x'], obj), undefined)
    })
    it('Deep completely non-existent path', () => {
      const obj = {a: {b: {c: 3}}}
      assert.deepStrictEqual(getFromPath(['x', 'y', 'z'], obj), undefined)
    })
  })

  describe('getDiffType', () => {
    it('Added object', () => {
      const before: Expanse = {objects: {}}
      const after: Expanse = {objects: {a: makeObject('a')}}
      const changeLog = getChangeLog(before, after)
      const diffType = getDiffType(changeLog, before, after, ['objects', 'a'])
      assert.deepStrictEqual(diffType, {type: 'added', before: undefined, after: makeObject('a')})
    })

    it('Removed object', () => {
      const before: Expanse = {objects: {a: makeObject('a')}}
      const after: Expanse = {objects: {}}
      const changeLog = getChangeLog(before, after)
      const diffType = getDiffType(changeLog, before, after, ['objects', 'a'])
      assert.deepStrictEqual(diffType, {type: 'removed', before: makeObject('a'), after: undefined})
    })

    it('Changed object', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: 'old'})}}
      const after: Expanse = {objects: {a: makeObject('a', {name: 'new'})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(changeLog, before, after, ['objects', 'a', 'name'])
      const aDiff = getDiffType(changeLog, before, after, ['objects', 'a'])
      assert.deepStrictEqual(nameDiff, {type: 'changedDirectly', before: 'old', after: 'new'})
      assert.deepStrictEqual(aDiff.type, 'subfieldsChanged')
      assert.deepStrictEqual(
        (aDiff as SubfieldsChanged<GraphObject>).before,
        makeObject('a', {name: 'old'})
      )
      assert.deepStrictEqual(
        (aDiff as SubfieldsChanged<GraphObject>).after,
        makeObject('a', {name: 'new'})
      )
    })

    it('Unchanged object', () => {
      const before: Expanse = {objects: {a: makeObject('a')}}
      const after: Expanse = {objects: {a: makeObject('a')}}
      const changeLog = getChangeLog(before, after)
      const aDiff = getDiffType(changeLog, before, after, ['objects', 'a'])
      assert.deepStrictEqual(aDiff, {
        type: 'unchanged',
        before: makeObject('a'),
        after: makeObject('a'),
      })
    })

    it('Non-existent path unchanged', () => {
      const before: Expanse = {objects: {a: makeObject('a')}}
      const after: Expanse = {objects: {a: makeObject('a')}}
      const changeLog = getChangeLog(before, after)
      const bDiff = getDiffType(changeLog, before, after, ['objects', 'b'])
      assert.deepStrictEqual(bDiff, {type: 'unchanged', before: undefined, after: undefined})
    })

    it('Updating position (arrays)', () => {
      const changeLog = getChangeLog(beforeOrigin, afterMove)
      const diffType = getDiffType(changeLog, beforeOrigin, afterMove, ['objects', 'a', 'position'])
      assert.deepStrictEqual(
        diffType,
        {
          type: 'subfieldsChanged',
          before: [0, 0, 0],
          after: [1, 1, 1],
        }
      )
    })

    it('Uses default for before value', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const after: Expanse = {objects: {a: makeObject('a', {name: 'new'})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'changedDirectly',
        before: 'defaultName',
        after: 'new',
      })
    })

    it('Uses default for after value', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: 'old'})}}
      const after: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'changedDirectly',
        before: 'old',
        after: 'defaultName',
      })
    })

    it('Undefined to Default is Unchanged', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: 'defaultName'})}}
      const after: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'unchanged',
        before: 'defaultName',
        after: 'defaultName',
      })
    })

    it('Undefined to Undefined is Unchanged', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const after: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'unchanged',
        before: 'defaultName',
        after: 'defaultName',
      })
    })

    it('Added continuation doesn\'t register', () => {
      const before: Expanse = {objects: {}}
      const after: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'unchanged',
        before: 'defaultName',
        after: 'defaultName',
      })
    })

    it('Deleted continuation doesn\'t register', () => {
      const before: Expanse = {objects: {a: makeObject('a', {name: undefined})}}
      const after: Expanse = {objects: {}}
      const changeLog = getChangeLog(before, after)
      const nameDiff = getDiffType(
        changeLog,
        before,
        after,
        ['objects', 'a', 'name'],
        'defaultName'
      )
      assert.deepStrictEqual(nameDiff, {
        type: 'unchanged',
        before: 'defaultName',
        after: 'defaultName',
      })
    })
  })
})
