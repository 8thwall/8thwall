// @visibility(//visibility:public)

import Yoga, {Overflow} from './yoga'

import {type LayoutNode, applyStyle} from '../shared/flex-styles'
import type {UiRuntimeSettings, LayoutMetrics} from './ui-types'

const overflowToString = (overflow: Overflow): 'visible' | 'hidden' | 'scroll' | undefined => {
  switch (overflow) {
    case Overflow.Hidden:
      return 'hidden'
    case Overflow.Scroll:
      return 'scroll'
    case Overflow.Visible:
      return 'visible'
    default:
      return undefined
  }
}

const metricsFromLayoutNode = (node: LayoutNode): LayoutMetrics => {
  const parent = node.getParent()
  const parentRelatedMetrics = parent
    ? {
      parentWidth: parent.getComputedWidth(),
      parentHeight: parent.getComputedHeight(),
    }
    : null
  return {
    top: node.getComputedTop(),
    left: node.getComputedLeft(),
    width: node.getComputedWidth(),
    height: node.getComputedHeight(),
    ...parentRelatedMetrics,
    overflow: overflowToString(node.getOverflow()),
  }
}

const updateLayoutNode = (node: LayoutNode, settings: UiRuntimeSettings): void => {
  applyStyle(node, settings)
}

const getNodeId = (node: LayoutNode): number => (
  node.M.O
)

const createLayoutNode = (settings?: UiRuntimeSettings): LayoutNode => {
  const node = Yoga.Node.createDefault() as any
  node.getId = () => node.M.O
  if (settings !== undefined) {
    updateLayoutNode(node, settings)
  }
  return node as LayoutNode
}

export type {UiRuntimeSettings, LayoutMetrics}

export {updateLayoutNode, createLayoutNode, metricsFromLayoutNode, getNodeId}
