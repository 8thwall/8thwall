import type {
  BaseGraphObject,
  GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {getObjectParentId, hasParent} from '@ecs/shared/object-hierarchy'

import {
  findPrefabDependencyCycle, makePrefabAndReplaceWithInstance,
} from '../src/client/studio/configuration/prefab'
import type {IdMapper} from '../src/client/studio/id-generation'
import {deriveScene} from '../src/client/studio/derive-scene'
import {
  resolveObjectReference, getObjectOrMergedInstance,
} from '../src/client/studio/derive-scene-operations'

/* eslint quote-props: ["error", "as-needed"] */
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

const makePrefab = (id: string, extra?: Partial<GraphObject>): BaseGraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  geometry: null,
  material: null,
  prefab: true,
  components: {},
  ...extra,
})

const makeInstance = (
  id: string, instanceOf: string, deletions?: {}, overrides?: Partial<GraphObject>,
  children?: PrefabInstanceChildren
): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  components: {},
  instanceData: {
    instanceOf,
    deletions:
     deletions || {},
    ...(children ? {children} : {}),
  },
  ...overrides,
})

const SOURCE_PREFIX_SEED: IdMapper = {
  fromId: (id: string) => `source${id}`,
}

describe('makePrefabAndReplaceWithInstance', () => {
  it('should replace a prefab with an instance', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {position: [1, 2, 3]}),
      },
    }

    const newScene = makePrefabAndReplaceWithInstance(initialScene, 'A', SOURCE_PREFIX_SEED)
    assert.deepEqual(newScene, {
      objects: {
        sourceA: makeObject('sourceA', {
          name: '<unset>',
          position: [1, 2, 3],
          prefab: true,
        }),
        A: {
          id: 'A',
          name: '<unset>',
          position: [1, 2, 3],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          components: {},
          instanceData: {
            instanceOf: 'sourceA',
            deletions: {},
            children: {},
          },
        },
      },
    })
  })

  it('retains parentId of the original object', () => {
    const scene: SceneGraph = {
      objects: {
        A: makeObject('A', {parentId: 'space1', position: [1, 2, 3]}),
      },
      spaces: {
        space1: {
          id: 'space1',
          name: 'space1',
        },
      },
    }

    const newScene = makePrefabAndReplaceWithInstance(scene, 'A', SOURCE_PREFIX_SEED)
    assert.deepEqual(newScene, {
      objects: {
        sourceA: makeObject('sourceA', {
          position: [1, 2, 3],
          prefab: true,
        }),
        A: makeInstance('A', 'sourceA', {}, {
          parentId: 'space1',
          position: [1, 2, 3],
        }, {}),
      },
      spaces: scene.spaces,
    })
  })

  it('moves all children of the original to new IDs', () => {
    const scene: SceneGraph = {
      objects: {
        A: makeObject('A', {parentId: 'space1', position: [1, 2, 3]}),
        B: makeObject('B', {
          parentId: 'A',
          position: [4, 4, 4],
        }),
        C: makeObject('C', {
          parentId: 'B',
          position: [5, 5, 5],
        }),
        D: makeObject('D', {parentId: 'space1'}),
      },
      spaces: {
        space1: {
          id: 'space1',
          name: 'space1',
        },
      },
    }

    const newScene = makePrefabAndReplaceWithInstance(scene, 'A', SOURCE_PREFIX_SEED)
    assert.deepEqual(newScene, {
      objects: {
        D: makeObject('D', {parentId: 'space1'}),
        sourceA: makeObject('sourceA', {
          position: [1, 2, 3],
          prefab: true,
        }),
        sourceB: makeObject('sourceB', {
          parentId: 'sourceA',
          position: [4, 4, 4],
        }),
        sourceC: makeObject('sourceC', {
          parentId: 'sourceB',
          position: [5, 5, 5],
        }),
        A: makeInstance('A', 'sourceA', {}, {
          position: [1, 2, 3],
          parentId: 'space1',
        }, {}),
      },
      spaces: scene.spaces,
    })
  })

  const setReference = (scene: SceneGraph, from: string, to: string) => {
    scene.objects[from].components.ref = {
      id: 'ref',
      name: 'ref',
      parameters: {target: {type: 'entity', id: to}},
    }
  }

  it('should update object references properly', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A'),
        B: makeObject('B', {
          parentId: 'A',
          position: [4, 4, 4],
        }),
        C: makeObject('C', {
          parentId: 'B',
          position: [5, 5, 5],
        }),
        D: makeObject('D'),
        E: makeObject('E', {parentId: 'D'}),
        F: makeObject('F', {parentId: 'E'}),
      },
    }

    const expectedScene: SceneGraph = {
      objects: {
        sourceA: makeObject('sourceA', {
          name: '<unset>',
          position: [0, 0, 0],
          prefab: true,
        }),
        sourceB: makeObject('sourceB', {
          parentId: 'sourceA',
          position: [4, 4, 4],
        }),
        sourceC: makeObject('sourceC', {
          parentId: 'sourceB',
          position: [5, 5, 5],
        }),
        A: makeInstance('A', 'sourceA', {}, {}, {}),
        D: makeObject('D'),
        E: makeObject('E', {parentId: 'D'}),
        F: makeObject('F', {parentId: 'E'}),
      },
    }

    setReference(initialScene, 'B', 'C')  // inside -> inside
    setReference(initialScene, 'D', 'C')  // outside -> inside
    setReference(initialScene, 'E', 'D')  // outside -> outside
    setReference(initialScene, 'C', 'D')  // inside -> outside

    setReference(expectedScene, 'sourceB', 'sourceC')  // inside -> inside
    setReference(expectedScene, 'D', 'A/sourceC')  // outside -> inside
    setReference(expectedScene, 'E', 'D')  // outside -> outside

    // NOTE(christoph): We are intentionally not clearing this reference even though it references
    // a space's object from inside a prefab. This preserves the original behavior of the object
    // even though it's technically more correct to move the reference to instanceData.
    setReference(expectedScene, 'sourceC', 'D')  // inside -> outside

    const newScene = makePrefabAndReplaceWithInstance(initialScene, 'A', SOURCE_PREFIX_SEED)

    assert.deepEqual(newScene, expectedScene)
  })
})

