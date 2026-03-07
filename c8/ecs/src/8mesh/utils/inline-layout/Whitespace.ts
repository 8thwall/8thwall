/* eslint-disable max-len */
/**
 * @see https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace#whitespace_helper_functions
 *
 * Throughout, whitespace is defined as one of the characters
 *  "\t" TAB \u0009
 *  "\n" LF  \u000A
 *  "\r" CR  \u000D
 *  " "  SPC \u0020
 *
 * This does not use Javascript's "\s" because that includes non-breaking
 * spaces (and also some other characters).
 * */
/* eslint-enable max-len */

import type {Inline, LineBreak} from './inline-types'

const WHITE_CHARS = {'\t': '\u0009', '\n': '\u000A', '\r': '\u000D', ' ': '\u0020'}
const WHITE_CHARS_KEYS = new Set(Object.keys(WHITE_CHARS))
const NORMAL = 'normal'
const NOWRAP = 'nowrap'
const PRE = 'pre'
const PRE_LINE = 'pre-line'
const PRE_WRAP = 'pre-wrap'

type WhiteSpace = typeof NORMAL | typeof NOWRAP | typeof PRE | typeof PRE_LINE | typeof PRE_WRAP

/** ************************************************************************************************
 * Internal logics BEGIN
 ************************************************************************************************ */

/**
 * Visually collapse inlines from right to left ( endtrim )
 * @param {Array} inlines
 * @param targetInline
 * @private
 */
// TODO (tri): rewrite this in a more functional way
function _collapseRightInlines(inlines: Inline[], targetInline?: Inline): void {
  if (!targetInline) return

  for (let i = 0; i < inlines.length; i++) {
    const inline = inlines[i]
    inline.width = 0
    inline.height = 0
    inline.offsetX = (targetInline.offsetX ?? 0) + (targetInline.width ?? 0)
  }
}

/**
 * Visually collapse inlines from left to right (starttrim)
 * @param {Array} inlines
 * @param targetInline
 * @private
 */
// TODO (tri): rewrite this in a more functional way
function _collapseLeftInlines(inlines: Inline[], targetInline?: Inline): void {
  if (!targetInline) return

  for (let i = 0; i < inlines.length; i++) {
    const inline = inlines[i]
    inline.width = 0
    inline.height = 0
    inline.offsetX = targetInline.offsetX
  }
}

/**
 * get the distance in world coord to the next glyph defined
 * as break-line-safe ( like whitespace for instance )
 * @private
 */
// TODO (tri): rewrite this in a more functional way
function _distanceToNextBreak(
  inlines: Inline[],
  currentIdx: number,
  letterSpacing: number,
  accumulator?: number
): number {
  // eslint-disable-next-line no-param-reassign
  accumulator = accumulator ?? 0

  // end of the text
  if (!inlines[currentIdx]) return accumulator

  const inline = inlines[currentIdx]
  const kerning = inline.kerning ?? 0
  const xoffset = inline.xoffset ?? 0
  const xadvance = inline.xadvance ?? inline.width ?? 0

  // if inline.lineBreak is set, it is 'mandatory' or 'possible'
  if (inline.lineBreak) return accumulator + xadvance

  // no line break is possible on this character
  return _distanceToNextBreak(
    inlines,
    currentIdx + 1,
    letterSpacing,
    accumulator + xadvance + letterSpacing + xoffset + kerning
  )
}

/**
 * Test if we should line break here even if the current glyph is not out of boundary.
 * It might be necessary if the last glyph was break-line-friendly (whitespace, hyphen..)
 * and the distance to the next friendly glyph is out of boundary.
 * @private
 * @param prevChar - The previous character
 * @param lastInlineOffset - The offset of the last character
 * @param nextBreak - The distance to the next break
 * @param innerWidth - The inner width
 * @param breakOn - The characters to break on
 * @returns {boolean}
 */
function _shouldFriendlyBreak(
  prevChar: Inline,
  lastInlineOffset: number,
  nextBreak: number,
  innerWidth: number,
  breakOn: string[]
): boolean {
  // We can't check if last glyph is break-line-friendly it does not exist
  if (!prevChar || !prevChar.glyph) {
    return false
  }

  // Next break-line-friendly glyph is inside boundary
  if (lastInlineOffset + nextBreak < innerWidth) {
    return false
  }

  // Previous glyph was break-line-friendly
  return breakOn.indexOf(prevChar.glyph) > -1
}

/** ************************************************************************************************
 * Internal logics END
 ************************************************************************************************ */

/**
 * Collapse white spaces and sequence of white spaces on string
 *
 * @param textContent
 * @param whiteSpace
 * @returns {string}
 */
function collapseWhitespaceOnString(textContent: string, whiteSpace: WhiteSpace): string {
  let content = textContent
  if (whiteSpace === NOWRAP || whiteSpace === NORMAL) {
    content = textContent.replace(/\n/g, ' ')
  }
  if (whiteSpace === PRE_LINE) {
    content = textContent.replace(/[ ]{2,}/g, ' ')
  }
  return content
}

