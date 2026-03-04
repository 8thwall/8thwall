/**
 * NOTE: because yoga-layout is an ESM module it doesn't work with the current package.json
 * configuration. This test is disabled from CI until we find a solution.
 * To make the test work, you have to change the package.json configuration to use ESM modules by
 * adding the following configuration:
 *
 *   "type": "module",
 */
import {describe, it, beforeEach, afterEach, before} from 'mocha'
import {assert} from 'chai'

import {createLayoutNode, getNodeId} from '../src/runtime/ui'
import type {LayoutNode} from '../src/shared/flex-styles'

describe('yoga-layout test', () => {
  let parent: LayoutNode
  let child1: LayoutNode
  let child2: LayoutNode
  let child3: LayoutNode

  beforeEach(() => {
    parent = createLayoutNode()
    child1 = createLayoutNode()
    child2 = createLayoutNode()
    child3 = createLayoutNode()
    parent.insertChild(child1, 0)
    parent.insertChild(child2, 1)
    parent.insertChild(child3, 2)
  })

  afterEach(() => {
    parent.freeRecursive()
  })

  describe('yoga node has secret Id property', () => {
    it('should have a secret Id property [M.O]', () => {
      const allNodes: any[] = [parent, child1, child2, child3]
      // eslint-disable-next-line no-restricted-syntax
      for (const node of allNodes) {
        assert.isDefined(node.M.O)
      }
    })
  })

  describe('storing yoga nodes reference', () => {
    const orderMap = new Map<LayoutNode, number>()

    before(() => {
      orderMap.set(child1, 0)
      orderMap.set(child2, 1)
      orderMap.set(child3, 2)
    })

    it('should always fail to hit cache', () => {
      for (let i = 0; i < parent.getChildCount(); i++) {
        const child = parent.getChild(i) as LayoutNode
        assert.equal(orderMap.get(child), undefined)
      }
    })
  })

  describe('storing yoga nodes internal Id', () => {
    const orderMap = new Map<number, number>()
    before(() => {
      orderMap.set(getNodeId(child1), 0)
      orderMap.set(getNodeId(child2), 1)
      orderMap.set(getNodeId(child3), 2)
    })

    it('should always hit cache', () => {
      for (let i = 0; i < parent.getChildCount(); i++) {
        const child = parent.getChild(i) as LayoutNode
        assert.equal(orderMap.get(getNodeId(child)), i)
      }
    })

    it('should have unique ids', () => {
      const ids = new Set<number>([getNodeId(parent)])
      for (let i = 0; i < parent.getChildCount(); i++) {
        const child = parent.getChild(i) as LayoutNode
        ids.add(getNodeId(child))
      }
      assert.equal(ids.size, parent.getChildCount() + 1)
    })
  })
})
