import type {Eid} from '../shared/schema'
import {asm} from './asm'
import {ENTITY_ID_SIZE, WASM_POINTER_SIZE} from './constants'

function* extractListData(listPtr: number, count: number): Generator<number> {
// NOTE(christoph): Wasm pointers are 32 bit.
  for (let i = 0; i < count; i++) {
    yield asm.dataView.getUint32(listPtr + WASM_POINTER_SIZE * i, true)
  }
}

function* extractEids(listPtr: number, count: number): Generator<Eid> {
// NOTE(christoph): Wasm pointers are 32 bit.
  const ptr = asm.dataView.getUint32(listPtr, true)
  for (let i = 0; i < count; i++) {
    yield asm.dataView.getBigInt64(ptr + i * ENTITY_ID_SIZE, true)
  }
}

export {
  extractListData,
  extractEids,
}
