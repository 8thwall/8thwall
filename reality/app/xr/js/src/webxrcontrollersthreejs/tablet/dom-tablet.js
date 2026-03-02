// @dep(//reality/app/xr/js/src/third_party/html-to-image)
// @inliner-skip-next
import {toImage as drawElementToImage} from '../../third_party/html-to-image/src/index'
import {intersectionToClient} from './coordinates'
import {createDomTabletModel, getBarWidth} from './dom-tablet-model'
import {isPartOf} from '../scene-graph'
import {BASE_TABLET_SCALE} from './constants'

const elementIsRenderable = (el) => {
  if (el instanceof HTMLCanvasElement || el instanceof HTMLIFrameElement) {
    return false
  }

  if (el.tagName && el.tagName.toLowerCase() === 'noscript') {
    return false
  }

  // Ignore A-Frame elements
  if (el.object3D) {
    return false
  }

  return true
}

const shouldDrawToCanvas = (el) => {
  if (!elementIsRenderable(el)) {
    return false
  }
  try {
    // Note that "visibility: hidden" or "opacity: 0" elements can affect layout of other
    // elements, so those still need to be included.
    if (window.getComputedStyle(el).getPropertyValue('display') === 'none') {
      return false
    }
  } catch (err) {
    // Ignore
  }

  return true
}

const shouldIgnoreMutationTarget = (el) => {
  if (!el) {
    // Target not provided, safer to update
    return false
  }

  if (el.closest('.xr-tablet-ignore')) {
    return false
  }

  if (el.classList && el.classList.contains('xr-tablet-ignore')) {
    return false
  }

  return !elementIsRenderable(el)
}

