import type {World} from '../world'
import type {Eid, ReadData} from '../../shared/schema'
import {Hidden, ThreeObject, Ui} from '../components'
import {makeSystemHelper} from './system-helper'
import {
  createLayoutNode, getNodeId, LayoutMetrics, metricsFromLayoutNode, updateLayoutNode,
} from '../ui'
import type {LayoutNode} from '../../shared/flex-styles'
import {create3dUi, remove3dUi, EidToLoadingPromise, update3dUi, setUiUserData} from '../3d-ui'
import ThreeMeshUI from '../8mesh'
import {input, ScreenTouchEndEvent, ScreenTouchStartEvent} from '../input'
import type * as THREE_TYPES from '../three-types'
import {findParentComponent} from '../../shared/find-component'
import {findRootComponent} from '../../shared/find-root-component'
import {validateNumber} from '../../shared/validate-number'
import {Direction} from '../../shared/flex-enum'
import {addChild, notifyChanged} from '../matrix-refresh'
import {UI_CLICK, UI_PRESSED, UI_RELEASED} from '../ui-events'
import type {QueuedEvent} from '../events-types'
import {UI_DEFAULTS} from '../../shared/ui-constants'
import THREE from '../three'
import type {SchemaOf} from '../world-attribute'
import {THREE_LAYERS} from '../../shared/three-layers'
import {getReversePainterSortWithUi} from '../../shared/custom-sorting'
import {determineUiOrder} from '../../shared/determine-ui-order'
import {Disabled} from '../disabled'

type UiType = 'overlay' | '3d'

type UiNode = {
  eid: Eid
  node: LayoutNode
  metrics?: LayoutMetrics
  object: ThreeMeshUI.Block
  isImage: boolean
  isVideo: boolean
  currentType: UiType
  removeListeners: () => void
  order: number
  stackingOrder: number
}

type ResolveDimension = (n?: number) => number
type RootNode = {
  rootNode: LayoutNode
  resolveWidth: ResolveDimension
  resolveHeight: ResolveDimension
}

const ROOT_NODE_ID = 0n

const findLowestCommonParent = (world: World, eid1: Eid, eid2: Eid) => {
  const visited = new Set<Eid>()
  let cursor = eid1
  while (cursor) {
    if (!Ui.has(world, cursor)) {
      break
    }
    visited.add(cursor)
    cursor = world.getParent(cursor)
  }

  cursor = eid2
  while (cursor) {
    if (!Ui.has(world, cursor)) {
      return undefined
    }
    if (visited.has(cursor)) {
      return cursor
    }
    cursor = world.getParent(cursor)
  }

  return undefined
}

const isEntityHidden = (world: World, eid: Eid) => {
  let cursor = eid
  while (cursor) {
    if (Hidden.has(world, cursor)) {
      return true
    }
    cursor = world.getParent(cursor)
  }
  return false
}

const getAllPressedEids = (world: World, eid: Eid, target: Eid[]) => {
  let cursor = eid
  while (cursor && Ui.has(world, cursor)) {
    target.push(cursor)
    cursor = world.getParent(cursor)
  }
}

