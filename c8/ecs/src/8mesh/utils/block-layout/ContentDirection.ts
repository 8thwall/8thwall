const ROW = 'row'
const ROW_REVERSE = 'row-reverse'
const COLUMN = 'column'
const COLUMN_REVERSE = 'column-reverse'

type ContentDirection = typeof ROW | typeof ROW_REVERSE | typeof COLUMN | typeof COLUMN_REVERSE

function contentDirection(
  // TODO (tri) type container with MeshUIContainer
  container: any,
  direction: ContentDirection,
  startPos: number,
  reverse: number
) {
  // end to end children
  let accumulator = startPos

  let childGetSize = 'getWidth'
  let axisPrimary = 'x'
  let axisSecondary = 'y'

  if (direction.indexOf(COLUMN) === 0) {
    childGetSize = 'getHeight'
    axisPrimary = 'y'
    axisSecondary = 'x'
  }

  // Refactor reduce into `for i` in order to get rid of this keyword
  for (let i = 0; i < container.childrenBoxes.length; i++) {
    const child = container.childrenBoxes[i]
    const childId = child.id
    const childSize = child[childGetSize]()
    const childMargin = child.margin || 0

    accumulator += childMargin * reverse

    container.childrenPos[childId] = {
      [axisPrimary]: accumulator + ((childSize / 2) * reverse),
      [axisSecondary]: 0,
    }

    // update accumulator for next children
    accumulator += (reverse * (childSize + childMargin))
  }
}

export {
  contentDirection,
  ROW,
  ROW_REVERSE,
  COLUMN,
  COLUMN_REVERSE,
}

export type {ContentDirection}
