// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)
import {describe, it, assert} from '@nia/bzl/js/chai-js'

import type {LayoutNode} from './flex-styles'
import {determineUiOrder} from './determine-ui-order'

const createNode = (parent?: LayoutNode, stacking?: number) => {
  const children: LayoutNode[] = []
  const node = {
    M: {O: Math.random()},
    getChildCount: () => children.length,
    getChild: (idx: number) => children[idx],
    insertChild: (child: LayoutNode, idx: number) => {
      children.splice(idx, 0, child)
    },
    getId: () => node.M.O,
  } as any as LayoutNode

  if (parent) {
    parent.insertChild(node, parent.getChildCount())
  }

  return {...node, stackingOrder: stacking ?? 0}
}

const getNodeId = (node: LayoutNode): number => node.M.O

describe('determineUiOrder', () => {
  it('should apply the correct hierarchy order and stacking context', () => {
    /*
    *                  G
    *       /          |          \
    *      H           I           C(-2)
    *      |         /  \         /  \
    *      N(3)     J    K       D    A(-1)
    *     /  \      |    |      / \   |
    *    O(1) M(-1) L(1) P(3)  E   F  B
    */
    const g = createNode()

    const h = createNode(g)
    const i = createNode(g)
    const c = createNode(g, -2)

    const n = createNode(h, 3)
    const j = createNode(i)
    const k = createNode(i)
    const d = createNode(c)
    const a = createNode(c, -1)

    const o = createNode(n, 1)
    const m = createNode(n, -1)
    const l = createNode(j, 1)
    const p = createNode(j, 3)
    const e = createNode(d)
    const f = createNode(d)
    const b = createNode(a)

    const nodes = [g, h, i, c, n, j, k, d, a, o, m, l, p, e, f, b]
    const nodeIdToStackingOrder = Object.fromEntries(
      nodes.map(node => ([getNodeId(node), node.stackingOrder]))
    )
    const nodeIdToUiOrder = new Map<number, number>()
    const nodeIdToContext = new Map<number, number>()

    determineUiOrder({
      rootNode: g,
      getStackingOrder: node => nodeIdToStackingOrder[getNodeId(node)] ?? 0,
      setData: (node, {uiOrder, stackingContext}) => {
        nodeIdToUiOrder.set(getNodeId(node), uiOrder)
        nodeIdToContext.set(getNodeId(node), stackingContext)
      },
    })

    const assertStackingContext = (name: string, node: LayoutNode, expectedContext: number) => {
      const nodeId = getNodeId(node)
      const context = nodeIdToContext.get(nodeId)!
      assert.approximately(
        context,
        expectedContext,
        1e-10,
        `Node ${name} should have stacking context ${expectedContext}, got ${context}`
      )
    }

    assertStackingContext('G', g, 0)
    // Under G is "C, (G), L, N, P"
    assertStackingContext('C', c, -1 / 2)
    assertStackingContext('L', l, 1 / 6)
    assertStackingContext('N', n, 3 / 6)
    assertStackingContext('P', p, 5 / 6)
    // Under C is "A, (C)"
    assertStackingContext('A', a, -3 / 4)
    // Under N is "M, (N), O"
    assertStackingContext('M', m, 5 / 12)
    assertStackingContext('O', o, 7 / 12)

    const assertStackingContextEquals = (name: string, node: LayoutNode, other: LayoutNode) => {
      const nodeId = getNodeId(node)
      const otherId = getNodeId(other)
      const context = nodeIdToContext.get(nodeId)!
      const otherContext = nodeIdToContext.get(otherId)!
      assert.strictEqual(
        context,
        otherContext,
        `${name} should have the same context as its parent`
      )
    }

    // Inherited from G
    assertStackingContextEquals('H', h, g)
    assertStackingContextEquals('I', i, g)
    assertStackingContextEquals('J', j, g)
    assertStackingContextEquals('K', k, g)

    // Inherited from C
    assertStackingContextEquals('D', d, c)
    assertStackingContextEquals('E', e, c)
    assertStackingContextEquals('F', f, c)

    // Inherited from A
    assertStackingContextEquals('B', b, a)

    const resultingSequence = nodes.sort((nodeA, nodeB) => {
      const aContext = nodeIdToContext.get(getNodeId(nodeA)) ?? 0
      const bContext = nodeIdToContext.get(getNodeId(nodeB)) ?? 0
      if (aContext !== bContext) {
        return aContext - bContext
      }
      const aOrder = nodeIdToUiOrder.get(getNodeId(nodeA)) ?? 0
      const bOrder = nodeIdToUiOrder.get(getNodeId(nodeB)) ?? 0
      return aOrder - bOrder
    })

    const expectedSequence = [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p]

    assert.deepEqual(resultingSequence, expectedSequence)
  })
})
