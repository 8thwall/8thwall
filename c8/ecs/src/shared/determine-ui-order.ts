import type {LayoutNode} from './flex-styles'

const forEachChild = (parent: LayoutNode, callback: (child: LayoutNode) => void) => {
  const childCount = parent.getChildCount() || 0
  for (let i = 0; i < childCount; i++) {
    const childNode = parent.getChild(i) as LayoutNode
    if (childNode) {
      callback(childNode)
    }
  }
}

type OrderingData = {
  uiOrder: number
  stackingContext: number
}

type Context = {
  order: number
  negatives: LayoutNode[]
  positives: LayoutNode[]
}

interface DetermineUiOrderParams {
  rootNode: LayoutNode
  getStackingOrder: (node: LayoutNode) => number
  setData: (node: LayoutNode, data: OrderingData) => void
}

// a stacking context is defined by a UI that has a stackingOrder
// assigns every stacking context a new number that is globally comparable
// by dividing the range into parts for each child and recursing
// assigns the hierarchy order of elements along the way
const determineUiOrder = ({
  rootNode, getStackingOrder, setData,
}: DetermineUiOrderParams) => {
  const stackingComparator = (a: LayoutNode, b: LayoutNode) => {
    const aOrder = getStackingOrder(a)
    const bOrder = getStackingOrder(b)
    return aOrder - bOrder
  }

  let currentOrder = 0

  const inheritOrRegisterWithContext = (context: Context, node: LayoutNode) => {
    const stackingOrder = getStackingOrder(node)
    if (stackingOrder === 0) {
      setData(node, {uiOrder: currentOrder++, stackingContext: context.order})
      forEachChild(node, child => inheritOrRegisterWithContext(context, child))
    } else if (stackingOrder < 0) {
      context.negatives.push(node)
    } else {
      context.positives.push(node)
    }
  }

  const allocateStackingContexts = (node: LayoutNode, start: number, end: number) => {
    const context: Context = {
      order: (start + end) / 2,
      positives: [],
      negatives: [],
    }

    setData(node, {uiOrder: currentOrder++, stackingContext: context.order})
    forEachChild(node, child => inheritOrRegisterWithContext(context, child))

    const divvyRange = (from: number, to: number, nodes: LayoutNode[]) => {
      if (nodes.length === 0) {
        return
      }
      nodes.sort(stackingComparator)
      const step = (to - from) / nodes.length
      nodes.forEach((childUiNode, index) => {
        const childStart = from + step * index
        const childEnd = childStart + step
        allocateStackingContexts(childUiNode, childStart, childEnd)
      })
    }

    divvyRange(start, context.order, context.negatives)
    divvyRange(context.order, end, context.positives)
  }

  allocateStackingContexts(rootNode, -1, 1)
}

export {
  determineUiOrder,
}
