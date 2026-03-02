import type {EmscriptenModule} from './types/emscripten'

const writeStringToEmscriptenHeap = (xrcc: EmscriptenModule, str: string) => {
  const ptr = xrcc._malloc(str.length + 1)
  // eslint-disable-next-line no-restricted-properties
  xrcc.stringToUTF8(str, ptr, str.length + 1)
  return ptr
}

const writeArrayToEmscriptenHeap = (xrcc: EmscriptenModule, data: Float32Array | Uint32Array) => {
  const {length} = data
  const nDataBytes = length * data.BYTES_PER_ELEMENT
  const ptr = xrcc._malloc(nDataBytes)
  xrcc.writeArrayToMemory(new Uint8Array(data.buffer), ptr)
  return {ptr, length}
}

export {writeStringToEmscriptenHeap, writeArrayToEmscriptenHeap}
