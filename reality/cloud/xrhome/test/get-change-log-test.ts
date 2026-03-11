import {describe, it} from 'mocha'
import {assert, expect} from 'chai'

import type {Expanse, GraphObject} from '@ecs/shared/scene-graph'

import {getChangeLog, getLeafPaths} from '../src/client/studio/get-change-log'

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

const empty: Expanse = {
  objects: {},
}

const testBox: Expanse = {
  objects: {
    'test': makeObject('test'),
  },
}

const testBoxWithComponent: Expanse = {
  objects: {
    'test': {
      ...makeObject('test'),
      components: {
        'id1': {
          id: 'id1',
          name: 'testing!',
          parameters: {},
        },
      },
    },
  },
}

const testBoxWithTwoComponents: Expanse = {
  objects: {
    'test': {
      ...makeObject('test'),
      components: {
        'id1': {
          id: 'id1',
          name: 'testing!',
          parameters: {},
        },
        'id2': {
          id: 'id2',
          name: 'testing!',
          parameters: {},
        },
      },
    },
  },
}

const testNonObject: Expanse = {
  objects: {
    'test': makeObject('test'),
  },
  activeCamera: 'camId',
}

const test2Box = {
  objects: {
    'test2': makeObject('test2'),
  },
}

const testBoxWithGeometryWide = {
  objects: {
    'test': {
      ...makeObject('test'),
      geometry: {
        type: 'box' as const,
        width: 10,
        height: 10,
        depth: 10,
      },
    },
  },
}

const testBoxWithGeometrySkinny = {
  objects: {
    'test': {
      ...makeObject('test'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 10,
        depth: 10,
      },
    },
  },
}

const multipleWideBoxes = {
  objects: {
    'test': {
      ...makeObject('test'),
      geometry: {
        type: 'box' as const,
        width: 10,
        height: 10,
        depth: 10,
      },
    },
    'test2': {
      ...makeObject('test2'),
      geometry: {
        type: 'box' as const,
        width: 10,
        height: 10,
        depth: 10,
      },
    },
    'test3': {
      ...makeObject('test3'),
      geometry: {
        type: 'box' as const,
        width: 10,
        height: 10,
        depth: 10,
      },
    },

  },
}

const multipleSkinnyBoxes = {
  objects: {
    'test': {
      ...makeObject('test'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 10,
        depth: 10,
      },
    },
    'test2': {
      ...makeObject('test2'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 1,
        depth: 10,
      },
    },
    'test3': {
      ...makeObject('test3'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 1,
        depth: 1,
      },
    },

  },
}

const multipleSkinnyBoxesShift = {
  objects: {
    'test2': {
      ...makeObject('test2'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 1,
        depth: 10,
      },
    },
    'test3': {
      ...makeObject('test3'),
      geometry: {
        type: 'box' as const,
        width: 1,
        height: 1,
        depth: 1,
      },
    },
    'test4': makeObject('test4'),
  },
}

const multipleSpaces1: Expanse = {
  objects: {
    'test': makeObject('test', {parentId: 'space1'}),
    'test2': makeObject('test2', {parentId: 'space1'}),
    'test3': makeObject('test3', {parentId: 'space2'}),
    'test4': makeObject('test4', {parentId: 'space2'}),
    'test5': makeObject('test5', {parentId: 'persistentSpaceWithObjectChange'}),
    'test6': makeObject('test6', {parentId: 'unchangedSpace'}),
  },
  spaces: {
    'space1': {
      name: 'Space 1',
      id: 'space1',
    },
    'space2': {
      name: 'Space 2',
      id: 'space2',
    },
    'persistentSpaceWithObjectChange': {
      name: 'Persistent Space',
      id: 'persistentSpaceWithObjectChange',
    },
    'unchangedSpace': {
      name: 'Unchanged Space',
      id: 'unchangedSpace',
    },
  },
}

const multipleSpaces2: Expanse = {
  objects: {
    'test': makeObject('test', {parentId: 'space3'}),
    'test2': makeObject('test2', {parentId: 'space3'}),
    'test3': makeObject('test3', {parentId: 'space4'}),
    'test4': makeObject('test4', {parentId: 'space4'}),
    'test5': makeObject('test5', {
      parentId: 'persistentSpaceWithObjectChange',
      position: [0, 0, 1],
    }),
    'test6': makeObject('test6', {parentId: 'unchangedSpace'}),
  },
  spaces: {
    'space3': {
      name: 'Space 3',
      id: 'space3',
    },
    'space4': {
      name: 'Space 4',
      id: 'space4',
    },
    'persistentSpaceWithObjectChange': {
      name: 'Persistent Space',
      id: 'persistentSpaceWithObjectChange',
    },
    'unchangedSpace': {
      name: 'Unchanged Space',
      id: 'unchangedSpace',
    },
  },
}

// eslint-disable-next-line arrow-parens
const expectInSomeOrder = <T>(a: T[], b: T[]) => {
  expect(a.length).to.equal(b.length)
  expect(a).to.have.deep.members(b)
}

