import {createInstanced} from '../shared/instanced'
import type {RootAttribute, WorldAttribute} from './world-attribute'
import type {World} from './world'
import type {Eid} from '../shared/schema'
import {asm} from './asm'
import {createCursor} from './cursor'
import {getParent, setThreeParent} from './parent'

const disableObject = (world: World, eid: Eid) => {
  asm.entitySetEnabled(world._id, eid, false)
  const object = world.three.entityToObject.get(eid)
  if (object && object.parent) {
    object.removeFromParent()
  }
}

const disabledForWorld = createInstanced((world: World): WorldAttribute<{}> => {
  const cursor = createCursor(world, [])
  return ({
    set: (eid: Eid) => {
      disableObject(world, eid)
    },
    reset: (eid: Eid) => {
      disableObject(world, eid)
    },
    has: (eid: Eid) => !!asm.entityIsSelfDisabled(world._id, eid),
    remove: (eid: Eid) => {
      asm.entitySetEnabled(world._id, eid, true)
      setThreeParent(world, eid, getParent(world, eid))
    },
    dirty: () => {
      throw new Error('Invalid method for ecs.Disabled')
    },
    acquire: () => {
      throw new Error('Invalid method for ecs.Disabled')
    },
    commit: () => {
      throw new Error('Invalid method for ecs.Disabled')
    },
    get: (eid: Eid) => {
      if (!asm.entityIsSelfDisabled(world._id, eid)) {
        throw new Error('Entity is not disabled, cannot call ecs.Disabled.get')
      }
      return cursor
    },
    mutate: () => {
      throw new Error('Invalid method for ecs.Disabled')
    },
    cursor: () => {
      throw new Error('Invalid method for ecs.Disabled')
    },
    id: asm.getDisabledComponentId(world._id),
  })
})
const Disabled: RootAttribute<{}> = {
  set: (world, eid) => disabledForWorld(world).set(eid),
  reset: (world, eid) => disabledForWorld(world).reset(eid),
  dirty: (world, eid) => disabledForWorld(world).dirty(eid),
  get: (world, eid) => disabledForWorld(world).get(eid),
  cursor: (world, eid) => disabledForWorld(world).cursor(eid),
  acquire: (world, eid) => disabledForWorld(world).acquire(eid),
  commit: (world, eid) => disabledForWorld(world).commit(eid),
  mutate: (world, eid, fn) => disabledForWorld(world).mutate(eid, fn),
  has: (world, eid) => disabledForWorld(world).has(eid),
  remove: (world, eid) => disabledForWorld(world).remove(eid),
  forWorld: disabledForWorld,
  schema: {},
  orderedSchema: [],
  defaults: {},
}
export {
  Disabled,
}
