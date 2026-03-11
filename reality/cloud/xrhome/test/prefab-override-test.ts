import type {
  BaseGraphObject,
  GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {isOverriddenInstance} from '../src/client/studio/configuration/prefab'

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

describe('isOverriddenInstance', () => {
  it('instance with no overrides is not overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isFalse(isOverriddenInstance(graph, instance.id), 'Instance should be overridden')
  })

  it('direct overrides makes instance overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {
      material: {type: 'basic', color: '#ff0000'},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(isOverriddenInstance(graph, instance.id), 'Instance should be overridden')
  })

  it('deletions makes instance overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {material: true}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(isOverriddenInstance(graph, instance.id), 'Instance should be overridden')
  })

  it('component overrides makes instance overridden', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const testComponentOverridden = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const prefab = makePrefab('prefab', {components: {testComponent}})
    const instance = makeInstance('instance', 'prefab', {}, {
      components: {testComponent: testComponentOverridden},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(isOverriddenInstance(graph, instance.id), 'Instance should be overridden')
  })

  it('component deletions makes instance overridden', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const prefab = makePrefab('prefab', {components: {testComponent}})
    const instance = makeInstance('instance', 'prefab', {components: {testComponent: true}}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(isOverriddenInstance(graph, instance.id), 'Instance should be overridden')
  })

  it('instance child with no overrides is not overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isFalse(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should not be overridden'
    )
  })

  it('direct overrides makes instance child overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        material: {type: 'basic', color: '#ff0000'},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should be overridden'
    )
  })

  it('deletions makes instance child overridden', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        deletions: {material: true},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should be overridden'
    )
  })

  it('component overrides makes instance child overridden', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const testComponentOverridden = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      components: {testComponent},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        components: {testComponent: testComponentOverridden},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should be overridden'
    )
  })

  it('component deletions makes instance child overridden', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {},
    }
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      components: {testComponent},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        deletions: {components: {testComponent: true}},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should be overridden'
    )
  })

  it('order override does not make root instance overridden', () => {
    const prefab = makePrefab('prefab', {material: {type: 'basic', color: '#ffffff'}})
    const instance = makeInstance('instance', 'prefab', {}, {order: 5})
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isFalse(isOverriddenInstance(graph, instance.id), 'Instance should not be overridden')
  })

  it('order override makes instance child overridden', () => {
    const prefab = makePrefab('prefab', {material: {type: 'basic', color: '#ffffff'}})
    const prefabChild = makePrefab('prefabChild', {
      parentId: 'prefab',
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {prefabChild: {order: 5}})
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    assert.isTrue(
      isOverriddenInstance(graph, 'instance/prefabChild'),
      'Instance child should be overridden'
    )
  })
})
