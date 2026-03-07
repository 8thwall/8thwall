import {ROW, type ContentDirection} from './ContentDirection'

const START = 'start'
const CENTER = 'center'
const END = 'end'
const SPACE_AROUND = 'space-around'
const SPACE_BETWEEN = 'space-between'
const SPACE_EVENLY = 'space-evenly'

type JustifyContent = typeof START | typeof CENTER | typeof END |
  typeof SPACE_AROUND | typeof SPACE_BETWEEN | typeof SPACE_EVENLY

const AVAILABLE_JUSTIFICATIONS = [
  START,
  CENTER,
  END,
  SPACE_AROUND,
  SPACE_BETWEEN,
  SPACE_EVENLY,
]

/**
 *
 * @param {JustifyContent} justification
 * @param {number} axisOffset
 * @returns {number}
 */
const _getJustificationOffset = (justification: JustifyContent, axisOffset: number) => {
  // Only end and center have justification offset
  switch (justification) {
    case END:
      return axisOffset
    case CENTER:
      return axisOffset / 2
    default:
      return 0
  }
}

/**
 *
 * @param items
 * @param spaceToDistribute
 * @param justification
 * @param reverse
 * @returns {any[]}
 */
const _getJustificationMargin = (
  items: any[], spaceToDistribute: number, justification: JustifyContent, reverse: number
) => {
  const justificationMargins = Array(items.length).fill(0)
  if (spaceToDistribute > 0) {
    // Only space-*  have justification margin betweem items
    // eslint-disable-next-line default-case
    switch (justification) {
      case SPACE_BETWEEN:
        // only one children would act as start
        if (items.length > 1) {
          const margin = (spaceToDistribute / (items.length - 1)) * reverse
          // set this margin for any children

          // except for first child
          justificationMargins[0] = 0

          for (let i = 1; i < items.length; i++) {
            justificationMargins[i] = margin * i
          }
        }

        break

      case SPACE_EVENLY:
        // only one children would act as start
        if (items.length > 1) {
          const margin = (spaceToDistribute / (items.length + 1)) * reverse

          // set this margin for any children
          for (let i = 0; i < items.length; i++) {
            justificationMargins[i] = margin * (i + 1)
          }
        }

        break

      case SPACE_AROUND:
        // only one children would act as start
        if (items.length > 1) {
          const margin = (spaceToDistribute / (items.length)) * reverse

          const start = margin / 2
          justificationMargins[0] = start

          // set this margin for any children
          for (let i = 1; i < items.length; i++) {
            justificationMargins[i] = start + margin * i
          }
        }

        break
    }
  }

  return justificationMargins
}

const justifyContent = (
  // TODO (tri) type boxComponent with MeshUIContainer
  boxComponent: any,
  direction: ContentDirection,
  startPos: number,
  reverse: number
) => {
  const justification = boxComponent.getJustifyContent()
  if (!AVAILABLE_JUSTIFICATIONS.includes(justification as JustifyContent)) {
    // eslint-disable-next-line no-console
    console.warn(`justifyContent === '${justification}' is not supported`)
  }

  const side = direction.indexOf(ROW) === 0 ? 'width' : 'height'
  const usedDirectionSpace = boxComponent.getChildrenSideSum(side)
  const innerSize = side === 'width' ? boxComponent.getInnerWidth() : boxComponent.getInnerHeight()
  const remainingSpace = innerSize - usedDirectionSpace

  // Items Offset
  const axisOffset = (startPos * 2) - (usedDirectionSpace * Math.sign(startPos))
  // const axisOffset = ( startPos * 2 ) - ( usedDirectionSpace * REVERSE );
  const justificationOffset = _getJustificationOffset(justification, axisOffset)

  // Items margin
  const justificationMargins = _getJustificationMargin(
    boxComponent.childrenBoxes, remainingSpace, justification, reverse
  )

  // Apply
  const axis = direction.indexOf(ROW) ? 'x' : 'y'
  boxComponent.childrenBoxes.forEach((child: any, childIndex: number) => {
    // eslint-disable-next-line max-len
    boxComponent.childrenPos[child.id][axis] -= justificationOffset - justificationMargins[childIndex]
  })
}

export {
  justifyContent,
  START,
  CENTER,
  END,
  SPACE_AROUND,
  SPACE_BETWEEN,
  SPACE_EVENLY,
}

export type {
  JustifyContent,
}
