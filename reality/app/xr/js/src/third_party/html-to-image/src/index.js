import * as cloneNode_1 from './cloneNode'
import * as embedImages_1 from './embedImages'
import * as applyStyleWithOptions_1 from './applyStyleWithOptions'
import * as embedWebFonts_1 from './embedWebFonts'
import * as util_1 from './util'
import {create8wCanvas} from '../../../canvas'

function getImageSize(node, options = {}) {
  const width = options.width || util_1.getNodeWidth(node)
  const height = options.height || util_1.getNodeHeight(node)
  return {width, height}
}
function toSvg(node, options = {}) {
  const {width, height} = getImageSize(node, options)
  return Promise.resolve(node)
    .then(nativeNode => cloneNode_1.cloneNode(nativeNode, options, true))
    .then(util_1.waitAFrame)
    .then(clonedNode => embedWebFonts_1.embedWebFonts(clonedNode, options))
    .then(util_1.waitAFrame)
    .then(clonedNode => embedImages_1.embedImages(clonedNode, options))
    .then(util_1.waitAFrame)
    .then(clonedNode => applyStyleWithOptions_1.applyStyleWithOptions(clonedNode, options))
    .then(clonedNode => util_1.nodeToDataURL(clonedNode, width, height))
}
const dimensionCanvasLimit = 16384  // as per https://developer.mozilla.org/en-US/docs/Web/HTML/Element/canvas#maximum_canvas_size
function checkCanvasDimensions(canvas) {
  if (canvas.width > dimensionCanvasLimit ||
        canvas.height > dimensionCanvasLimit) {
    if (canvas.width > dimensionCanvasLimit &&
            canvas.height > dimensionCanvasLimit) {
      if (canvas.width > canvas.height) {
        canvas.height *= dimensionCanvasLimit / canvas.width
        canvas.width = dimensionCanvasLimit
      } else {
        canvas.width *= dimensionCanvasLimit / canvas.height
        canvas.height = dimensionCanvasLimit
      }
    } else if (canvas.width > dimensionCanvasLimit) {
      canvas.height *= dimensionCanvasLimit / canvas.width
      canvas.width = dimensionCanvasLimit
    } else {
      canvas.width *= dimensionCanvasLimit / canvas.height
      canvas.height = dimensionCanvasLimit
    }
  }
}
function toCanvas(node, options = {}) {
  return toSvg(node, options)
    .then(util_1.waitAFrame)
    .then(util_1.createImage)
    .then(util_1.waitAFrame)
    .then((img) => {
      const canvas = options.canvas || create8wCanvas('html-to-image')
      const context = canvas.getContext('2d')
      const ratio = options.pixelRatio || util_1.getPixelRatio()
      const {width, height} = getImageSize(node, options)
      const canvasWidth = options.canvasWidth || width
      const canvasHeight = options.canvasHeight || height
      if (options.disableCanvasResize) {
        // Setting the canvas's width to itself also clears the canvas.
        canvas.width = canvas.width  // eslint-disable-line no-self-assign
      } else {
        canvas.width = canvasWidth * ratio
        canvas.height = canvasHeight * ratio
      }
      if (!options.skipAutoScale) {
        checkCanvasDimensions(canvas)
      }
      canvas.domWidth = canvasWidth
      canvas.domHeight = canvasHeight
      if (options.backgroundColor) {
        context.fillStyle = options.backgroundColor
        context.fillRect(0, 0, canvas.width, canvas.height)
      }
      context.drawImage(img, 0, 0, canvas.width, canvas.height)
      return canvas
    })
}
function toImage(node, options = {}) {
  return toSvg(node, options)
    .then(util_1.waitAFrame)
    .then(util_1.createOutputImage)
    .then(util_1.waitAFrame)
}
function toPixelData(node, options = {}) {
  const {width, height} = getImageSize(node, options)
  return toCanvas(node, options).then((canvas) => {
    const ctx = canvas.getContext('2d')
    return ctx.getImageData(0, 0, width, height).data
  })
}
function toPng(node, options = {}) {
  return toCanvas(node, options).then(canvas => canvas.toDataURL())
}
function toJpeg(node, options = {}) {
  return toCanvas(node, options).then(canvas => canvas.toDataURL('image/jpeg', options.quality || 1))
}
function toBlob(node, options = {}) {
  return toCanvas(node, options).then(util_1.canvasToBlob)
}
function getFontEmbedCSS(node, options = {}) {
  return embedWebFonts_1.getWebFontCSS(node, options)
}

export {
  getFontEmbedCSS,
  toBlob,
  toImage,
  toJpeg,
  toPng,
  toPixelData,
  toCanvas,
  toSvg,
}