describe('prefab', () => {
  it('Can merge an empty prefab and instance', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab')
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, instance.id)
    assert.deepEqual(mergedInstance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: null,
      material: null,
      components: {},
    })
  })

  it('Can merge a prefab with a geometry and an instance with a material', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {
      material: {type: 'basic', color: '#ffffff'},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, instance.id)
    assert.deepEqual(mergedInstance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      material: {type: 'basic', color: '#ffffff'},
      components: {},
    })
  })

  it('Instance values override prefab values', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {
      geometry: {type: 'sphere', radius: 1},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, instance.id)
    assert.deepEqual(mergedInstance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: {type: 'sphere', radius: 1},
      material: null,
      components: {},
    })
  })

  it('Can merge a child of an instance and the source child', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const child = makeObject('child', {
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      'instance/child': {
        id: 'instance/child',
        deletions: {},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, 'instance/child')
    assert.deepEqual(mergedInstance, {
      id: 'instance/child',
      parentId: 'instance',
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: {type: 'sphere', radius: 1},
      material: null,
      components: {},
    })
  })

  it('Can merge a child of an instance with override values and the source child', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const child = makeObject('child', {
      name: 'child',
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child: {
        id: 'child',
        deletions: {},
        geometry: {type: 'cylinder', radius: 1, height: 2},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, 'instance/child')
    assert.deepEqual(mergedInstance, {
      id: 'instance/child',
      parentId: 'instance',
      name: 'child',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: {type: 'cylinder', radius: 1, height: 2},
      material: null,
      components: {},
    })
  })

  it('Handles parentId override on an instance child', () => {
    const prefab = makePrefab('prefab')
    const child = makeObject('child', {parentId: 'prefab'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child: {
        id: 'child',
        parentId: 'newParent',
        deletions: {},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const mergedInstance = getObjectOrMergedInstance(graph, 'instance/child')
    const objectParent = getObjectParentId(graph, 'instance/child')
    assert.deepEqual(objectParent, 'newParent')
    assert.deepEqual(mergedInstance, {
      id: 'instance/child',
      parentId: 'newParent',
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: null,
      material: null,
      components: {},
    })
  })

  it('Returns correct child IDs for a prefab instance', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'child1'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child1: {
        id: 'child1',
        deletions: {},
      },
      child2: {
        id: 'child2',
        deletions: {},
      },
    })
    const normalObject = makeObject('normalObject', {parentId: 'instance'})
    const normalObject2 = makeObject('normalObject2', {parentId: 'instance/child1'})
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        instance,
        normalObject,
        normalObject2,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const childIds = derivedScene.getChildren('instance')
    assert.deepEqual(childIds, ['normalObject', 'instance/child1'])
    const nestedChildIds = derivedScene.getChildren('instance/child1')
    assert.deepEqual(nestedChildIds, ['normalObject2', 'instance/child2'])
  })

  it('Returns correct child IDs for a prefab instance with deeply nested children', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'child1'})
    const child3 = makeObject('child3', {parentId: 'child2'})
    const child4 = makeObject('child4', {parentId: 'child3'})
    const child5 = makeObject('child5', {parentId: 'prefab'})
    const child6 = makeObject('child6', {parentId: 'child2'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child1: {
        id: 'child1',
        deletions: {},
      },
      child2: {
        id: 'child2',
        deletions: {},
      },
      child3: {
        id: 'child3',
        deletions: {},
      },
      child4: {
        id: 'child4',
        deletions: {},
      },
      child5: {
        id: 'child5',
        deletions: {},
      },
      child6: {
        id: 'child6',
        deletions: {},
      },
    })
    const normalObject = makeObject('normalObject', {parentId: 'instance'})
    const normalObject2 = makeObject('normalObject2', {parentId: 'instance/child1'})
    const normalObject3 = makeObject('normalObject3', {parentId: 'instance/child2'})
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        child3,
        child4,
        child5,
        child6,
        instance,
        normalObject,
        normalObject2,
        normalObject3,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const childIds = derivedScene.getChildren('instance')
    assert.deepEqual(childIds, ['normalObject', 'instance/child1', 'instance/child5'])
    const nestedChildIds = derivedScene.getChildren('instance/child1')
    assert.deepEqual(nestedChildIds, ['normalObject2', 'instance/child2'])
    const nestedChildIds2 = derivedScene.getChildren('instance/child2')
    assert.deepEqual(nestedChildIds2, ['normalObject3', 'instance/child3', 'instance/child6'])
  })

  it('Excludes deleted children from a prefab instance', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'prefab'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child1: {
        id: 'child1',
        deletions: {},
      },
      child2: {
        deleted: true,
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        instance,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const childIds = derivedScene.getChildren('instance')
    assert.deepEqual(childIds, ['instance/child1'])
  })

  it('Excludes nested children from a deleted child of a prefab instance', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'child1'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child1: {
        id: 'child1',
        deletions: {},
        deleted: true,
      },
      child2: {
        id: 'child2',
        deletions: {},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        instance,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const childIds = derivedScene.getChildren('instance')
    assert.deepEqual(childIds, [])
  })

  it('hasParent Returns false if the instance in a scoped parent ID does not exist',
    () => {
      const graph: SceneGraph = {
        objects: {
          child: {
            id: 'child',
            parentId: 'nonexistentInstance/source',
            position: [0, 0, 0],
            rotation: [0, 0, 0, 1],
            scale: [1, 1, 1],
            geometry: null,
            material: null,
            components: {},
          },
        },
        spaces: {},
      }
      assert.isFalse(hasParent(graph, 'child'))
    })

  it('Returns correct prefab ID for a scoped ID whose instance is childed to a prefab', () => {
    const prefab = makePrefab('prefab')
    const child = makeObject('child', {parentId: 'prefab'})
    const parentPrefab = makePrefab('parentPrefab')
    const instance = makeInstance('instance', 'prefab', {},
      {
        parentId: 'parentPrefab',
      },
      {
        child: {
          id: 'child',
          deletions: {},
        },
      })
    const graph: SceneGraph = {
      objects: {
        parentPrefab,
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const parentPrefabId = derivedScene.getParentPrefabId('instance/child')
    assert.strictEqual(parentPrefabId, 'parentPrefab')
  })

  it('should return undefined when id is not a valid object', () => {
    const invalidId = 'nonexistent'
    const graph: SceneGraph = {
      objects: {},
      spaces: {},
    }
    const derivedScene = deriveScene(graph)
    const parentPrefabId = derivedScene.getParentPrefabId(invalidId)
    assert.isUndefined(parentPrefabId)
  })
})

