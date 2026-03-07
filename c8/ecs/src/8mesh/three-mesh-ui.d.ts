/* eslint-disable max-classes-per-file */
import type {Color, Object3D} from './utils/three-types'

type BlockOptions = {
  width?: number
  height?: number
  padding?: number
  fontFamily?: any
  fontTexture?: any
  backgroundColor?: Color
  fontColor?: Color
  backgroundOpacity?: number
  backgroundDepthWrite?: boolean
  foregroundDepthWrite?: boolean
  borderRadius?:
    number | [topLeft: number, topRight: number, bottomRight: number, bottomLeft: number]
  // @todo add missing properties
  [property: string]: any
}

declare class Block extends Object3D {
  constructor(options: BlockOptions)

  // @todo add typed properties and functions from mixin classes
  [property: string]: any // eslint-disable-line
}

type TextOptions = {
  // @todo add missing properties
  [property: string]: any
}

declare class Text extends Object3D {
  constructor(options: TextOptions)

  set(options: TextOptions): void
}

type InlineBlockOptions = {
  // @todo add missing properties
  [property: string]: any
}

declare class InlineBlock extends Object3D {
  constructor(options: InlineBlockOptions)
}

type KeyboardOptions = {
  // @todo add missing properties
  [property: string]: any
}

declare class Keyboard extends Object3D {
  constructor(options: KeyboardOptions)
}

declare namespace FontLibrary {
  function setFontFamily(): void
  function setFontTexture(): void
  function setFontTextures(): void
  function getFontOf(): void

  // @todo fix type
  function addFont(...args: any[]): any
  function addFontFromUrl(...args: any[]): any
}

declare function update(): void
declare global {
  namespace JSX {
    interface IntrinsicElements {
      block: any
    }
  }
}

export {
  BlockOptions,
  Block,
  Text,
  InlineBlock,
  Keyboard,
  FontLibrary,
  update,
}