const createDomTablet = ({element, interactionProvider}) => {
  const {THREE} = window

  const planeGeometry = new THREE.PlaneGeometry(1, 1)
  const viewProjection_ = new THREE.Matrix4()
  const frustum_ = new THREE.Frustum()

  const imageMaterial = new THREE.MeshBasicMaterial({
    color: 0xffffff,
    side: THREE.DoubleSide,
    transparent: true,
    opacity: 0.9,
    map: new THREE.Texture(),
  })

  const tabletQuad = new THREE.Mesh(planeGeometry, imageMaterial)
  const tabletGroup = new THREE.Group()

  const handleQuad = new THREE.Mesh(planeGeometry, new THREE.MeshBasicMaterial())
  handleQuad.visible = false
  handleQuad.scale.y = 0.15
  handleQuad.position.y = -0.63
  handleQuad.position.z = -0.05
  tabletGroup.add(handleQuad)

  tabletGroup.scale.setScalar(BASE_TABLET_SCALE)

  const model_ = createDomTabletModel()

  const resourceCache_ = {}  // Cache for resources loaded for mirroring the DOM to canvas

  let resizeObserver_
  let mutationObserver_
  let pendingRefresh_
  let isDrawingCanvas_ = false
  let domWidth = element.offsetWidth
  let domHeight = element.offsetHeight
  let didRefresh_ = false
  let minimized_ = false
  let parent_ = null
  let domVisible_ = false
  let needsDomUpdateOnVisibility_ = false
  const ignoreDomElementHeadTag_ = document.createElement('style')
  ignoreDomElementHeadTag_.appendChild(
    document.createTextNode('.xr-tablet-ignore {display: none !important;}')
  )
  let ignoreDomElementHeadTagInjected_ = false

  model_.onLoad().then((modelObject) => {
    // Order matters here to have the overlapping translucent parts render properly
    tabletGroup.add(modelObject)
    tabletGroup.add(tabletQuad)
  })

  const updateParent = () => {
    // If we're minimized or haven't rendered yet, ensure we're not in the scene
    if (minimized_ || !didRefresh_) {
      if (tabletGroup.parent) {
        tabletGroup.parent.remove(tabletGroup)
      }
      return
    }

    if (tabletGroup.parent === parent_) {
      return
    }

    if (tabletGroup.parent) {
      tabletGroup.parent.remove(tabletGroup)
    }
    if (parent_) {
      parent_.add(tabletGroup)
    }
  }

  const refreshCanvas = () => {
    if (isDrawingCanvas_) {
      return
    }

    isDrawingCanvas_ = true

    drawElementToImage(element, {
      filter: shouldDrawToCanvas,
      style: {background: 'transparent'},
      suppressErrors: didRefresh_,
      resourceCache: resourceCache_,
      width: window.innerWidth,
      height: window.innerHeight,
      preferredFontFormat: ['woff2', 'woff', 'truetype', 'svg'],
      // Image place holder is required otherwise we'll get unhandled promise rejection.
      // TODO(pawel) Better place holder, this just allows things to work.
      imagePlaceholder: `image/svg+xml,${btoa('<svg width="100" height="100" xmlns="http://www.w3.org/2000/svg"></svg>')}`,
    }).then((img) => {
      didRefresh_ = true
      tabletQuad.material.map.image = img
      tabletQuad.material.map.needsUpdate = true

      ;({naturalWidth: domWidth, naturalHeight: domHeight} = img)
      const width = domWidth / domHeight
      tabletQuad.scale.set(width, 1, 1)
      handleQuad.scale.x = getBarWidth(width) / 2 + 0.05
      model_.setWidth(domWidth / domHeight)
      updateParent()
      isDrawingCanvas_ = false
    })
  }

  const queueRefresh = () => {
    if (pendingRefresh_) {
      return
    }
    if (!domVisible_) {
      needsDomUpdateOnVisibility_ = true
      return
    }
    pendingRefresh_ = setTimeout(() => {
      pendingRefresh_ = null
      refreshCanvas()
    }, 750)
  }

  const handleDomChange = () => {
    queueRefresh()
  }

  const isTablet = object => isPartOf(object, tabletGroup)

  const isMinimizeButton = object => model_.isMinimizeButton(object)
  const isExitButton = object => model_.isExitButton(object)
  const isHandle = object => isPartOf(object, handleQuad) || model_.isHandle(object)

  const onControllerStart = (intersection) => {
    const position = intersectionToClient(intersection, tabletQuad, domWidth, domHeight, element)
    return interactionProvider.startPointer(position)
  }

  const onControllerMove = (pointer, intersection) => {
    const position = intersectionToClient(intersection, tabletQuad, domWidth, domHeight, element)
    interactionProvider.updatePointerPosition(pointer, position)
  }

  const onControllerEnd = (pointer, intersection) => {
    const position = intersectionToClient(intersection, tabletQuad, domWidth, domHeight, element)
    interactionProvider.updatePointerPosition(pointer, position)
    interactionProvider.endPointer(pointer)
  }

  const attach = () => {
    refreshCanvas()
    if (!ignoreDomElementHeadTagInjected_) {
      document.head.appendChild(ignoreDomElementHeadTag_)
      ignoreDomElementHeadTagInjected_ = true
    }
    if (window.ResizeObserver) {
      resizeObserver_ = new window.ResizeObserver(handleDomChange)
      resizeObserver_.observe(element)
    } else {
      window.addEventListener('resize', handleDomChange)
    }

    if (window.MutationObserver) {
      mutationObserver_ = new window.MutationObserver((events) => {
        if (events.every(e => shouldIgnoreMutationTarget(e.target))) {
          return
        }
        handleDomChange()
      })
      mutationObserver_.observe(element, {
        childList: true,
        attributes: true,
        subtree: true,
      })
    }
  }

  const detach = () => {
    clearTimeout(pendingRefresh_)
    pendingRefresh_ = null

    if (ignoreDomElementHeadTagInjected_) {
      ignoreDomElementHeadTag_.remove()
      ignoreDomElementHeadTagInjected_ = false
    }
    if (resizeObserver_) {
      resizeObserver_.unobserve(element)
      resizeObserver_ = null
    }

    window.removeEventListener('resize', handleDomChange)

    if (mutationObserver_) {
      mutationObserver_.disconnect()
      mutationObserver_ = null
    }

    isDrawingCanvas_ = false
    minimized_ = false
    parent_ = null
    domVisible_ = false
    needsDomUpdateOnVisibility_ = false
    updateParent()
  }

  // Check recursively for object visibility. An object is visible if every object in its hierarchy
  // is visible.
  const isVisible = (obj) => {
    if (!obj.visible) {
      return false
    }
    return obj.parent ? isVisible(obj.parent) : true
  }

  const update = (camera) => {
    domVisible_ = !minimized_ && isVisible(tabletQuad)
    if (domVisible_) {
      viewProjection_.multiplyMatrices(camera.projectionMatrix, camera.matrixWorldInverse)
      frustum_.setFromProjectionMatrix(viewProjection_)
      domVisible_ = frustum_.intersectsObject(tabletQuad)
    }
    if (domVisible_ && needsDomUpdateOnVisibility_) {
      queueRefresh()
      needsDomUpdateOnVisibility_ = false
    }
  }
  const setMinimized = (minimized) => {
    minimized_ = minimized
    updateParent()
  }

  const setParent = (parent) => {
    parent_ = parent
    updateParent()
  }

  // Immediately refresh the canvas once.
  refreshCanvas()

  return {
    getObject: () => tabletGroup,
    onControllerStart,
    onControllerMove,
    onControllerEnd,
    isTablet,
    isMinimizeButton,
    isExitButton,
    isHandle,
    attach,
    detach,
    update,
    setMinimized,
    setParent,
  }
}

export {
  createDomTablet,
}