describe('Prefab Dependency Cycle DerivedScene', () => {
  it('should find a cycle within prefabs', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
        3: makeInstance('3', '1', {}, {parentId: '2'}),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, '2', '1'))
  })

  it('should not find a cycle within unrelated prefabs', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '1', '2'))
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '2', '1'))
  })

  it('should find a cycle within itself', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, '1', '1'))
  })

  it('should find a cycle when dropping prefab A into one of its children', () => {
    const scene = {
      objects: {
        A: makePrefab('A'),
        box: makeObject('box', {parentId: 'A'}),
        box2: makeObject('box2', {parentId: 'box'}),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'A', 'box'))
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'A', 'box2'))
  })

  it('should find a cycle when A is parented to instance of A or vice-versa', () => {
    const scene = {
      objects: {
        A: makePrefab('A'),
        instanceOfA: makeInstance('instanceOfA', 'A'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'A', 'instanceOfA'))
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'instanceOfA', 'A'))
  })

  it('should not find a cycle in a simple hierarchy', () => {
    const scene = {
      objects: {
        1: makeObject('1'),
        2: makeObject('2'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '2', '1'))
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '1', '2'))
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '1', '1'))
  })

  it('should not find a cycle when the parentId is a space', () => {
    const scene = {
      spaces: {
        spaceId: {
          id: 'spaceId',
          name: 'space',
        },
      },
      objects: {
        1: makePrefab('1'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '1', 'spaceId'))
  })

  it('should not find a cycle in between unrelated instances', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
        3: makeInstance('3', '1'),
        4: makeInstance('4', '2'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '3', '4'))
  })

  it('should not find a cycle when the target is an instance of a prefab', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
        3: makeInstance('3', '1', {}, {parentId: '2'}),
        4: makePrefab('4'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '4', '3'))
  })

  it('should find a cycle when target is an instance of a prefab', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
        3: makeInstance('3', '1', {}, {parentId: '2'}),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, '2', '3'))
  })

  it('should find a cycle when parent id is an instance of a prefab', () => {
    const scene = {
      objects: {
        1: makePrefab('1'),
        2: makePrefab('2'),
        3: makeInstance('4', '1', {}, {parentId: '2'}),
        4: makeInstance('5', '2'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, '4', '3'))
  })

  it('should find a cycle when parent id is an instance of a prefab parented to another prefab',
    () => {
      const scene = {
        objects: {
          1: makePrefab('1'),
          2: makePrefab('2'),
          3: makePrefab('3'),
          4: makeInstance('4', '2', {}, {parentId: '1'}),
          5: makeInstance('5', '2', {}, {parentId: '3'}),
        },
      }
      const derivedScene = deriveScene(scene)
      assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, '4', '5'))
    })

  it('should not find a cycle when re-parenting prefab child objects under the same tree',
    () => {
      const scene = {
        objects: {
          1: makePrefab('1'),
          2: makeObject('2', {parentId: '1'}),
          3: makeObject('3', {parentId: '1'}),
        },
      }
      const derivedScene = deriveScene(scene)
      assert.isFalse(findPrefabDependencyCycle(scene, derivedScene, '2', '3'))
    })

  it('should check more complex dependencies A->B->C, check if instance of A can be in C', () => {
    const scene = {
      objects: {
        A: makePrefab('A'),
        B: makePrefab('B'),
        C: makePrefab('C'),
        instanceOfB: makeInstance('instanceOfB', 'B', {}, {parentId: 'A'}),
        instanceOfC: makeInstance('instanceOfC', 'C', {}, {parentId: 'B'}),
        childOfC: makeObject('childOfC', {parentId: 'C'}),
        instanceOfA: makeInstance('instanceOfA', 'A'),
      },
    }
    const derivedScene = deriveScene(scene)
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'instanceOfA', 'C'))
    assert.isTrue(findPrefabDependencyCycle(scene, derivedScene, 'instanceOfA', 'childOfC'))
  })
})

