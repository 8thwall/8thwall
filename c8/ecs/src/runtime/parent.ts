import type {World} from './world'
import {asm} from './asm'
import type {Eid} from '../shared/schema'
import {WASM_POINTER_SIZE} from './constants'
import {extractEids} from './list-data'
import {addChild} from './matrix-refresh'

const setThreeParent = (world: World, eid: Eid, parentEid: Eid) => {
  const object = world.three.entityToObject.get(eid)
  if (object) {
    const newParent = world.three.entityToObject.get(parentEid) || world.three.scene
    addChild(newParent, object)
  }
}

const setParent = (world: World, eid: Eid, parentEid: Eid) => {
  asm.entitySetParent(world._id, eid, parentEid)
  const disabled = asm.entityIsSelfDisabled(world._id, eid)
  if (!disabled) {
    setThreeParent(world, eid, parentEid)
  }
}

const getParent = (world: World, eid: Eid) => asm.entityGetParent(world._id, eid)

function* getChildren(world: World, parent: Eid) {
  const listPtr = asm._malloc(WASM_POINTER_SIZE)
  const count = asm.entityGetChildren(world._id, parent, listPtr)
  for (const eid of extractEids(listPtr, count)) {
    yield eid
  }
  // This frees the entity reference list
  asm._free(asm.dataView.getUint32(listPtr, true))
  asm._free(listPtr)
}

export {
  setParent,
  setThreeParent,
  getParent,
  getChildren,
}
