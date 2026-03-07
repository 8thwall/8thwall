const WASM_POINTER_SIZE = 4  // WASM is always 32 bit as of now.
const FLOAT_SIZE = 4  // Floats are 32 bits.
const UINT32_SIZE = 4  // Uint32 are 32 bits.
const ENTITY_ID_SIZE = 8  // Entities in flecs are 64 bits.

export {
  WASM_POINTER_SIZE,
  ENTITY_ID_SIZE,
  FLOAT_SIZE,
  UINT32_SIZE,
}
