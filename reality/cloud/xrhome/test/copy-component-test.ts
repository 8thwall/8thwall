import type {
  BaseGraphObject,
  GraphComponent,
  GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {
  pasteCustomComponentPropertiesIntoObject,
  pasteDirectPropertiesIntoObject,
} from '../src/client/studio/configuration/copy-component'

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

const makeComponent = (
  id: string,
  name: string,
  extra?: Partial<GraphComponent>
): GraphComponent => ({
  id,
  name,
  parameters: {},
  ...extra,
})

describe('copy component tests', () => {
  it('should be able to copy a direct component into a normal object', () => {
    const objectA = makeObject('objectA', {geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const objectB = makeObject('objectB', {geometry: {type: 'capsule', height: 2, radius: 1}})
    const graph: SceneGraph = {
      objects: {
        objectA,
        objectB,
      },
      spaces: {},
    }

    const copiedComponent = {...objectA.geometry}
    const newScene = pasteDirectPropertiesIntoObject(
      graph, ['objectB'], {geometry: copiedComponent}
    )

    const expectedObjectB = {
      ...objectB,
      geometry: copiedComponent,
    }
    assert.deepEqual(newScene.objects.objectB, expectedObjectB)
  })

  it('should be able to copy a direct component into a instance of a prefab', () => {
    const object = makeObject('object', {geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const prefab = makePrefab('prefab', {geometry: {type: 'capsule', height: 2, radius: 1}})
    const instance = makeInstance('instance', 'prefab')
    const graph: SceneGraph = {
      objects: {
        object,
        prefab,
        instance,
      },
      spaces: {},
    }

    const copiedComponent = {...object.geometry}
    const newScene = pasteDirectPropertiesIntoObject(
      graph, ['instance'], {geometry: copiedComponent}
    )

    const expectedInstance = {
      ...instance,
      geometry: copiedComponent,
    }
    assert.deepEqual(newScene.objects.instance, expectedInstance)
  })

  it('should be able to copy a direct component into a child instance of a prefab', () => {
    const object = makeObject('object', {geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const prefab = makePrefab('prefab', {geometry: {type: 'capsule', height: 2, radius: 1}})
    const prefabChild =
      makeObject('prefabChild', {geometry: {type: 'sphere', radius: 1}, parentId: 'prefab'})
    const instance = makeInstance('instance', 'prefab')

    const graph: SceneGraph = {
      objects: {
        object,
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const copiedComponent = {...object.geometry}
    const newScene = pasteDirectPropertiesIntoObject(
      graph, ['instance/prefabChild'], {geometry: copiedComponent}
    )

    const expectedInstance = {
      ...instance,
      instanceData: {
        ...instance.instanceData,
        children: {
          prefabChild: {
            geometry: copiedComponent,
          },
        },
      },
    }
    assert.deepEqual(newScene.objects.instance, expectedInstance)
  })

  it('should paste custom component into a normal object', () => {
    const objectA = makeObject('objectA', {
      components: {
        componentA: makeComponent('componentA', 'myComponent', {parameters: {value: 42}}),
      },
    })
    const componentB = makeComponent('componentB', 'otherComponent', {parameters: {value: 100}})
    const objectB = makeObject('objectB', {
      components: {
        componentB,
      },
    })

    const graph: SceneGraph = {
      objects: {objectA, objectB},
      spaces: {},
    }

    const componentToCopy = objectA.components.componentA
    const newScene = pasteCustomComponentPropertiesIntoObject(
      graph, ['objectB'], componentToCopy
    )

    const expectedObjectB = {
      ...objectB,
      components: {
        componentA: {...componentToCopy},
        componentB: {...componentB},
      },
    }
    assert.deepEqual(newScene.objects.objectB, expectedObjectB)
  })

  it('should paste custom component into a prefab instance', () => {
    const componentB = makeComponent('componentB', 'otherComponent', {parameters: {value: 50}})
    const prefab = makePrefab('prefab', {components: {}})
    const instance = makeInstance('instance', 'prefab', {}, {
      components: {
        componentB,
      },
    })
    const sourceObj = makeObject('sourceObj', {
      components: {
        componentA: makeComponent('componentA', 'myComponent', {parameters: {value: 100}}),
      },
    })

    const graph: SceneGraph = {
      objects: {prefab, instance, sourceObj},
      spaces: {},
    }

    const componentToCopy = sourceObj.components.componentA
    const newScene = pasteCustomComponentPropertiesIntoObject(
      graph, ['instance'], componentToCopy
    )

    const expectedInstance = {
      ...instance,
      components: {
        componentA: {...componentToCopy},
        componentB: {...componentB},
      },
    }
    assert.deepEqual(newScene.objects.instance, expectedInstance)
  })

  it('should paste custom component into a child of a prefab instance', () => {
    const componentB = makeComponent('componentB', 'otherComponent', {parameters: {value: 75}})
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild = makeObject('prefabChild', {
      components: {},
      parentId: 'prefab',
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        components: {
          componentB,
        },
      },
    })
    const sourceObj2 = makeObject('sourceObj2', {
      components: {
        componentA: makeComponent('componentA', 'myComponent', {parameters: {value: 200}}),
      },
    })

    const graph: SceneGraph = {
      objects: {prefab, prefabChild, instance, sourceObj2},
      spaces: {},
    }

    const componentToCopy = sourceObj2.components.componentA
    const newScene = pasteCustomComponentPropertiesIntoObject(
      graph, ['instance/prefabChild'], componentToCopy
    )

    const expectedInstance = {
      ...instance,
      instanceData: {
        ...instance.instanceData,
        children: {
          prefabChild: {
            components: {
              componentA: {...componentToCopy},
              componentB: {...componentB},
            },
          },
        },
      },
    }
    assert.deepEqual(newScene.objects.instance, expectedInstance)
  })

  it('should paste custom component into a child of a prefab instance with existing component',
    () => {
      const componentB = makeComponent('componentB', 'otherComponent', {parameters: {value: 25}})
      const prefab = makePrefab('prefab', {components: {}})
      const prefabChild = makeObject('prefabChild', {
        components: {
          oldCompId: makeComponent('oldCompId', 'myComponent', {parameters: {value: 5}}),
        },
        parentId: 'prefab',
      })
      const instance = makeInstance(
        'instance',
        'prefab',
        {},
        {},
        {
          prefabChild: {
            components: {
              oldCompId: makeComponent('oldCompId', 'myComponent', {parameters: {value: 5}}),
              componentB,
            },
          },
        }
      )
      const sourceObj3 = makeObject('sourceObj3', {
        components: {
          componentA: makeComponent('componentA', 'myComponent', {parameters: {value: 999}}),
        },
      })

      const graph: SceneGraph = {
        objects: {prefab, prefabChild, instance, sourceObj3},
        spaces: {},
      }

      const componentToCopy = sourceObj3.components.componentA
      const newScene = pasteCustomComponentPropertiesIntoObject(
        graph, ['instance/prefabChild'], componentToCopy
      )

      const expectedInstance = {
        ...instance,
        instanceData: {
          ...instance.instanceData,
          children: {
            prefabChild: {
              components: {
                oldCompId: {...componentToCopy, id: 'oldCompId'},
                componentB: {...componentB},
              },
            },
          },
        },
      }
      assert.deepEqual(newScene.objects.instance, expectedInstance)
    })
})
