const WOFF = 'application/font-woff'
const JPEG = 'image/jpeg'
const mimes = {
  woff: WOFF,
  woff2: WOFF,
  ttf: 'application/font-truetype',
  eot: 'application/vnd.ms-fontobject',
  png: 'image/png',
  jpg: JPEG,
  jpeg: JPEG,
  gif: 'image/gif',
  tiff: 'image/tiff',
  svg: 'image/svg+xml',
}
function getExtension(url) {
  const match = /\.([^./]*?)$/g.exec(url)
  return match ? match[1] : ''
}
// Wait for a frame using setTimeout 0. This is equivalent to requesting an animation frame, except
// that it also works in immersive sessions where requesting the animation frame requires a handle
// to the session (which we don't have here).
const waitAFrame = arg => new Promise(resolve => setTimeout(() => resolve(arg), 0))
function getMimeType(url) {
  const extension = getExtension(url).toLowerCase()
  return mimes[extension] || ''
}
function resolveUrl(url, baseUrl) {
  // url is absolute already
  if (url.match(/^[a-z]+:\/\//i)) {
    return url
  }
  // url is absolute already, without protocol
  if (url.match(/^\/\//)) {
    return window.location.protocol + url
  }
  // dataURI, mailto:, tel:, etc.
  if (url.match(/^[a-z]+:/i)) {
    return url
  }
  const doc = document.implementation.createHTMLDocument()
  const base = doc.createElement('base')
  const a = doc.createElement('a')
  doc.head.appendChild(base)
  doc.body.appendChild(a)
  if (baseUrl) {
    base.href = baseUrl
  }
  a.href = url
  return a.href
}
function isDataUrl(url) {
  return url.search(/^(data:)/) !== -1
}
function makeDataUrl(content, mimeType) {
  return `data:${mimeType};base64,${content}`
}
function parseDataUrlContent(dataURL) {
  return dataURL.split(/,/)[1]
}
const uuid = (function uuid() {
  // generate uuid for className of pseudo elements.
  // We should not use GUIDs, otherwise pseudo elements sometimes cannot be captured.
  let counter = 0
  // ref: http://stackoverflow.com/a/6248722/2519373
  const random = () =>
    // eslint-disable-next-line no-bitwise
    `0000${((Math.random() * Math.pow(36, 4)) << 0).toString(36)}`.slice(-4)
  return () => {
    counter += 1
    return `u${random()}${counter}`
  }
}())
const delay = ms => args => new Promise(resolve => setTimeout(() => resolve(args), ms))
function toArray(arrayLike) {
  const arr = []
  for (let i = 0, l = arrayLike.length; i < l; i += 1) {
    arr.push(arrayLike[i])
  }
  return arr
}
function px(node, styleProperty) {
  const val = window.getComputedStyle(node).getPropertyValue(styleProperty)
  return parseFloat(val.replace('px', ''))
}
function getNodeWidth(node) {
  const leftBorder = px(node, 'border-left-width')
  const rightBorder = px(node, 'border-right-width')
  return node.clientWidth + leftBorder + rightBorder
}
function getNodeHeight(node) {
  const topBorder = px(node, 'border-top-width')
  const bottomBorder = px(node, 'border-bottom-width')
  return node.clientHeight + topBorder + bottomBorder
}
function getPixelRatio() {
  let ratio
  let FINAL_PROCESS
  try {
    FINAL_PROCESS = process
  } catch (e) {
    // pass
  }
  const val = FINAL_PROCESS && FINAL_PROCESS.env
    ? FINAL_PROCESS.env.devicePixelRatio
    : null
  if (val) {
    ratio = parseInt(val, 10)
    if (Number.isNaN(ratio)) {
      ratio = 1
    }
  }
  return ratio || window.devicePixelRatio || 1
}
function canvasToBlob(canvas) {
  if (canvas.toBlob) {
    return new Promise(resolve => canvas.toBlob(resolve))
  }
  return new Promise((resolve) => {
    const binaryString = window.atob(canvas.toDataURL().split(',')[1])
    const len = binaryString.length
    const binaryArray = new Uint8Array(len)
    for (let i = 0; i < len; i += 1) {
      binaryArray[i] = binaryString.charCodeAt(i)
    }
    resolve(new Blob([binaryArray], {type: 'image/png'}))
  })
}
function createImage(url) {
  return new Promise((resolve, reject) => {
    const img = new Image()
    img.onload = () => resolve(img)
    img.onerror = reject
    img.crossOrigin = 'anonymous'
    img.decoding = 'sync'
    img.src = url
  })
}

let outputImage = null
function createOutputImage(url) {
  if (!outputImage) {
    outputImage = new Image()
    outputImage.crossOrigin = 'anonymous'
    outputImage.decoding = 'async'
    outputImage.width = 1024
    outputImage.height = 1024
  }
  return new Promise((resolve, reject) => {
    outputImage.onerror = reject
    outputImage.onload = () => resolve(outputImage)
    outputImage.src = url
  })
}
function svgToDataURL(svg) {
  return Promise.resolve()
    .then(() => new XMLSerializer().serializeToString(svg))
    // Uncomment to dump the generated svg into the console.
    // .then((s) => {
    //   console.log(s)
    //   return s
    // })
    .then(encodeURIComponent)
    .then(html => `data:image/svg+xml;charset=utf-8,${html}`)
}

function nodeToDataURL(node, width, height) {
  const xmlns = 'http://www.w3.org/2000/svg'
  const svg = document.createElementNS(xmlns, 'svg')
  const foreignObject = document.createElementNS(xmlns, 'foreignObject')
  svg.setAttribute('width', `${width}`)
  svg.setAttribute('height', `${height}`)
  svg.setAttribute('viewBox', `0 0 ${width} ${height}`)
  foreignObject.setAttribute('width', '100%')
  foreignObject.setAttribute('height', '100%')
  foreignObject.setAttribute('x', '0')
  foreignObject.setAttribute('y', '0')
  foreignObject.setAttribute('externalResourcesRequired', 'true')
  svg.appendChild(foreignObject)
  foreignObject.appendChild(node)
  // TODO(pawel) Further investigation: possible to get the SVGSVGElement (type of svg) to render
  // directly instead of having to serialize it?
  return svgToDataURL(svg)
}

export {
  canvasToBlob,
  createImage,
  createOutputImage,
  delay,
  getExtension,
  getMimeType,
  getNodeHeight,
  getNodeWidth,
  getPixelRatio,
  isDataUrl,
  makeDataUrl,
  nodeToDataURL,
  parseDataUrlContent,
  resolveUrl,
  svgToDataURL,
  toArray,
  waitAFrame,
  uuid,
}
