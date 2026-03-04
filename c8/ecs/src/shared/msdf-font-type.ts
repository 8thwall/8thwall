// @attr[](visibility = "//reality/app/nae/packager:__subpackages__")
// @attr[](visibility = "//c8/ecs:__subpackages__")
// @attr[](visibility = "//reality/cloud:shared")

type CharData = {
  id: number
  index: number
  char: string
  width: number
  height: number
  xoffset: number
  yoffset: number
  xadvance: number
  x: number
  y: number
  page: number
}

type FontInfo = {
  face: string
  size: number
  bold: number
  italic: number
  charset: string[]
  unicode: number
  stretchH: number
  smooth: number
  aa: number
  padding: number[]
  spacing: number[]
  outline: number
}

type Kerning = {
  first: number
  second: number
  amount: number
}

type RawFontSetData = {
  fontFile?: string
  pages: string[]
  chars: CharData[]
  info: FontInfo
  common: {
    lineHeight: number
    base: number
    scaleW: number
    scaleH: number
    pages: number
    packed: number
    alphaChnl: number
    redChnl: number
    greenChnl: number
    blueChnl: number
  }
  distanceField: {
    fieldType: 'mtsdf' | 'sdf' | 'msdf'
    distanceRange: number
  }
  kernings: Kerning[]
}

type FontSetData = RawFontSetData & {
  _kernings: Record<string, number>
  _chars: Record<string, CharData>
  _charset: Set<string>
}

const isProcessedFontSetData = (font: FontSetData | RawFontSetData): font is FontSetData => (
  '_kernings' in font && '_chars' in font && '_charset' in font
)

export type {
  RawFontSetData,
  FontSetData,
  CharData,
}

export {
  isProcessedFontSetData,
}