const makeUiSystem = (world: World, registerPostRender: (cb: () => void) => void) => {
  const {enter, changed, exit} = makeSystemHelper(Ui)
  const nodes: Map<Eid, UiNode> = new Map()
  const nodeIdToEntityId: Map<number, Eid> = new Map()
  const eidToNodeNeedUpdate: Map<Eid, UiNode> = new Map()
  const parentAddingQueue: Map<Eid, Eid[]> = new Map()
  const eidToLoadingPromise: EidToLoadingPromise = new Map()
  const needsLayout: Set<Eid> = new Set()
  const needsReorder: Set<Eid> = new Set()
  // NOTE(johnny): This list will prevent multiple pressed events from being emited when the
  // touch event is bubbling up the tree.
  const capturedEids: Eid[] = []
  let pressedEid: Eid | undefined

  const getUiNodeFromNode = (node: LayoutNode) => nodes
    .get(nodeIdToEntityId.get(getNodeId(node)) ?? ROOT_NODE_ID)

  const rootSettings: ReadData<SchemaOf<typeof Ui>> = {
    ...UI_DEFAULTS,
    font: UI_DEFAULTS.font.font,
    type: 'overlay',
  }

  let windowWidth: number
  let windowHeight: number

  const rootNodes: Map<Eid, RootNode> = new Map()

  const orthographicRootEntities: Set<Eid> = new Set()
  const orthographicRootNode = createLayoutNode(rootSettings)
  const orthographicCamera = new THREE.OrthographicCamera(
    window.innerWidth / -2,
    window.innerWidth / 2,
    window.innerHeight / 2,
    window.innerHeight / -2,
    1,
    200
  )
  orthographicCamera.layers.enable(THREE_LAYERS.renderedNotRaycasted)

  const orthographicScene = new THREE.Scene()
  orthographicCamera.position.set(0, 0, 100)
  orthographicCamera.lookAt(0, 0, 0)
  orthographicScene.add(orthographicCamera)

  world.insertRaycastStage({
    getCamera: () => orthographicCamera,
    scene: orthographicScene,
    includeWorldPosition: false,
  }, 0)

  const mainSceneSort = getReversePainterSortWithUi(world.three.renderer, world.three.scene)
  const orthographicSceneSort = getReversePainterSortWithUi(world.three.renderer, orthographicScene)
  world.three.renderer.setTransparentSort(mainSceneSort)
  registerPostRender(() => {
    if (orthographicRootEntities.size === 0) {
      return
    }
    world.three.renderer.setTransparentSort(orthographicSceneSort)
    world.three.renderer.autoClearColor = false
    world.three.renderer.clearDepth()
    world.three.renderer.render(orthographicScene, orthographicCamera)
    world.three.renderer.setTransparentSort(mainSceneSort)
  })

  const orthographicRootObject: ThreeMeshUI.Block = create3dUi(
    world, ROOT_NODE_ID, eidToLoadingPromise, rootSettings, false
  )
  orthographicRootObject.raycast = () => {}
  nodes.set(ROOT_NODE_ID, {
    eid: ROOT_NODE_ID,
    node: orthographicRootNode,
    object: orthographicRootObject as THREE_TYPES.Object3D,
    removeListeners: () => {},
    isImage: false,
    isVideo: false,
    currentType: 'overlay',
    order: 0,
    stackingOrder: 0,
  })
  nodeIdToEntityId.set(getNodeId(orthographicRootNode), ROOT_NODE_ID)

  addChild(orthographicScene, orthographicRootObject)
  orthographicRootObject.scale.set(100, 100, 1)
  needsLayout.add(ROOT_NODE_ID)

  rootNodes.set(ROOT_NODE_ID, {
    rootNode: orthographicRootNode,
    resolveWidth: () => windowWidth,
    resolveHeight: () => windowHeight,
  })

  const applyLayoutNode = (eid: Eid, node: UiNode): void => {
    if (!node.node.hasNewLayout()) {
      return
    }
    const ui = Ui.has(world, eid) ? Ui.get(world, eid) : rootSettings
    const isHidden = isEntityHidden(world, eid)
    const metrics = metricsFromLayoutNode(node.node)
    nodes.set(eid, {...node, metrics})
    update3dUi(world, eid, eidToLoadingPromise, node.object!, ui, isHidden, metrics)
    notifyChanged(node.object!)
    node.node.markLayoutSeen()
  }

  const addChildrenToMap = (eid: Eid) => {
    const children = eid === ROOT_NODE_ID
      ? orthographicRootEntities
      : world.getChildren(eid)

    for (const child of children) {
      if (nodes.has(child)) {
        eidToNodeNeedUpdate.set(child, nodes.get(child)!)
      }
      addChildrenToMap(child)
    }
  }

  const updateTreeLayout = () => {
    eidToNodeNeedUpdate.clear()

    needsLayout.forEach((eid) => {
      const node = nodes.get(eid)
      if (!node) {
        return
      }
      if (!eidToNodeNeedUpdate.has(eid)) {
        eidToNodeNeedUpdate.set(eid, node)
      }
      if (rootNodes.has(eid)) {
        const {rootNode, resolveWidth, resolveHeight} = rootNodes.get(eid)!
        rootNode.calculateLayout(
          resolveWidth(),
          resolveHeight(),
          Direction.LTR
        )
        addChildrenToMap(eid)
      }
    })

    eidToNodeNeedUpdate.forEach((node, eid) => {
      applyLayoutNode(eid, node)
    })

    needsLayout.clear()
  }

  const updateTreeOrder = () => {
    needsReorder.forEach((eid) => {
      const rootNode = nodes.get(eid)
      if (rootNode && rootNodes.has(eid)) {
        determineUiOrder({
          rootNode: rootNode.node,
          getStackingOrder: node => getUiNodeFromNode(node)?.stackingOrder ?? 0,
          setData: (node, data) => {
            const uiNode = getUiNodeFromNode(node)
            if (uiNode) {
              setUiUserData(uiNode.object, data)
            }
          },
        })
      }
    })

    needsReorder.clear()
  }

  const makeHandleUiPressed = (eid: Eid) => (e: QueuedEvent) => {
    const data = e.data as ScreenTouchStartEvent
    const {x, y} = data.position
    if (pressedEid !== eid && !capturedEids.includes(eid)) {
      world.events.dispatch(eid, UI_PRESSED, {x, y})
      pressedEid = eid
      getAllPressedEids(world, eid, capturedEids)
    }
  }

  const makeHandleUiReleased = (e: QueuedEvent) => {
    const data = e.data as ScreenTouchEndEvent
    const {x, y} = data.position
    const {target, endTarget} = data
    if (endTarget && target && target === pressedEid) {
      const lowestCommonParent = findLowestCommonParent(world, target, endTarget)
      if (lowestCommonParent) {
        world.events.dispatch(lowestCommonParent, UI_CLICK, {x, y})
      }
    }
    if (pressedEid && target === pressedEid) {
      world.events.dispatch(target, UI_RELEASED, {x, y})
      pressedEid = undefined
      capturedEids.length = 0
    }
  }

  const handleUiRemove = (eid: Eid) => {
    if (!nodes.has(eid)) {
      return
    }

    const {node, removeListeners} = nodes.get(eid)!
    removeListeners()
    const entityObject = world.three.entityToObject.get(eid)
    if (entityObject) {
      entityObject.userData.ui3d = null
    }

    const object = nodes.get(eid)!.object!
    const rootId = object.userData.rootUi?.userData.eid
    if (nodes.has(rootId)) {
      needsLayout.add(rootId)
    }
    remove3dUi(eid, eidToLoadingPromise, object)
    rootNodes.delete(eid)
    node.getParent()?.removeChild(node)
    node.free()
    nodes.delete(eid)
    orthographicRootEntities.delete(eid)
  }

  const handleUiDeepRemove = (eid: Eid): void => {
    for (const child of world.getChildren(eid)) {
      handleUiDeepRemove(child)
    }
    if (Ui.has(world, eid)) {
      handleUiRemove(eid)
    }
  }

  const findNextSiblingIndex = (parent: LayoutNode, order: number): number => {
    const childCount = parent.getChildCount()
    for (let i = 0; i < childCount; i++) {
      const child = parent.getChild(i) as LayoutNode
      if ((getUiNodeFromNode(child)?.order ?? 0) > order) {
        return i
      }
    }
    return childCount
  }

  const insertToParent = (child: LayoutNode, parent: LayoutNode) => {
    const afterChild = findNextSiblingIndex(parent, getUiNodeFromNode(child)?.order ?? 0)
    parent.insertChild(child, afterChild)
  }

  const handleCreateUi = (eid: Eid) => {
    const rootId = findRootComponent(world, eid, Ui)
    const overlay = Ui.get(world, rootId).type === 'overlay'
    // Create layout node
    const entityObject = world.three.entityToObject.get(eid)
    if (!entityObject) {
      // eslint-disable-next-line no-console
      console.error('entity has no object which should not happen')
    }
    const ui = Ui.get(world, eid)
    const displayNone = ui.display === 'none'
    const isHidden = isEntityHidden(world, eid)
    const {order} = ThreeObject.get(world, eid)
    const object = create3dUi(world, eid, eidToLoadingPromise, ui, isHidden) as THREE_TYPES.Object3D
    if (entityObject && !displayNone) {
      addChild(entityObject, object)
    }
    if (entityObject) {
      entityObject.userData.ui3d = object
    }
    const node = createLayoutNode(ui)
    const listenerPressed = makeHandleUiPressed(eid)
    const listenerReleased = makeHandleUiReleased

    world.events.addListener(eid, input.SCREEN_TOUCH_START, listenerPressed)
    world.events.addListener(eid, input.SCREEN_TOUCH_END, listenerReleased)

    nodeIdToEntityId.set(getNodeId(node), eid)
    nodes.set(eid, {
      eid,
      node,
      object,
      removeListeners: () => {
        world.events.removeListener(eid, input.SCREEN_TOUCH_START, listenerPressed)
        world.events.removeListener(eid, input.SCREEN_TOUCH_END, listenerReleased)
      },
      isImage: !!ui.image,
      isVideo: !!ui.video,
      currentType: overlay ? 'overlay' : '3d',
      order,
      stackingOrder: ui.stackingOrder,
    })

    if (parentAddingQueue.has(eid)) {
      // add this ui's children in the queue to self
      const children = parentAddingQueue.get(eid) ?? []
      for (const childId of children) {
        const {node: childNode, object: childObj} = nodes.get(childId) || {}
        if (childNode) {
          insertToParent(childNode, node)
          addChild(object, childObj!)
        }
      }
      parentAddingQueue.delete(eid)
    }

    const parentId = findParentComponent(world, eid, Ui)
    const parentUiNode = (!overlay && !parentId)
      ? undefined
      : nodes.get(parentId)
    if (parentId === ROOT_NODE_ID && overlay) {
      orthographicRootEntities.add(eid)
    }
    const {node: parentNode, object: parentObj} = parentUiNode ?? {}
    if (parentId && !parentNode) {
      // Parent ui hasn't been created yet. Queue this ui to be added later
      parentAddingQueue.set(parentId, [...(parentAddingQueue.get(parentId) ?? []), eid])
    } else if (parentNode) {
      insertToParent(node, parentNode)
      if (!displayNone) {
        addChild(parentObj!, object)
      }
    } else {
      // This is a root Ui add to the list of root nodes
      rootNodes.set(eid, {
        rootNode: node,
        // NOTE (jeffha): Need to get the most up-to-date UI cursor, otherwise
        //                the cursor may be stale and point to another element
        resolveHeight: () => validateNumber(Ui.get(world, eid).height) / 100,
        resolveWidth: () => validateNumber(Ui.get(world, eid).width) / 100,
      })
    }
    needsReorder.add(overlay ? ROOT_NODE_ID : rootId)
  }

  const handleUiDeepCreate = (eid: Eid): void => {
    if (Disabled.has(world, eid)) {
      return
    }

    if (Ui.has(world, eid)) {
      handleCreateUi(eid)
    }
    for (const child of world.getChildren(eid)) {
      handleUiDeepCreate(child)
    }
  }

  const handleUiApply = (eid: Eid) => {
    const {order} = ThreeObject.get(world, eid)
    let root = findRootComponent(world, eid, Ui)
    let rootOptions = Ui.get(world, root)
    if (rootOptions.type === 'overlay') {
      root = ROOT_NODE_ID
      rootOptions = rootSettings
    }
    const currentType = rootOptions.type
    const ui = Ui.get(world, eid)

    // Ui component is not created yet, create it
    if (!nodes.has(eid)) {
      handleCreateUi(eid)
      needsLayout.add(eid)
      needsLayout.add(root)
      return
    }

    // Ui component is created, update it
    const uiNode = nodes.get(eid)!
    const {
      node, isImage: wasImage, isVideo: wasVideo, currentType: wasType, object, order: wasOrder,
      stackingOrder: wasStackingOrder,
    } = uiNode
    const isImage = !!ui.image
    const isVideo = !!ui.video
    const assetChange = wasImage !== isImage || wasVideo !== isVideo
    const orderChange = wasOrder !== order
    const stackingChange = wasStackingOrder !== ui.stackingOrder

    const typeChange = currentType !== wasType

    if (typeChange || orderChange) {
      // type of ui changed, delete it and all its children and recreate it
      handleUiDeepRemove(eid)
      handleUiDeepCreate(eid)
      needsLayout.add(eid)
      needsLayout.add(root)
    } else if (assetChange) {
      // type of ui asset changed, recreate it
      handleUiRemove(eid)
      handleUiApply(eid)
    } else {
      const isHidden = isEntityHidden(world, eid)
      update3dUi(world, eid, eidToLoadingPromise, nodes.get(eid)!.object!, ui, isHidden)
      if (rootNodes.has(eid)) {
        rootNodes.set(eid, {
          rootNode: rootNodes.get(eid)!.rootNode,
          resolveHeight: () => validateNumber(Ui.get(world, eid).height) / 100,
          resolveWidth: () => validateNumber(Ui.get(world, eid).width) / 100,
        })
      }
      updateLayoutNode(node, ui)
      needsLayout.add(eid)
      needsLayout.add(root)
      if (ui.display === 'none') {
        if (object && object.parent) {
          object.removeFromParent()
        }
      } else if (object && !object.parent) {
        const parentId = findParentComponent(world, eid, Ui)
        const parentNode = rootNodes.has(eid) ? undefined : nodes.get(parentId)
        const parentObj = parentNode?.object ?? world.three.entityToObject.get(eid)
        if (object && parentObj && object.parent !== parentObj) {
          addChild(parentObj, object)
        }
      }
    }

    if (stackingChange) {
      uiNode.stackingOrder = ui.stackingOrder
      needsReorder.add(root)
    }
  }

  return () => {
    const newWindowWidth: number = window.innerWidth
    const newWindowHeight: number = window.innerHeight

    if (newWindowWidth !== windowWidth || newWindowHeight !== windowHeight) {
      windowWidth = newWindowWidth
      windowHeight = newWindowHeight

      orthographicRootNode.setWidth(windowWidth)
      orthographicRootNode.setHeight(windowHeight)

      for (const {rootNode, resolveWidth, resolveHeight} of rootNodes.values()) {
        rootNode.calculateLayout(
          resolveWidth(),
          resolveHeight(),
          1  // Inherit = 0, LTR = 1, RTL = 2
        )
      }
      for (const [eid, node] of nodes.entries()) {
        applyLayoutNode(eid, node)
      }

      orthographicCamera.left = windowWidth / -2
      orthographicCamera.right = windowWidth / 2
      orthographicCamera.top = windowHeight / 2
      orthographicCamera.bottom = windowHeight / -2
      orthographicCamera.updateProjectionMatrix()
    }
    exit(world).forEach(handleUiRemove)
    enter(world).forEach(handleUiApply)
    changed(world).forEach(handleUiApply)
    if (needsLayout.size > 0) {
      updateTreeLayout()
    }
    if (needsReorder.size > 0) {
      updateTreeOrder()
    }

    ThreeMeshUI.update()
    if (parentAddingQueue.size > 0) {
      throw new Error(
        'potentially there are still ui elements that have not been added to the scene'
      )
    }
  }
}

export {
  makeUiSystem,
}
