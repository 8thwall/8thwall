import type {AlignItems, FlexDirection, JustifyContent} from '@ecs/shared/flex-style-types'

import {Align, AlignDirection} from '../../../ui/components/visual-aligner'

const FLEX_ALIGN_TO_ALIGN_DIRECTION: Record<JustifyContent | AlignItems, AlignDirection> = {
  'flex-start': AlignDirection.START,
  'center': AlignDirection.CENTER,
  'flex-end': AlignDirection.END,
  'space-between': AlignDirection.SPACED,
  'space-around': AlignDirection.SPACED,
  'space-evenly': AlignDirection.SPACED,
  'baseline': AlignDirection.CENTER,
  'stretch': AlignDirection.CENTER,
}

const flipAlign = (align: AlignDirection) => {
  switch (align) {
    case AlignDirection.START:
      return AlignDirection.END
    case AlignDirection.END:
      return AlignDirection.START
    default:
      return align
  }
}

const extractJustifyContent = (
  flexDirection: FlexDirection, alignment: JustifyContent
) => {
  switch (flexDirection) {
    case 'row-reverse':
    case 'column-reverse':
      return flipAlign(FLEX_ALIGN_TO_ALIGN_DIRECTION[alignment])
    default:
      return FLEX_ALIGN_TO_ALIGN_DIRECTION[alignment]
  }
}

const getAlignments = (
  flexDirection: FlexDirection, justifyContent: JustifyContent, alignItems: AlignItems
): Align => {
  if (['row', 'row-reverse'].includes(flexDirection)) {
    return [
      extractJustifyContent(flexDirection, justifyContent),
      FLEX_ALIGN_TO_ALIGN_DIRECTION[alignItems]]
  }

  return [
    FLEX_ALIGN_TO_ALIGN_DIRECTION[alignItems],
    extractJustifyContent(flexDirection, justifyContent),
  ]
}

const getAlignItemsFromDirection = (
  direction: AlignDirection
): AlignItems => {
  switch (direction) {
    case AlignDirection.START:
      return 'flex-start'
    case AlignDirection.CENTER:
      return 'center'
    case AlignDirection.END:
      return 'flex-end'
    case AlignDirection.SPACED:
      return 'stretch'
    default:
      return 'flex-start'
  }
}

const getJustifyContentFromDirection = (
  direction: AlignDirection,
  isReversed: boolean
): JustifyContent => {
  switch (direction) {
    case AlignDirection.START:
      return isReversed ? 'flex-end' : 'flex-start'
    case AlignDirection.CENTER:
      return 'center'
    case AlignDirection.END:
      return isReversed ? 'flex-start' : 'flex-end'
    case AlignDirection.SPACED:
      return 'space-between'
    default:
      return 'flex-start'
  }
}

export {
  getAlignments,
  getAlignItemsFromDirection,
  getJustifyContentFromDirection,
}
