import {type ContentDirection, ROW} from './ContentDirection'

const START = 'start'
const CENTER = 'center'
const END = 'end'
const STRETCH = 'stretch'  // Still bit experimental
const TOP = 'top'  // @TODO: Be remove upon 7.x.x
const RIGHT = 'right'  // @TODO: Be remove upon 7.x.x
const BOTTOM = 'bottom'  // @TODO: Be remove upon 7.x.x
const LEFT = 'left'  // @TODO: Be remove upon 7.x.x

type AlignItems = typeof START | typeof CENTER | typeof END | typeof STRETCH | typeof TOP |
  typeof RIGHT | typeof BOTTOM | typeof LEFT

const AVAILABLE_ALIGN_ITEMS = [
  START,
  CENTER,
  END,
  STRETCH,
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
]

// TODO (tri) type boxComponent with MeshUIComponent
const alignItems = (boxComponent: any, direction: ContentDirection) => {
  const alignment = boxComponent.getAlignItems()
  if (!AVAILABLE_ALIGN_ITEMS.includes(alignment as AlignItems)) {
    // eslint-disable-next-line no-console
    console.warn(`alignItems === '${alignment}' is not supported`)
  }

  let getSizeMethod = 'getWidth'
  let axis = 'x'
  if (direction.indexOf(ROW) === 0) {
    getSizeMethod = 'getHeight'
    axis = 'y'
  }
  const axisTarget = (boxComponent[getSizeMethod]() / 2) - (boxComponent.padding || 0)
  // TODO (tri) type child with MeshUIComponent
  boxComponent.childrenBoxes.forEach((child: any) => {
    let offset

    // eslint-disable-next-line default-case
    switch (alignment) {
      case END:
      case RIGHT:  // @TODO : Deprecated and will be remove upon 7.x.x
      case BOTTOM:  // @TODO : Deprecated and will be remove upon 7.x.x
        if (direction.indexOf(ROW) === 0) {
          offset = -axisTarget + (child[getSizeMethod]() / 2) + (child.margin || 0)
        } else {
          offset = axisTarget - (child[getSizeMethod]() / 2) - (child.margin || 0)
        }

        break

      case START:
      case LEFT:  // @TODO : Deprecated and will be remove upon 7.x.x
      case TOP:  // @TODO : Deprecated and will be remove upon 7.x.x
        if (direction.indexOf(ROW) === 0) {
          offset = axisTarget - (child[getSizeMethod]() / 2) - (child.margin || 0)
        } else {
          offset = -axisTarget + (child[getSizeMethod]() / 2) + (child.margin || 0)
        }

        break
    }

    boxComponent.childrenPos[child.id][axis] = offset || 0
  })
}

// @TODO: Be remove upon 7.x.x
const DEPRECATED_ALIGN_ITEMS = [
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
]

/**
 * @deprecated
 * // @TODO: Be remove upon 7.x.x
 * @param alignment
 */
const warnAboutDeprecatedAlignItems = (alignment: AlignItems) => {
  if (DEPRECATED_ALIGN_ITEMS.indexOf(alignment) !== -1) {
    // eslint-disable-next-line no-console
    console.warn(
      `alignItems === '${alignment}' is deprecated and will be remove in 7.x.x. ` +
      'Fallback are \'start\'|\'end\''
    )
  }
}

export {
  alignItems,
  warnAboutDeprecatedAlignItems,
  START,
  CENTER,
  END,
  STRETCH,
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
}

export type {AlignItems}
