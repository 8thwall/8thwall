const writeStringToEmscriptenHeap = (module, str: string) => {
  const numBytes = module.lengthBytesUTF8(str) + 1
  const ptr = module._malloc(numBytes)
  module.stringToUTF8(str, ptr, numBytes)
  return ptr
}

const writeArrayToEmscriptenHeap = (module, data: Uint8Array) => {
  const ptr = module._malloc(data.byteLength)
  module.writeArrayToMemory(data, ptr)
  return ptr
}

export {
  writeStringToEmscriptenHeap,
  writeArrayToEmscriptenHeap,
}
