/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import type {
  Node,
} from 'yoga-layout'

import {
  Align,
  Direction,
  Display,
  Edge,
  FlexDirection,
  Gutter,
  Justify,
  Overflow,
  PositionType,
  Wrap,
} from './flex-enum'

import type {UiRuntimeSettings} from '../runtime/ui-types'

type LayoutNode = Node & {
  setDirection: (direction: Direction) => void
  M: {O: number}
}

const alignContent = (str?: string): Align => {
  switch (str) {
    case undefined:
    case '':
    case 'flex-start':
      return Align.FlexStart
    case 'flex-end':
      return Align.FlexEnd
    case 'center':
      return Align.Center
    case 'stretch':
      return Align.Stretch
    case 'space-between':
      return Align.SpaceBetween
    case 'space-around':
      return Align.SpaceAround
    case 'space-evenly':
      return Align.SpaceEvenly
    default:
      throw new Error(`"${str}" is not a valid value for alignContent`)
  }
}

const alignItems = (str?: string): Align => {
  switch (str) {
    case undefined:
    case '':
    case 'flex-start':
      return Align.FlexStart
    case 'flex-end':
      return Align.FlexEnd
    case 'center':
      return Align.Center
    case 'stretch':
      return Align.Stretch
    case 'baseline':
      return Align.Baseline
    default:
      throw new Error(`"${str}" is not a valid value for alignItems`)
  }
}

const alignSelf = (str?: string): Align => {
  switch (str) {
    case undefined:
    case '':
    case 'auto':
      return Align.Auto
    case 'flex-start':
      return Align.FlexStart
    case 'flex-end':
      return Align.FlexEnd
    case 'center':
      return Align.Center
    case 'stretch':
      return Align.Stretch
    case 'baseline':
      return Align.Baseline
    default:
      throw new Error(`"${str}" is not a valid value for alignSelf`)
  }
}

const display = (str?: string): Display => {
  switch (str) {
    case 'none':
      return Display.None
    case undefined:
    case '':
    case 'flex':
      return Display.Flex
    default:
      throw new Error(`"${str}" is not a valid value for display`)
  }
}

const flexDirection = (str?: string): FlexDirection => {
  switch (str) {
    case undefined:
    case '':
    case 'row':
      return FlexDirection.Row
    case 'column':
      return FlexDirection.Column
    case 'row-reverse':
      return FlexDirection.RowReverse
    case 'column-reverse':
      return FlexDirection.ColumnReverse
    default:
      throw new Error(`"${str}" is not a valid value for flexDirection`)
  }
}

const flexWrap = (str?: string): Wrap => {
  switch (str) {
    case 'wrap':
      return Wrap.Wrap
    case undefined:
    case '':
    case 'nowrap':
      return Wrap.NoWrap
    case 'wrap-reverse':
      return Wrap.WrapReverse
    default:
      throw new Error(`"${str}" is not a valid value for flexWrap`)
  }
}

const justifyContent = (str?: string): Justify => {
  switch (str) {
    case '':
    case 'flex-start':
      return Justify.FlexStart
    case 'flex-end':
      return Justify.FlexEnd
    case 'center':
      return Justify.Center
    case 'space-between':
      return Justify.SpaceBetween
    case 'space-around':
      return Justify.SpaceAround
    case 'space-evenly':
      return Justify.SpaceEvenly
    default:
      throw new Error(`"${str}" is not a valid value for justifyContent`)
  }
}

const overflow = (str?: string): Overflow => {
  switch (str) {
    case undefined:
    case '':
    case 'visible':
      return Overflow.Visible
    case 'hidden':
      return Overflow.Hidden
    case 'scroll':
      return Overflow.Scroll
    default:
      throw new Error(`"${str}" is not a valid value for overflow`)
  }
}

const position = (str?: string): PositionType => {
  switch (str) {
    case 'absolute':
      return PositionType.Absolute
    case 'relative':
      return PositionType.Relative
    case undefined:
    case '':
    case 'static':
      return PositionType.Static
    default:
      throw new Error(`"${str}" is not a valid value for position`)
  }
}

const numberOrPercent = (str?: string): number | `${number}%` | undefined => {
  if (str === '' || str === undefined) {
    return undefined
  }
  return str as number | `${number}%`
}

const numberPercentOrAuto = (str?: string): 'auto' | `${number}%` | undefined => {
  if (str === '' || str === undefined) {
    return undefined
  }
  return str as 'auto' | `${number}%`
}

const applyStyle = (node: LayoutNode, style: UiRuntimeSettings): void => {
  node.setAlignContent(alignContent(style.alignContent))
  node.setAlignItems(alignItems(style.alignItems))
  node.setAlignSelf(alignSelf(style.alignSelf))
  node.setBorder(Edge.All, style.borderWidth)
  node.setPositionType(position(style.position))
  // TODO (tri) remove or revise this logic when `static` is supported
  if (position(style.position) === PositionType.Static) {
    node.setPosition(Edge.Top, undefined)
    node.setPosition(Edge.Bottom, undefined)
    node.setPosition(Edge.Left, undefined)
    node.setPosition(Edge.Right, undefined)
  } else {
    node.setPosition(Edge.Top, numberOrPercent(style.top))
    node.setPosition(Edge.Bottom, numberOrPercent(style.bottom))
    node.setPosition(Edge.Left, numberOrPercent(style.left))
    node.setPosition(Edge.Right, numberOrPercent(style.right))
  }
  node.setDisplay(display(style.display))
  node.setFlex(style.flex)
  node.setFlexBasis(numberPercentOrAuto(style.flexBasis))
  node.setFlexDirection(flexDirection(style.flexDirection))
  node.setGap(Gutter.All, numberOrPercent(style.gap))
  node.setGap(Gutter.Row, numberOrPercent(style.rowGap))
  node.setGap(Gutter.Column, numberOrPercent(style.columnGap))
  node.setFlexGrow(style.flexGrow)
  node.setFlexShrink(style.flexShrink)
  node.setFlexWrap(flexWrap(style.flexWrap))
  node.setHeight(numberOrPercent(style.height))
  node.setJustifyContent(justifyContent(style.justifyContent))
  node.setMargin(Edge.All, numberPercentOrAuto(style.margin))
  node.setMargin(Edge.Bottom, numberPercentOrAuto(style.marginBottom))
  node.setMargin(Edge.Left, numberPercentOrAuto(style.marginLeft))
  node.setMargin(Edge.Right, numberPercentOrAuto(style.marginRight))
  node.setMargin(Edge.Top, numberPercentOrAuto(style.marginTop))
  node.setMaxHeight(numberOrPercent(style.maxHeight))
  node.setMaxWidth(numberOrPercent(style.maxWidth))
  node.setMinHeight(numberOrPercent(style.minHeight))
  node.setMinWidth(numberOrPercent(style.minWidth))
  node.setOverflow(overflow(style.overflow))
  node.setPadding(Edge.All, numberOrPercent(style.padding))
  node.setPadding(Edge.Bottom, numberOrPercent(style.paddingBottom))
  node.setPadding(Edge.Left, numberOrPercent(style.paddingLeft))
  node.setPadding(Edge.Right, numberOrPercent(style.paddingRight))
  node.setPadding(Edge.Top, numberOrPercent(style.paddingTop))
  node.setWidth(numberOrPercent(style.width))
}

export type {
  LayoutNode,
}
export {
  applyStyle,
  numberPercentOrAuto,
}
