/* eslint-disable import/group-exports, camelcase */

// Adapted from
//   https://github.com/DefinitelyTyped/DefinitelyTyped/blob/master/types/emscripten/index.d.ts

type RenderContext = WebGL2RenderingContext | WebGLRenderingContext

type ContextAttributes = ReturnType<RenderContext['getContextAttributes']>

type GlLibrary = {
  makeContextCurrent: (handle: number) => void
  getNewId: (table: EmscriptenTexture[]) => number
  textures: EmscriptenTexture[]
  registerContext: (context: RenderContext, attributes: ContextAttributes) => number
  stringCache: Record<string, unknown>
}

export type EmscriptenTexture = WebGLTexture & {name: number}

export interface EmscriptenModule {

  HEAP8: Int8Array
  HEAP16: Int16Array
  HEAP32: Int32Array
  HEAPU8: Uint8Array
  HEAPU16: Uint16Array
  HEAPU32: Uint32Array
  HEAPF32: Float32Array
  HEAPF64: Float64Array

  // Specified in EXPORTED_FUNCTIONS
  _malloc(size: number): number
  _free(ptr: number): void

  // Specified in EXPORTED_RUNTIME_METHODS
  GL: GlLibrary
  stringToUTF8(str: string, outPtr: number, maxBytesToRead?: number): void
  writeArrayToMemory(array: number[] | Uint8Array, buffer: number): void
}
