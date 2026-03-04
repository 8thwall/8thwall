type LineBreak = 'mandatory' | 'possible' | null

// TODO (tri) remove this type when the inline-layout calculation is refactor to not use this
// layering of array types anymore
type Inline = Partial<{
  height: number
  width: number
  anchor: number
  xadvance: number
  xoffset: number
  lineBreak: LineBreak
  glyph: string
  fontSize: number
  lineHeight: number
  lineBase: number
  // Processed Data
  x: number
  y: number
  offsetX: number
  offsetY: number
  kerning: number
  interLine: number
}>

type InlineWithChildren = Inline & Array<InlineWithChildren>

export type {Inline, LineBreak, InlineWithChildren}
