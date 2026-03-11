import type {LayoutNode} from '@ecs/shared/flex-styles'
import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject, UiRootType} from '@ecs/shared/scene-graph'
import {
  createLayoutNode, getNodeId, LayoutMetrics, metricsFromLayoutNode, updateLayoutNode,
} from '@ecs/runtime/ui'
import {getUiAttributes} from '@ecs/shared/get-ui-attributes'
import {validateNumber} from '@ecs/shared/validate-number'
import uuid from 'uuid'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {determineUiOrder} from '@ecs/shared/determine-ui-order'

import {useTargetResolution} from './use-target-resolution'
import type {DerivedScene} from './derive-scene'
import {useDerivedScene} from './derived-scene-context'

interface RawYogaParentContext {
  rootIds: Set<string>
  layoutMetrics: Map<string, LayoutMetrics>
  layoutChildren: Map<string, string[]>
  overlayContainers: Map<string, string>  // root id -> overlay container id
  uiOrder: Map<string, number>  // entity id -> ui order
  stackingContext: Map<string, number>  // entity id -> stacking context
}

type YogaParentContext = DeepReadonly<RawYogaParentContext>

const Context = React.createContext<YogaParentContext | null>(null)

const useYogaParentContext = (): YogaParentContext => {
  const ctx = React.useContext(Context)
  if (!ctx) {
    throw new Error('useYogaParentContext must be used within a YogaParentContextProvider')
  }
  return ctx
}

const getSceneLowestParentWithAttribute = (
  derivedScene: DerivedScene, id: string, key: keyof GraphObject
): string | undefined => {
  let parentId = derivedScene.getObject(id)?.parentId
  while (parentId) {
    const obj = derivedScene.getObject(parentId)
    if (!obj) {
      return undefined
    }
    if (obj[key] !== undefined) {
      return parentId
    }
    parentId = obj?.parentId
  }
  return undefined
}

type Dimension = {width: number, height: number}

const sceneGraphYogaLayout = (
  derivedScene: DerivedScene, screenWidth: number, screenHeight: number
): RawYogaParentContext => {
  // traverse scene graph and create layout nodes
  const layoutNodes = new Map<string, LayoutNode>()
  const nodeIdToEntityId = new Map<number, string>()
  const rootIds = new Set<string>()
  const rootDimensions = new Map<string, Dimension>()
  const layoutChildren = new Map<string, string[]>()
  const orderMap = new Map<number, number>()
  const overlayContainers = new Map<string, string>()

  const findNextSiblingIndex = (parent: LayoutNode, order: number): number => {
    const childCount = parent.getChildCount()
    for (let i = 0; i < childCount; i++) {
      const child = parent.getChild(i) as LayoutNode
      if ((orderMap.get(getNodeId(child)) ?? 0) > order) {
        return i
      }
    }
    return childCount
  }

  const insertToParent = (child: LayoutNode, parent: LayoutNode) => {
    const afterChild = findNextSiblingIndex(parent, orderMap.get(getNodeId(child)) ?? 0)
    parent.insertChild(child, afterChild)
  }

  const sceneObjects = derivedScene.getAllSceneObjects()
  // Doing a two pass layout tree creation to ensure that parents are created before children
  sceneObjects.map((object): [string, LayoutNode, Dimension, UiRootType] | null => {
    const {id} = object
    if (object.ui && !object.disabled) {
      const attributes = getUiAttributes(object.ui)
      const node = createLayoutNode()
      const nodeId = getNodeId(node)
      nodeIdToEntityId.set(nodeId, id)
      updateLayoutNode(node, attributes)
      orderMap.set(nodeId, object.order ?? 0)
      layoutNodes.set(id, node)

      // NOTE (jeffha): root nodes should not have a relative (%) width or height
      const width = validateNumber(object.ui.width)
      const height = validateNumber(object.ui.height)

      // TODO (tri) cache root and parent ui ids for certain ids for faster lookup
      return [id, node, {width, height}, object.ui.type || UI_DEFAULTS.type]
    }
    return null
  }).filter(Boolean).forEach(([id, node, dimension, type]) => {
    const parentId = getSceneLowestParentWithAttribute(derivedScene, id, 'ui')
    if (parentId) {
      const parent = layoutNodes.get(parentId)
      if (parent) {
        insertToParent(node, parent)
        layoutChildren.set(parentId, [...(layoutChildren.get(parentId) ?? []), id])
      }
    } else if (type === 'overlay') {
      const containerId = uuid()
      const containerNode = createLayoutNode()
      updateLayoutNode(containerNode, {
        ...UI_DEFAULTS,
        font: UI_DEFAULTS.font.font,
        width: `${screenWidth}`,
        height: `${screenHeight}`,
        backgroundOpacity: 0,
      })
      layoutNodes.set(containerId, containerNode)
      insertToParent(node, containerNode)
      overlayContainers.set(id, containerId)
      rootIds.add(id)
      rootDimensions.set(containerId, {
        width: screenWidth,
        height: screenHeight,
      })
    } else {
      rootIds.add(id)
      rootDimensions.set(id, {width: dimension.width, height: dimension.height})
    }
  })

  // assign ui a depth-first hierarchy-based order
  const uiOrder = new Map<string, number>()
  const stackingContext = new Map<string, number>()
  rootIds.forEach((rootId) => {
    const rootNode = layoutNodes.get(rootId)
    if (rootNode) {
      determineUiOrder({
        rootNode,
        setData: (node, {uiOrder: uo, stackingContext: sc}) => {
          uiOrder.set(nodeIdToEntityId.get(getNodeId(node)), uo)
          stackingContext.set(nodeIdToEntityId.get(getNodeId(node)), sc)
        },
        getStackingOrder: node => derivedScene.getObject(nodeIdToEntityId.get(getNodeId(node)))
          .ui?.stackingOrder ?? 0,
      })
    }
  })

  const rootNodeIds = Array.from(rootIds)
    .map(objectId => overlayContainers.get(objectId) || objectId)

  // update layout nodes metric from scene graph changes
  rootNodeIds.forEach((id) => {
    const node = layoutNodes.get(id)
    if (node) {
      const dimension = rootDimensions.get(id)
      node.calculateLayout(
        dimension?.width ?? 0,
        dimension?.height ?? 0,
        1  // Inherit = 0, LTR = 1, RTL = 2
      )
    }
  })

  const layoutMetrics = new Map<string, LayoutMetrics>()
  for (const [id, node] of layoutNodes.entries()) {
    layoutMetrics.set(id, metricsFromLayoutNode(node))
    node.markLayoutSeen()
  }

  // manually free layout nodes to avoid memory leak
  rootNodeIds.forEach((id) => {
    const node = layoutNodes.get(id)
    node.freeRecursive()
  })

  return {rootIds, layoutMetrics, layoutChildren, overlayContainers, uiOrder, stackingContext}
}

const YogaParentContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  const derivedScene = useDerivedScene()
  const {width, height} = useTargetResolution()

  const state = React.useMemo(() => (
    // update layout nodes from scene graph changes
    sceneGraphYogaLayout(derivedScene, width, height)
  ), [derivedScene, width, height])

  return <Context.Provider value={state}>{children}</Context.Provider>
}

export {
  YogaParentContextProvider,
  useYogaParentContext,
}

export type {
  YogaParentContext,
}