describe('Merged instances', () => {
  it('should correctly merge deletions', () => {
    const scene: SceneGraph = {
      objects: {
        prefab: makePrefab('prefab', {
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: '#ff0000'},
        }),
        instance: makeInstance('instance', 'prefab', {material: true}, {}),
      },
      spaces: {},
    }
    const merged = getObjectOrMergedInstance(scene, 'instance')
    assert.isUndefined(merged.material)
    assert.isOk(merged.geometry)
  })

  it('should correctly merge objects with component deletions', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parmeters: {},
    }
    const scene: SceneGraph = {
      objects: {
        prefab: makePrefab('prefab', testComponent),
        instance: makeInstance('instance', 'prefab', {components: {testComponent: true}}, {}),
      },
      spaces: {},
    }
    const merged = getObjectOrMergedInstance(scene, 'instance')
    assert.isUndefined(merged.components.testComponent)
  })

  it('should correctly merge instance children with deletions', () => {
    const scene: SceneGraph = {
      objects: {
        prefab: makePrefab('prefab'),
        child1: makeObject('child1', {
          parentId: 'prefab',
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: '#ff0000'},
        }),
        child2: makeObject('child2', {
          parentId: 'prefab',
          material: {type: 'basic', color: '#ff0000'},
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
        }),
        instance: makeInstance('instance', 'prefab', {}, {}, {
          child1: {id: 'child1', deletions: {geometry: true}},
          child2: {id: 'child2', deletions: {material: true}},
        }),
      },
      spaces: {},
    }

    const mergedChild1 = getObjectOrMergedInstance(scene, 'instance/child1')
    const mergedChild2 = getObjectOrMergedInstance(scene, 'instance/child2')
    assert.isUndefined(mergedChild1.geometry)
    assert.isOk(mergedChild1.material)
    assert.isOk(mergedChild2.geometry)
    assert.isUndefined(mergedChild2.material)
  })

  it('should correctly merge instance children with component deletions', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const scene: SceneGraph = {
      objects: {
        prefab: makePrefab('prefab', testComponent),
        child1: makeObject('child1', {parentId: 'prefab'}),
        instance: makeInstance('instance', 'prefab', {}, {}, {
          child1: {id: 'child1', deletions: {components: {testComponent: true}}},
        }),
      },
      spaces: {},
    }

    const mergedChild1 = getObjectOrMergedInstance(scene, 'instance/child1')
    assert.isUndefined(mergedChild1.components.testComponent)
  })
})

