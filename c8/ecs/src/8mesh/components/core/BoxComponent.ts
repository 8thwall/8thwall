// @ts-nocheck
/* eslint-disable consistent-return */
/**

Job: Handle everything related to a BoxComponent element dimensioning and positioning

Knows: Parents and children dimensions and positions

It's worth noting that in three-mesh-ui, it's the parent Block that computes
its children position. A Block can only have either only box components (Block)
as children, or only inline components (Text, InlineBlock).

 */

import * as ContentDirection from '../../utils/block-layout/ContentDirection'
import * as AlignItems from '../../utils/block-layout/AlignItems'
import {justifyContent} from '../../utils/block-layout/JustifyContent'

const COLUMN_DIRECTIONS = [ContentDirection.COLUMN, ContentDirection.COLUMN_REVERSE]
const ROW_DIRECTIONS = [ContentDirection.ROW, ContentDirection.ROW_REVERSE]

export default function BoxComponent(Base) {
  // eslint-disable-next-line @typescript-eslint/no-shadow
  return class BoxComponent extends Base {
    constructor(options) {
      super(options)
      this.isBoxComponent = true
      this.childrenPos = {}
    }

    /** Get width of this component minus its padding */
    getInnerWidth() {
      const direction = this.getContentDirection()

      switch (direction) {
        case ContentDirection.ROW:
        case ContentDirection.ROW_REVERSE:
          return this.width - (this.padding * 2 || 0) || this.getChildrenSideSum('width')

        case ContentDirection.COLUMN:
        case ContentDirection.COLUMN_REVERSE:
          return this.getHighestChildSizeOn('width')

        default:
          // eslint-disable-next-line no-console
          console.error(`Invalid contentDirection : ${direction}`)
          break
      }
    }

    /** Get height of this component minus its padding */
    getInnerHeight() {
      const direction = this.getContentDirection()

      switch (direction) {
        case ContentDirection.ROW:
        case ContentDirection.ROW_REVERSE:
          return this.getHighestChildSizeOn('height')

        case ContentDirection.COLUMN:
        case ContentDirection.COLUMN_REVERSE:
          return this.height - (this.padding * 2 || 0) || this.getChildrenSideSum('height')

        default:
          // eslint-disable-next-line no-console
          console.error(`Invalid contentDirection : ${direction}`)
          break
      }
    }

    /** Return the sum of all this component's children sides + their margin */
    getChildrenSideSum(dimension) {
      return this.childrenBoxes.reduce((accu, child) => {
        const margin = (child.margin * 2) || 0

        const childSize = (dimension === 'width')
          ? (child.getWidth() + margin)
          : (child.getHeight() + margin)
        return accu + childSize
      }, 0)
    }

    /** Look in parent record what is the instructed position for this component,
     * then set its position */
    setPosFromParentRecords() {
      if (this.parentUI && this.parentUI.childrenPos[this.id]) {
        this.position.x = (this.parentUI.childrenPos[this.id].x)
        this.position.y = (this.parentUI.childrenPos[this.id].y)
      }
    }

    /** Position inner elements according to dimensions and layout parameters. */
    computeChildrenPosition() {
      if (this.children.length > 0) {
        const direction = this.getContentDirection()
        let directionalOffset

        // eslint-disable-next-line default-case
        switch (direction) {
          case ContentDirection.ROW:
            directionalOffset = -this.getInnerWidth() / 2
            break

          case ContentDirection.ROW_REVERSE:
            directionalOffset = this.getInnerWidth() / 2
            break

          case ContentDirection.COLUMN:
            directionalOffset = this.getInnerHeight() / 2
            break

          case ContentDirection.COLUMN_REVERSE:
            directionalOffset = -this.getInnerHeight() / 2
            break
        }

        const reverse = -Math.sign(directionalOffset)
        ContentDirection.contentDirection(this, direction, directionalOffset, reverse)
        justifyContent(this, direction, directionalOffset, reverse)
        AlignItems.alignItems(this, direction)
      }
    }

    /**
     * Returns the highest linear dimension among all the children of the passed component
     * MARGIN INCLUDED
     */
    getHighestChildSizeOn(direction: 'width' | 'height') {
      return this.childrenBoxes.reduce((accu, child) => {
        const margin = child.margin || 0
        const maxSize = direction === 'width'
          ? child.getWidth() + (margin * 2)
          : child.getHeight() + (margin * 2)
        return Math.max(accu, maxSize)
      }, 0)
    }

    /**
     * Get width of this element
     * With padding, without margin
     */
    getWidth() {
      // This is for stretch alignment
      // @TODO : Conceive a better performant way
      if (this.parentUI && this.parentUI.getAlignItems() === AlignItems.STRETCH) {
        // eslint-disable-next-line max-len
        if (!COLUMN_DIRECTIONS.includes(this.parentUI.getContentDirection())) {
          return this.parentUI.getWidth() - (this.parentUI.padding * 2 || 0)
        }
      }

      return this.width || this.getInnerWidth() + (this.padding * 2 || 0)
    }

    /**
     * Get height of this element
     * With padding, without margin
     */
    getHeight() {
      // This is for stretch alignment
      // @TODO : Conceive a better performant way
      if (this.parentUI && this.parentUI.getAlignItems() === AlignItems.STRETCH) {
        // eslint-disable-next-line max-len
        if (!ROW_DIRECTIONS.includes(this.parentUI.getContentDirection())) {
          return this.parentUI.getHeight() - (this.parentUI.padding * 2 || 0)
        }
      }

      return this.height || this.getInnerHeight() + (this.padding * 2 || 0)
    }
  }
}