describe('get-change-log', () => {
  describe('Get Leaf Paths', () => {
    it('Small JSON', () => {
      const obj = {
        x: 'y',
      }
      const expected = [
        ['x'],
      ]
      const result = getLeafPaths(obj)
      expectInSomeOrder(result, expected)
    })

    it('Deeper JSON', () => {
      const obj = {
        x: {
          y: 'z',
        },
      }
      const expected = [
        ['x', 'y'],
      ]
      const result = getLeafPaths(obj)
      expectInSomeOrder(result, expected)
    })

    it('Multiple Leaves', () => {
      const obj = {
        x: 'y',
        a: 'b',
      }
      const expected = [
        ['x'],
        ['a'],
      ]
      const result = getLeafPaths(obj)
      expectInSomeOrder(result, expected)
    })

    it('Nested Larger Objects', () => {
      const obj = {
        x: {
          x1: 'test',
          x2: 'test',
        },
        y: {
          y1: 'test',
          y2: 'test',
        },
      }
      const expected = [
        ['x', 'x1'],
        ['x', 'x2'],
        ['y', 'y1'],
        ['y', 'y2'],
      ]
      const result = getLeafPaths(obj)
      expectInSomeOrder(result, expected)
    })

    it('Multiple values', () => {
      const obj = {
        x: {
          x1: 'test',
          x2: 'test',
        },
        y: {
          y1: 'test',
          y2: 'test',
        },
        z: {
          z1: 1,
          z2: {},
          z3: [],
          z4: null,
          // this is actually set as a key, so it should be a leaf path
          z5: undefined,
        },
      }
      const expected = [
        ['x', 'x1'],
        ['x', 'x2'],
        ['y', 'y1'],
        ['y', 'y2'],
        ['z', 'z1'],
        ['z', 'z2'],
        ['z', 'z3'],
        ['z', 'z4'],
        ['z', 'z5'],
      ]
      const result = getLeafPaths(obj)
      expectInSomeOrder(result, expected)
    })
  })

  describe('Basic', () => {
    it('Same scene has no changes', () => {
      const changes = getChangeLog(testBox, testBox)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.updatedPaths, [])
      expectInSomeOrder(changes.addedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Adding box only includes added box', () => {
      const changes = getChangeLog(empty, testBox)
      assert.deepEqual(changes.objectAdditions, {'test': makeObject('test')})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.addedPaths, [['objects', 'test']])
      expectInSomeOrder(changes.updatedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Deleting box only includes deleted box', () => {
      const changes = getChangeLog(testBox, empty)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {'test': makeObject('test')})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.deletedPaths, [['objects', 'test']])
      expectInSomeOrder(changes.addedPaths, [])
      expectInSomeOrder(changes.updatedPaths, [['objects']])
    })

    it('Deleting a box and adding another', () => {
      const changes = getChangeLog(testBox, test2Box)
      assert.deepEqual(changes.objectAdditions, {'test2': makeObject('test2')})
      assert.deepEqual(changes.objectDeletions, {'test': makeObject('test')})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.addedPaths, [['objects', 'test2']])
      expectInSomeOrder(changes.deletedPaths, [['objects', 'test']])
      expectInSomeOrder(changes.updatedPaths, [])
    })

    it('Deleting a box and adding another inverted', () => {
      const changes = getChangeLog(test2Box, testBox)
      assert.deepEqual(changes.objectAdditions, {'test': makeObject('test')})
      assert.deepEqual(changes.objectDeletions, {'test2': makeObject('test2')})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.addedPaths, [['objects', 'test']])
      expectInSomeOrder(changes.deletedPaths, [['objects', 'test2']])
      expectInSomeOrder(changes.updatedPaths, [])
    })

    it('Updating box geometry', () => {
      const changes = getChangeLog(testBoxWithGeometrySkinny, testBoxWithGeometryWide)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {'test': [['geometry', 'width']]})
      expectInSomeOrder(changes.updatedPaths, [['objects', 'test', 'geometry', 'width']])
      expectInSomeOrder(changes.addedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Updating multiple box geometries', () => {
      const changes = getChangeLog(multipleSkinnyBoxes, multipleWideBoxes)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      expectInSomeOrder(changes.objectUpdates.test,
        [['geometry', 'width']])
      expectInSomeOrder(changes.objectUpdates.test2,
        [['geometry', 'width'],
          ['geometry', 'height']])
      expectInSomeOrder(changes.objectUpdates.test3,
        [['geometry', 'width'],
          ['geometry', 'height'],
          ['geometry', 'depth']])
      expectInSomeOrder(changes.updatedPaths,
        [['objects', 'test', 'geometry', 'width'],
          ['objects', 'test2', 'geometry', 'width'],
          ['objects', 'test2', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'width'],
          ['objects', 'test3', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'depth']])
      expectInSomeOrder(changes.addedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Updating multiple box geometries is same inverted', () => {
      const changes = getChangeLog(multipleWideBoxes, multipleSkinnyBoxes)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      expectInSomeOrder(changes.objectUpdates.test,
        [['geometry', 'width']])
      expectInSomeOrder(changes.objectUpdates.test2,
        [['geometry', 'width'],
          ['geometry', 'height']])
      expectInSomeOrder(changes.objectUpdates.test3,
        [['geometry', 'width'],
          ['geometry', 'height'],
          ['geometry', 'depth']])
      expectInSomeOrder(changes.updatedPaths,
        [['objects', 'test', 'geometry', 'width'],
          ['objects', 'test2', 'geometry', 'width'],
          ['objects', 'test2', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'width'],
          ['objects', 'test3', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'depth']])
      expectInSomeOrder(changes.addedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Updates, objectadditions, and objectdeletions', () => {
      const changes = getChangeLog(multipleWideBoxes, multipleSkinnyBoxesShift)
      assert.deepEqual(changes.objectAdditions, {'test4': makeObject('test4')})
      assert.deepEqual(changes.objectDeletions, {'test': multipleWideBoxes.objects.test})
      expectInSomeOrder(changes.objectUpdates.test2,
        [['geometry', 'width'],
          ['geometry', 'height']])
      expectInSomeOrder(changes.objectUpdates.test3,
        [['geometry', 'width'],
          ['geometry', 'height'],
          ['geometry', 'depth']])
      expectInSomeOrder(changes.updatedPaths,
        [['objects', 'test2', 'geometry', 'width'],
          ['objects', 'test2', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'width'],
          ['objects', 'test3', 'geometry', 'height'],
          ['objects', 'test3', 'geometry', 'depth']])
      expectInSomeOrder(changes.addedPaths, [['objects', 'test4']])
      expectInSomeOrder(changes.deletedPaths, [['objects', 'test']])
    })

    it('Adding a component', () => {
      const changes = getChangeLog(testBox, testBoxWithComponent)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      expectInSomeOrder(changes.objectUpdates.test,
        [['components', 'id1', 'id'],
          ['components', 'id1', 'name'],
          ['components', 'id1', 'parameters']])
      expectInSomeOrder(changes.addedPaths,
        [['objects', 'test', 'components', 'id1']])
      expectInSomeOrder(changes.updatedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Adding two components', () => {
      const changes = getChangeLog(testBox, testBoxWithTwoComponents)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      expectInSomeOrder(changes.objectUpdates.test,
        [['components', 'id1', 'id'],
          ['components', 'id1', 'name'],
          ['components', 'id1', 'parameters'],
          ['components', 'id2', 'id'],
          ['components', 'id2', 'name'],
          ['components', 'id2', 'parameters']])
      expectInSomeOrder(changes.addedPaths,
        [['objects', 'test', 'components', 'id1'],
          ['objects', 'test', 'components', 'id2']])
      expectInSomeOrder(changes.updatedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Non object changes', () => {
      const changes = getChangeLog(testBox, testNonObject)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {})
      expectInSomeOrder(changes.addedPaths, [['activeCamera']])
      expectInSomeOrder(changes.updatedPaths, [])
      expectInSomeOrder(changes.deletedPaths, [])
    })

    it('Space Changes Test Forward', () => {
      const changes = getChangeLog(multipleSpaces1, multipleSpaces2)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {
        'test': [['parentId']],
        'test2': [['parentId']],
        'test3': [['parentId']],
        'test4': [['parentId']],
        'test5': [['position', '2']],
      })
      assert.deepEqual(changes.spaceChanges, {
        'space1': {
          type: 'removed',
          name: 'Space 1',
          changedObjectCount: 0,
        },
        'space2': {
          type: 'removed',
          name: 'Space 2',
          changedObjectCount: 0,
        },
        'space3': {
          type: 'added',
          name: 'Space 3',
          changedObjectCount: 0,
        },
        'space4': {
          type: 'added',
          name: 'Space 4',
          changedObjectCount: 0,
        },
        'persistentSpaceWithObjectChange': {
          type: 'changed',
          name: 'Persistent Space',
          changedObjectCount: 1,
        },
      })
    })

    it('Space Changes Test Backward', () => {
      const changes = getChangeLog(multipleSpaces2, multipleSpaces1)
      assert.deepEqual(changes.objectAdditions, {})
      assert.deepEqual(changes.objectDeletions, {})
      assert.deepEqual(changes.objectUpdates, {
        'test': [['parentId']],
        'test2': [['parentId']],
        'test3': [['parentId']],
        'test4': [['parentId']],
        'test5': [['position', '2']],
      })
      assert.deepEqual(changes.spaceChanges, {
        'space1': {
          type: 'added',
          name: 'Space 1',
          changedObjectCount: 0,
        },
        'space2': {
          type: 'added',
          name: 'Space 2',
          changedObjectCount: 0,
        },
        'space3': {
          type: 'removed',
          name: 'Space 3',
          changedObjectCount: 0,
        },
        'space4': {
          type: 'removed',
          name: 'Space 4',
          changedObjectCount: 0,
        },
        'persistentSpaceWithObjectChange': {
          type: 'changed',
          name: 'Persistent Space',
          changedObjectCount: 1,
        },
      })
    })
  })
})