describe('resolveObjectReference', () => {
  it('should resolve normal object references to themselves', () => {
    const graph: SceneGraph = {
      objects: {
        object: makeObject('object'),
        prefab: makePrefab('prefab'),
        prefabChild: makeObject('prefabChild', {parentId: 'prefab'}),
        instance: makeInstance('instance', 'prefab'),
      },
      spaces: {},
    }

    resolveObjectReference(graph, 'object', 'prefab')
    assert.equal('prefab', resolveObjectReference(graph, 'object', 'prefab'))
    assert.equal('prefabChild', resolveObjectReference(graph, 'object', 'prefabChild'))
    assert.equal('object', resolveObjectReference(graph, 'instance', 'object'))
  })

  it(`resolve if the object is a prefab instance or instance child,
      and reference is to something within prefab`, () => {
    const graph: SceneGraph = {
      objects: {
        object: makeObject('object'),
        prefab: makePrefab('prefab'),
        prefabChild: makeObject('prefabChild', {parentId: 'prefab'}),
        instance: makeInstance('instance', 'prefab'),
      },
      spaces: {},
    }

    assert.equal('instance', resolveObjectReference(graph, 'instance', 'prefab'))
    assert.equal('instance/prefabChild', resolveObjectReference(graph, 'instance', 'prefabChild'))
    assert.equal('instance', resolveObjectReference(graph, 'instance/prefabChild', 'prefab'))
  })
})
