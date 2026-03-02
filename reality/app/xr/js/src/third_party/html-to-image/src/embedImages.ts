import * as getBlobFromURL_1 from './getBlobFromURL'
import * as embedResources_1 from './embedResources'
import * as util_1 from './util'

function embedBackground(clonedNode, options) {
  let _a
  const background = (_a = clonedNode.style) === null || _a === void 0 ? void 0 : _a.getPropertyValue('background')
  if (!background) {
    return Promise.resolve(clonedNode)
  }
  return Promise.resolve(background)
    .then(cssString => embedResources_1.embedResources(cssString, null, options))
    .then((cssString) => {
      clonedNode.style.setProperty('background', cssString, clonedNode.style.getPropertyPriority('background'))
      return clonedNode
    })
}
function embedImageNode(clonedNode, options) {
  if (!(clonedNode instanceof HTMLImageElement && !util_1.isDataUrl(clonedNode.src)) &&
        !(clonedNode instanceof SVGImageElement &&
            !util_1.isDataUrl(clonedNode.href.baseVal))) {
    return Promise.resolve(clonedNode)
  }
  const src = clonedNode instanceof HTMLImageElement
    ? clonedNode.src
    : clonedNode.href.baseVal
  return Promise.resolve(src)
    .then(url => getBlobFromURL_1.getBlobFromURL(url, options))
    .then(data => util_1.makeDataUrl(data.blob, util_1.getMimeType(src) || data.contentType))
    .then(dataURL => new Promise((resolve, reject) => {
      clonedNode.onload = resolve
      clonedNode.onerror = reject
      if (clonedNode instanceof HTMLImageElement) {
        clonedNode.srcset = ''
        clonedNode.src = dataURL
      } else {
        clonedNode.href.baseVal = dataURL
      }
    }))
    .then(() => clonedNode, () => clonedNode)
}
function embedChildren(clonedNode, options) {
  const children = util_1.toArray(clonedNode.childNodes)
  // eslint-disable-next-line no-use-before-define
  const deferreds = children.map(child => embedImages(child, options))
  return Promise.all(deferreds).then(() => clonedNode)
}
function embedImages(clonedNode, options) {
  if (!(clonedNode instanceof Element)) {
    return Promise.resolve(clonedNode)
  }
  return Promise.resolve(clonedNode)
    .then(node => embedBackground(node, options))
    .then(node => embedImageNode(node, options))
    .then(node => embedChildren(node, options))
}

export {
  embedImages,
}
