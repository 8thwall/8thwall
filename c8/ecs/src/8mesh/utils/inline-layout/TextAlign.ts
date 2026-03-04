import type {InlineWithChildren} from './inline-types'

const LEFT = 'left'
const RIGHT = 'right'
const CENTER = 'center'
const JUSTIFY = 'justify'
const JUSTIFY_LEFT = 'justify-left'
const JUSTIFY_RIGHT = 'justify-right'
const JUSTIFY_CENTER = 'justify-center'

type TextAlign = typeof LEFT | typeof RIGHT | typeof CENTER | typeof JUSTIFY |
typeof JUSTIFY_LEFT | typeof JUSTIFY_RIGHT | typeof JUSTIFY_CENTER

const _computeLineOffset = (
  lineWidth: number, alignment: TextAlign, innerWidth: number, lastLine: boolean
): number => {
  switch (alignment) {
    case JUSTIFY_LEFT:
    case JUSTIFY:
    case LEFT:
      return -innerWidth / 2

    case JUSTIFY_RIGHT:
    case RIGHT:
      return -lineWidth + (innerWidth / 2)

    case CENTER:
      return -lineWidth / 2

    case JUSTIFY_CENTER:
      if (lastLine) {
        // center alignement
        return -lineWidth / 2
      }

      // left alignment
      return -innerWidth / 2

    default:
      throw new Error(`textAlign: '${alignment}' is not valid`)
  }
}

const textAlign = (lines: InlineWithChildren, alignment: TextAlign, innerWidth: number): void => {
  // Start the alignment by sticking to directions : left, right, center
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i]

    // compute the alignment offset of the line
    const offsetX = _computeLineOffset(
      line.width ?? 0, alignment, innerWidth, i === lines.length - 1
    )

    // apply the offset to each characters of the line
    for (let j = 0; j < line.length; j++) {
      line[j].offsetX = (line[j].offsetX ?? 0) + offsetX
    }

    line.x = offsetX
  }

  // last operations for justifications alignments
  if (alignment.indexOf(JUSTIFY) === 0) {
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i]

      // do not process last line for justify-left or justify-right
      if (alignment.indexOf('-') !== -1 && i === lines.length - 1) return

      // can only justify is space is remaining
      const remainingSpace = innerWidth - (line.width ?? 0)
      if (remainingSpace <= 0) return

      // count the valid spaces to extend
      // Do not take the first nor the last space into account
      let validSpaces = 0
      for (let j = 1; j < line.length - 1; j++) {
        validSpaces += line[j].glyph === ' ' ? 1 : 0
      }
      const additionalSpace = remainingSpace / validSpaces

      // for right justification, process the loop in reverse
      let inverter = 1
      if (alignment === JUSTIFY_RIGHT) {
        line.reverse()
        inverter = -1
      }

      let incrementalOffsetX = 0

      // start at ONE to avoid first space
      for (let j = 1; j <= line.length - 1; j++) {
        // apply offset on each char
        const char = line[j]
        char.offsetX = (char.offsetX ?? 0) + (incrementalOffsetX * inverter)

        // and increase it when space
        incrementalOffsetX += char.glyph === ' ' ? additionalSpace : 0
      }

      // for right justification, the loop was processed in reverse
      if (alignment === JUSTIFY_RIGHT) {
        line.reverse()
      }
    }
  }
}

export {
  textAlign,
  LEFT,
  RIGHT,
  CENTER,
  JUSTIFY,
  JUSTIFY_LEFT,
  JUSTIFY_RIGHT,
  JUSTIFY_CENTER,
}

export type {
  TextAlign,
}