/**
 * Get the breakability of a newline character according to white-space property
 *
 * @param whiteSpace
 * @returns {LineBreak | void}
 */
// eslint-disable-next-line consistent-return
function newlineBreakability(whiteSpace: WhiteSpace): LineBreak | void {
  switch (whiteSpace) {
    case PRE:
    case PRE_WRAP:
    case PRE_LINE:
      return 'mandatory'

    case NOWRAP:
    case NORMAL:
    default:
      // do not automatically break on newline
  }
}

/**
 * Check for breaks in inlines according to whiteSpace value
 *
 * @param inlines
 * @param i
 * @param lastInlineOffset
 * @param options
 * @returns {boolean}
 */
// TODO (tri): ensure that this function is pure and does not mutate any of its arguments
function shouldBreak(
  inlines: Inline[],
  i: number,
  lastInlineOffset: number,
  whitespace: WhiteSpace,
  letterSpacing: number,
  breakOn: string[],
  innerWidth: number
): boolean {
  const inline = inlines[i]

  switch (whitespace) {
    case NORMAL:
    case PRE_LINE:
    case PRE_WRAP:

      // prevent additional computation if line break is mandatory
      if (inline.lineBreak === 'mandatory') return true

      /* eslint-disable no-case-declarations */
      const kerning = inline.kerning ?? 0
      const xoffset = inline.xoffset ?? 0
      const xadvance = inline.xadvance ?? inline.width ?? 0

      // prevent additional computation if this character already exceed the available size
      if (lastInlineOffset + xadvance + xoffset + kerning > innerWidth) return true

      const nextBreak = _distanceToNextBreak(inlines, i, letterSpacing)
      return _shouldFriendlyBreak(
        inlines[i - 1],
        lastInlineOffset,
        nextBreak,
        innerWidth,
        breakOn
      )
      /* eslint-enable no-case-declarations */

    case PRE:
      return inline.lineBreak === 'mandatory'

    case NOWRAP:
    default:
      return false
  }
}

/**
 * Alter a line according to white-space property
 * @param line
 * @param {('normal'|'pre-wrap'|'pre-line')} whiteSpace
 * @returns {number}
 */
// TODO (tri): completely rewrite or remove this function for a more functional approach
function collapseWhitespaceOnInlines(line: Inline[], whiteSpace: WhiteSpace): number {
  const firstInline = line[0]
  const lastInline = line[line.length - 1]

  // @see https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace
  //
  // current implementation is 'pre-line'
  // if the breaking character is a space, get the previous one
  switch (whiteSpace) {
    // trim/collapse first and last whitespace characters of a line
    case PRE_WRAP:
      // only process whiteChars glyphs inlines
      // if( firstInline.glyph && whiteChars[firstInline.glyph] && line.length > 1 ){
      if (firstInline.glyph && firstInline.glyph === '\n' && line.length > 1) {
        _collapseLeftInlines([firstInline], line[1])
      }

      // if( lastInline.glyph && whiteChars[lastInline.glyph] && line.length > 1 ){
      if (lastInline.glyph && lastInline.glyph === '\n' && line.length > 1) {
        _collapseRightInlines([lastInline], line[line.length - 2])
      }

      break

    case PRE_LINE:
    case NOWRAP:
    case NORMAL:
      /* eslint-disable no-case-declarations */
      let inlinesToCollapse = []
      let collapsingTarget
      // collect starting whitespaces to collapse
      for (let i = 0; i < line.length; i++) {
        const inline = line[i]
        if (inline.glyph && WHITE_CHARS_KEYS.has(inline.glyph) && line.length > i) {
          inlinesToCollapse.push(inline)
          collapsingTarget = line[i + 1]
          // eslint-disable-next-line no-continue
          continue
        }

        break
      }

      _collapseLeftInlines(inlinesToCollapse, collapsingTarget)
      inlinesToCollapse = []
      collapsingTarget = undefined
      // collect ending whitespace to collapse
      for (let i = line.length - 1; i > 0; i--) {
        const inline = line[i]
        if (inline.glyph && WHITE_CHARS_KEYS.has(inline.glyph) && i > 0) {
          inlinesToCollapse.push(inline)
          collapsingTarget = line[i - 1]
          // eslint-disable-next-line no-continue
          continue
        }

        break
      }

      _collapseRightInlines(inlinesToCollapse, collapsingTarget)
      break
      /* eslint-enable no-case-declarations */

    case PRE:
      break

    default:
      // eslint-disable-next-line no-console
      console.warn(`whiteSpace: '${whiteSpace}' is not valid`)
      return 0
  }

  return firstInline.offsetX ?? 0
}

export {
  collapseWhitespaceOnString,
  collapseWhitespaceOnInlines,
  newlineBreakability,
  shouldBreak,
  NORMAL,
  NOWRAP,
  PRE,
  PRE_LINE,
  PRE_WRAP,
  WHITE_CHARS,
}

export type {
  WhiteSpace,
}
