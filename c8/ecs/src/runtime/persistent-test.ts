// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {Eid} from '../shared/schema'

describe('Persistent Entity test', () => {
  before(async () => {
    await ecs.ready()
  })
  it('Can manage a persistent entity', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = args.join(' ')
    }

    // @ts-ignore
    const countObjects = (scene) => {
      let count = 0
      // @ts-ignore
      scene.traverse((object) => {
        if (object.userData.eid) {
          count++
        }
      })

      return count
    }

    const world = window.ecs.createWorld(...initThree())

    const expectSceneObjects = (expected: number, label: string) => {
      const actual = countObjects(world.three.scene)
      assert.equal(expected, actual, `${label} on Test: ${currentMsg}`)
    }

    const expectSameEid = (eid1: Eid, eid2: Eid, label: string) => {
      assert.equal(eid1, eid2,
        `${label} (expected: ${eid1}, actual: ${eid2}) on Test: ${currentMsg}`)
    }

    const sceneGraph = {
      'objects': {
        '7e98572b-0ccc-4f7d-911d-5782b9e8c988': {
          'id': '7e98572b-0ccc-4f7d-911d-5782b9e8c988',
          'position': [
            0,
            0.5,
            0,
          ],
          'rotation': [
            0,
            0,
            0,
            1,
          ],
          'scale': [
            1,
            1,
            1,
          ],
          'geometry': {
            'type': 'box',
            'depth': 1,
            'height': 1,
            'width': 1,
          },
          'material': {
            'type': 'basic',
            'color': '#7611b6',
          },
          'components': {},
          'parentId': '989fbb13-7f6a-45ce-9305-5a7296afa0ce',
          'name': 'Box',
        },
        '9c6c48f1-434b-4225-a80c-596cc94db1b5': {
          'id': '9c6c48f1-434b-4225-a80c-596cc94db1b5',
          'position': [
            0,
            0.5,
            0,
          ],
          'rotation': [
            0,
            0,
            0,
            1,
          ],
          'scale': [
            1,
            1,
            1,
          ],
          'geometry': {
            'type': 'box',
            'depth': 1,
            'height': 1,
            'width': 1,
          },
          'material': {
            'type': 'basic',
            'color': '#7611b6',
          },
          'components': {},
          'parentId': '7e98572b-0ccc-4f7d-911d-5782b9e8c988',
          'name': 'Box',
        },
        'ac1989e3-3b71-49e2-a05f-e682aeb18c36': {
          'id': 'ac1989e3-3b71-49e2-a05f-e682aeb18c36',
          'position': [
            20,
            20,
            10,
          ],
          'rotation': [
            0,
            0,
            0,
            1,
          ],
          'scale': [
            1,
            1,
            1,
          ],
          'geometry': null,
          'material': null,
          'components': {},
          'parentId': '7e98572b-0ccc-4f7d-91d1-5782b9e8c988',
          'name': 'Directional Light',
          'light': {
            'type': 'directional',
            'intensity': 1,
          },
        },
      },
      'spaces': {
        '989fbb13-7f6a-45ce-9305-5a7296afa0ce': {
          'id': '989fbb13-7f6a-45ce-9305-5a7296afa0ce',
          'name': 'Space1',
        },
        '7e98572b-0ccc-4f7d-91d1-5782b9e8c988': {
          'id': '7e98572b-0ccc-4f7d-91d1-5782b9e8c988',
          'name': 'Space2',
        },
      },
      'entrySpaceId': '989fbb13-7f6a-45ce-9305-5a7296afa0ce',
    }

    // @ts-ignore
    const sceneHandle = world.loadScene(sceneGraph)

    world.setSceneHook(sceneHandle)

    let eid: Eid = 0n
    let childEid: Eid = 0n

    const setEids = () => {
      eid = sceneHandle.graphIdToEid.get('7e98572b-0ccc-4f7d-911d-5782b9e8c988') as Eid
      assert(!!sceneHandle.eidToObject.get(eid), 'eid should be set')
      childEid = sceneHandle.graphIdToEid.get('9c6c48f1-434b-4225-a80c-596cc94db1b5') as Eid
      assert(!!sceneHandle.eidToObject.get(childEid), 'childEid should be set')
    }

    const resetPersistentAndSpace = () => {
      if (eid) {
        ecs.Persistent.remove(world, eid)
      }
      if (childEid) {
        ecs.Persistent.remove(world, childEid)
      }
      world.spaces.loadSpace('Space1')
      setEids()
    }

    // initial start
    world.tick()
    setEids()

    log('Switching to Space2 deletes all entity objects in Space1')
    expectSceneObjects(2, 'initial scene objects')
    world.spaces.loadSpace('Space2')
    world.tick()
    expectSceneObjects(1, 'scene objects after switching to Space2')

    resetPersistentAndSpace()
    world.tick()

    log('Set/Remove persistent on an entity')
    expectSceneObjects(2, 'initial scene objects')
    ecs.Persistent.set(world, eid)
    world.spaces.loadSpace('Space2')
    world.tick()
    expectSceneObjects(3, 'scene objects after setting persistent on an entity')

    resetPersistentAndSpace()
    world.tick()

    log('Set/Remove persistent on an entity and switch back to Space1')
    expectSceneObjects(2, 'initial scene objects')
    const oldEid = eid
    ecs.Persistent.set(world, eid)
    world.spaces.loadSpace('Space2')
    setEids()
    world.tick()
    expectSceneObjects(3, 'scene objects after setting persistent on an entity')
    world.spaces.loadSpace('Space1')
    setEids()
    world.tick()
    expectSceneObjects(2, 'scene objects after switching back to Space1')
    expectSameEid(oldEid, eid, 'same eid after switching back to Space1')
  })
})
