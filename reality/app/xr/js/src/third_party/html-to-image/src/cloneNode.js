import * as getBlobFromURL_1 from './getBlobFromURL'
import * as clonePseudoElements_1 from './clonePseudoElements'
import * as util_1 from './util'

function cloneCanvasElement(node) {
  const dataURL = node.toDataURL()
  if (dataURL === 'data:,') {
    return Promise.resolve(node.cloneNode(false))
  }
  return util_1.createImage(dataURL)
}
function cloneVideoElement(node, options) {
  return Promise.resolve(node.poster)
    .then(url => getBlobFromURL_1.getBlobFromURL(url, options))
    .then(data => util_1.makeDataUrl(data.blob, util_1.getMimeType(node.poster) || data.contentType))
    .then(dataURL => util_1.createImage(dataURL))
}
function cloneImageElement(node, options) {
  return Promise.resolve(node.src)
    .then(url => getBlobFromURL_1.getBlobFromURL(url, options))
    .then(data => util_1.makeDataUrl(data.blob, data.contentType || util_1.getMimeType(node.src)))
    .then(dataURL => util_1.createImage(dataURL))
}

function cloneSingleNode(node, options) {
  if (node instanceof HTMLCanvasElement) {
    return cloneCanvasElement(node)
  }
  if (node instanceof HTMLVideoElement && node.poster) {
    return cloneVideoElement(node, options)
  }
  if (node instanceof HTMLImageElement && node.src) {
    return cloneImageElement(node, options)
  }
  return Promise.resolve(node.cloneNode(false))
}
const isSlotElement = node => node.tagName != null && node.tagName.toUpperCase() === 'SLOT'
function cloneChildren(nativeNode, clonedNode, options) {
  let _a
  const children = isSlotElement(nativeNode) && nativeNode.assignedNodes
    ? util_1.toArray(nativeNode.assignedNodes())
    : util_1.toArray(((_a = nativeNode.shadowRoot) !== null && _a !== void 0 ? _a : nativeNode).childNodes)
  if (children.length === 0 || nativeNode instanceof HTMLVideoElement) {
    return Promise.resolve(clonedNode)
  }
  return children
    .reduce((deferred, child) => deferred
    // eslint-disable-next-line no-use-before-define
      .then(() => cloneNode(child, options))
      .then((clonedChild) => {
        // eslint-disable-next-line promise/always-return
        if (clonedChild) {
          clonedNode.appendChild(clonedChild)
        }
      }), Promise.resolve())
    .then(() => clonedNode)
}
function cloneCSSStyle(nativeNode, clonedNode) {
  const source = window.getComputedStyle(nativeNode)
  const target = clonedNode.style
  if (!target) {
    return
  }
  if (source.cssText) {
    target.cssText = source.cssText
  } else {
    util_1.toArray(source).forEach((name) => {
      target.setProperty(name, source.getPropertyValue(name), source.getPropertyPriority(name))
    })
  }
}
function cloneInputValue(nativeNode, clonedNode) {
  if (nativeNode instanceof HTMLTextAreaElement) {
    clonedNode.innerHTML = nativeNode.value
  }
  if (nativeNode instanceof HTMLInputElement) {
    clonedNode.setAttribute('value', nativeNode.value)
  }
}
function decorate(nativeNode, clonedNode) {
  if (!(clonedNode instanceof Element)) {
    return Promise.resolve(clonedNode)
  }
  return Promise.resolve()
    .then(() => cloneCSSStyle(nativeNode, clonedNode))
    .then(() => clonePseudoElements_1.clonePseudoElements(nativeNode, clonedNode))
    .then(() => cloneInputValue(nativeNode, clonedNode))
    .then(() => clonedNode)
}
function cloneNode(node, options, isRoot) {
  if (!isRoot && options.filter && !options.filter(node)) {
    return Promise.resolve(null)
  }
  return Promise.resolve(node)
    .then(clonedNode => cloneSingleNode(clonedNode, options))
    .then(clonedNode => cloneChildren(node, clonedNode, options))
    .then(clonedNode => decorate(node, clonedNode))
}

export {
  cloneNode,
}
