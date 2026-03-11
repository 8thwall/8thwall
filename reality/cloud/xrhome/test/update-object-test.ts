// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import type {
  BaseGraphObject, GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import type {DeepReadonly} from 'ts-essentials'

import {updateObject} from '../src/client/studio/update-object'
import type {MutateCallback} from '../src/client/studio/scene-context'

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

const makeInstance = (
  id: string, instanceOf: string, deletions?: {}, extra?: Partial<GraphObject>,
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
    deletions: deletions || {},
    ...(children !== undefined ? {children} : {}),
  },
  ...extra,
})

const performObjectUpdate = (
  oldScene: DeepReadonly<SceneGraph>,
  objectId: string,
  cb: MutateCallback<BaseGraphObject>
): DeepReadonly<SceneGraph> => {
  let newScene = oldScene
  updateObject((updateCb) => {
    newScene = updateCb(oldScene)
  }, objectId, cb)
  return newScene
}

describe('updateObject', () => {
  it('Can overwrite component properties on a prefab instance and children', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'testValue',
      parameters: {
        param1: 'value1',
        param2: 'value2',
      },
    }
    const prefab = makePrefab('prefab', {
      components: {
        testComponent,
      },
    })
    const child1 = makeObject('child1', {
      parentId: 'prefab',
      components: {
        testComponent: {
          ...testComponent,
        },
      },
    })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
        child1,
      },
      spaces: {},
    }
    let newScene = performObjectUpdate(oldScene, 'instance', old => ({
      ...old,
      components: {
        testComponent: {
          ...testComponent,
          parameters: {
            ...testComponent.parameters,
            param1: 'newValue1',
          },
        },
      },
    }))

    newScene = performObjectUpdate(newScene, 'instance/child1', old => ({
      ...old,
      components: {
        testComponent: {
          ...testComponent,
          parameters: {
            ...testComponent.parameters,
            param1: 'newValue1',
          },
        },
      },
    }))

    assert.deepEqual(newScene.objects.instance.components, {
      testComponent: {
        ...testComponent,
        parameters: {
          param1: 'newValue1',
          param2: 'value2',
        },
      },
    })

    assert.deepEqual(newScene.objects.instance.instanceData.children.child1.components, {
      testComponent: {
        ...testComponent,
        parameters: {
          param1: 'newValue1',
          param2: 'value2',
        },
      },
    })
  })

  it('Can update a property of a prefab instance', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      material: {type: 'basic', color: '#FFFFFF'},
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      material: {type: 'basic', color: '#FFFFFF'},
    })
  })

  it('Can override a property of a prefab instance that the root prefab has', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#FFFFFF'},
    })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      material: {type: 'basic', color: '#000000'},
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      material: {type: 'basic', color: '#000000'},
    })
  })

  it('Can delete a property on the prefab from the instance', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#FFFFFF'},
    })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      material: undefined,
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      material: undefined,
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {material: true}},
    })
  })

  it('Adding a property to the instance that is in deletions should remove it from deletions',
    () => {
      const prefab = makePrefab('prefab', {
        material: {type: 'basic', color: '#FFFFFF'},
      })
      const instance = makeInstance('instance', 'prefab', {material: true})
      const oldScene = {
        objects: {
          prefab,
          instance,
        },
        spaces: {},
      }
      const newScene = performObjectUpdate(oldScene, instance.id, old => ({
        ...old,
        material: {type: 'basic', color: '#000000'},
      }))
      assert.deepEqual(newScene.objects.instance, {
        id: instance.id,
        name: '<unset>',
        position: [0, 0, 0],
        rotation: [0, 0, 0, 1],
        scale: [1, 1, 1],
        material: {type: 'basic', color: '#000000'},
        components: {},
        instanceData: {instanceOf: 'prefab', deletions: {}},
      })
    })

  it('Can update/delete/re-add hidden to instance', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#FFFFFF'},
      hidden: true,
    })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    let newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      hidden: false,
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      hidden: false,
    })

    newScene = performObjectUpdate(newScene, instance.id, old => ({
      ...old,
      hidden: undefined,
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {hidden: true}},
      hidden: undefined,
    })
  })

  it('Can update/delete/re-add hidden to instance where prefab does not have hidden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#FFFFFF'},
    })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    let newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      hidden: false,
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      hidden: false,
    })

    newScene = performObjectUpdate(newScene, instance.id, old => ({
      ...old,
      hidden: undefined,
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: '<unset>',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      hidden: undefined,
    })
  })

  it('If we make a non-deletion change to an object, instance data should be equal', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#FFFFFF'},
    })
    const instance = makeInstance('instance', 'prefab', {hidden: true})
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      material: {type: 'basic', color: '#000000'},
    }))
    assert.strictEqual(
      newScene.objects.instance.instanceData,
      instance.instanceData
    )
  })

  it('You can update parentId/prefab/name of an instance', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, instance.id, old => ({
      ...old,
      parentId: 'newParent',
      prefab: true,
      name: 'newName',
    }))
    assert.deepEqual(newScene.objects.instance, {
      id: instance.id,
      name: 'newName',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      instanceData: {instanceOf: 'prefab', deletions: {}},
      parentId: 'newParent',
      prefab: true,
    })
  })

  it('Updating properties of instance child correctly updates instanceData.children for scoped id',
    () => {
      const prefab = makePrefab('prefab')
      const child1 = makeObject('child1', {
        parentId: 'prefab', material: {type: 'basic', color: '#FFFFFF'},
      })
      const child2 = makeObject('child2', {parentId: 'child1'})
      const instance = makeInstance('instance', 'prefab', {}, {}, {
        'child1': {
          id: 'child1',
          deletions: {},
        },
        'child2': {
          id: 'child2',
          deletions: {},
        },
      })
      const oldScene = {
        objects: {
          prefab,
          child1,
          child2,
          instance,
        },
        spaces: {},
      }
      const newScene = performObjectUpdate(oldScene, 'instance/child1', old => ({
        ...old,
        material: {type: 'basic', color: '#000000'},
      }))
      assert.deepEqual(newScene.objects.instance.instanceData.children, {
        'child1': {
          id: 'child1',
          deletions: {},
          material: {type: 'basic', color: '#000000'},
        },
        'child2': {
          id: 'child2',
          deletions: {},
        },
      })
    })

  it('Removing a property from an instance child correctly adds it to deletions', () => {
    const prefab = makePrefab('prefab')
    const child = makeObject('child', {
      parentId: 'prefab', material: {type: 'basic', color: '#FFFFFF'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      'child': {
        id: 'child',
        deletions: {},
      },
    })
    const oldScene = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, 'instance/child', old => ({
      ...old,
      material: undefined,
    }))
    assert.deepEqual(newScene.objects.instance.instanceData.children, {
      'child': {
        id: 'child',
        deletions: {material: true},
        material: undefined,
      },
    })
  })

  it('Removing a property from an instance child and adding it back removes it from deletions',
    () => {
      const prefab = makePrefab('prefab')
      const child = makeObject('child', {
        parentId: 'prefab', material: {type: 'basic', color: '#FFFFFF'},
      })
      const instance = makeInstance('instance', 'prefab', {}, {}, {
        'child': {
          id: 'child',
          deletions: {material: true},
        },
      })
      const oldScene = {
        objects: {
          prefab,
          child,
          instance,
        },
        spaces: {},
      }
      const newScene = performObjectUpdate(oldScene, 'instance/child', old => ({
        ...old,
        material: {type: 'basic', color: '#000000'},
      }))
      assert.deepEqual(newScene.objects.instance.instanceData.children, {
        'child': {
          id: 'child',
          deletions: {},
          material: {type: 'basic', color: '#000000'},
        },
      })
    })

  it('handle deletions for root instance components', () => {
    const prefab = makePrefab('prefab',
      {components: {testComponent: {id: 'testComponent', name: 'testValue', parameters: {}}}})
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, 'instance', old => ({
      ...old,
      components: {},
    }))

    assert.deepEqual(newScene.objects.instance.instanceData, {
      instanceOf: 'prefab',
      deletions: {
        components: {
          testComponent: true,
        },
      },
    })
  })

  it('handle deletions for scoped children components', () => {
    const prefab = makePrefab('prefab',
      {components: {testComponent: {id: 'testComponent', name: 'testValue', parameters: {}}}})
    const child = makeObject('child', {
      parentId: 'prefab',
      components: {testComponent: {id: 'testComponent', name: 'testValue', parameters: {}}},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      'child': {
        id: 'child',
        deletions: {},
      },
    })
    const oldScene = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, 'instance/child', old => ({
      ...old,
      components: {},
    }))

    assert.deepEqual(newScene.objects.instance.instanceData.children, {
      'child': {
        id: 'child',
        components: {},
        deletions: {
          components: {
            testComponent: true,
          },
        },
      },
    })
  })

  it('handle multiple deletions for root instance components', () => {
    const prefab = makePrefab('prefab',
      {
        components: {
          testComponent1: {id: 'testComponent1', name: 'testValue', parameters: {}},
          testComponent2: {id: 'testComponent2', name: 'testValue', parameters: {}},
          testComponent3: {id: 'testComponent3', name: 'testValue', parameters: {}},
        },
      })
    const instance = makeInstance('instance', 'prefab')
    const oldScene = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, 'instance', old => ({
      ...old,
      components: {
        testComponent1: {id: 'testComponent1', name: 'testValue', parameters: {}},
        testComponent3: {id: 'testComponent3', name: 'testValue', parameters: {}},
      },
    }))

    assert.deepEqual(newScene.objects.instance.instanceData, {
      instanceOf: 'prefab',
      deletions: {
        components: {
          testComponent2: true,
        },
      },
    })
  })

  it('handle multiple deletions for scoped children components', () => {
    const component1 = {id: 'testComponent1', name: 'testValue', parameters: {}}
    const component2 = {id: 'testComponent2', name: 'testValue', parameters: {}}
    const component3 = {id: 'testComponent3', name: 'testValue', parameters: {}}
    const prefab = makePrefab('prefab',
      {components: {testComponent1: component1, testComponent3: component3}})
    const child = makeObject('child', {
      parentId: 'prefab',
      components: {
        testComponent1: component1,
        testComponent2: component2,
        testComponent3: component3,
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      'child': {
        id: 'child',
      },
    })
    const oldScene = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }
    const newScene = performObjectUpdate(oldScene, 'instance/child', old => ({
      ...old,
      components: {
        testComponent1: component1,
        testComponent3: component3,
      },
    }))

    assert.deepEqual(newScene.objects.instance.instanceData.children, {
      'child': {
        id: 'child',
        components: {
          testComponent1: component1,
          testComponent3: component3,
        },
        deletions: {
          components: {
            testComponent2: true,
          },
        },
      },
    })
  })

  it('Removing a component from an instance root and adding it back removes it from deletions',
    () => {
      const testComponent = {id: 'testComponent', name: 'testValue', parameters: {}}
      const prefab = makePrefab('prefab', {
        components: {testComponent},
      })
      const instance = makeInstance('instance', 'prefab', {
        components: {testComponent: true},
      })
      const oldScene = {
        objects: {
          prefab,
          instance,
        },
        spaces: {},
      }
      const newScene = performObjectUpdate(oldScene, 'instance', old => ({
        ...old,
        components: {'testComponent': testComponent},
      }))
      assert.deepEqual(newScene.objects.instance.instanceData, {
        instanceOf: 'prefab',
        deletions: {
          components: {},
        },
      })
      assert.deepEqual(newScene.objects.instance.components, {
        'testComponent': testComponent,
      })
    })

  it('Removing a component from an instance child and adding it back removes it from deletions',
    () => {
      const testComponent = {id: 'testComponent', name: 'testValue', parameters: {}}
      const prefab = makePrefab('prefab')
      const child = makeObject('child', {
        parentId: 'prefab', components: {testComponent},
      })
      const instance = makeInstance('instance', 'prefab', {}, {}, {
        'child': {
          id: 'child',
          deletions: {components: {testComponent: true}},
        },
      })
      const oldScene = {
        objects: {
          prefab,
          child,
          instance,
        },
        spaces: {},
      }
      const newScene = performObjectUpdate(oldScene, 'instance/child', old => ({
        ...old,
        components: {'testComponent': testComponent},
      }))
      assert.deepEqual(newScene.objects.instance.instanceData.children, {
        'child': {
          id: 'child',
          deletions: {
            components: {},
          },
          components: {'testComponent': testComponent},
        },
      })
    })
})
