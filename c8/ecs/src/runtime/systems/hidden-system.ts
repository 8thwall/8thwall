import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Hidden, Ui} from '../components'
import {makeSystemHelper} from './system-helper'

const dirtyUiEntities = (world: World, eid: Eid) => {
  if (Ui.has(world, eid)) {
    Ui.dirty(world, eid)
  }

  for (const child of world.getChildren(eid)) {
    dirtyUiEntities(world, child)
  }
}

const makeHiddenSystem = (world: World) => {
  const {enter, exit} = makeSystemHelper(Hidden)

  const hide = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object) {
      object.visible = false
      dirtyUiEntities(world, eid)
    }
  }

  const show = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)
    if (object) {
      object.visible = true
      dirtyUiEntities(world, eid)
    }
  }

  return () => {
    exit(world).forEach(show)
    enter(world).forEach(hide)
  }
}

export {
  makeHiddenSystem,
}
