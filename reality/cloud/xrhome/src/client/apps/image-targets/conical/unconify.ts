const lerp0 = (p1: number, p2: number, alpha: number): number => p1 * alpha + (1 - alpha) * p2

type Color = [number, number, number]
const lerpColor = (color1: Color, color2: Color, alpha: number): Color => {
  const output: Color = [0, 0, 0]
  output[0] = color1[0] * alpha + (1 - alpha) * color2[0]
  output[1] = color1[1] * alpha + (1 - alpha) * color2[1]
  output[2] = color1[2] * alpha + (1 - alpha) * color2[2]
  return output
}

const getColor = (data: Uint8ClampedArray, x: number, y: number, rowBytes: number): Color => ([
  data[y * rowBytes + x * 4],
  data[y * rowBytes + x * 4 + 1],
  data[y * rowBytes + x * 4 + 2],
])

const rgbLinearInterp2d = (pix: ImageData, pt: Point): Color => {
  const {x} = pt
  const {y} = pt
  const xFloor = Math.floor(x)
  const yFloor = Math.floor(y)
  const xScale = 1 - (x - xFloor)
  const yScale = 1 - (y - yFloor)

  const pixData = pix.data
  const rowBytes = pix.width * 4
  const topPix: Color = lerpColor(
    getColor(pixData, xFloor, yFloor, rowBytes),
    getColor(pixData, xFloor + 1, yFloor, rowBytes), xScale
  )
  const bottomPix: Color = lerpColor(
    getColor(pixData, xFloor, yFloor + 1, rowBytes),
    getColor(pixData, xFloor + 1, yFloor + 1, rowBytes), xScale
  )
  return lerpColor(topPix, bottomPix, yScale)
}

const sampleImageWithLinearInterpolation = (
  pix: ImageData,
  samplingPoints: Point[],
  outputImg: ImageData
): void => {
  let k: number = 0
  const rowSize = outputImg.width * 4
  for (let i = 0; i < outputImg.height; i++) {
    for (let j = 0; j < outputImg.width; j++) {
      const pt: Point = samplingPoints[k]
      const isOutOfBounds = pt.x < 0 || pt.y < 0 || pt.x >= pix.width || pt.y >= pix.height

      // out of sampling bounds
      if (!isOutOfBounds) {
        // linear interpolation
        const [r, g, b] = rgbLinearInterp2d(pix, pt)
        outputImg.data[i * rowSize + j * 4] = r
        outputImg.data[i * rowSize + j * 4 + 1] = g
        outputImg.data[i * rowSize + j * 4 + 2] = b
        outputImg.data[i * rowSize + j * 4 + 3] = 255
      }

      k++
    }
  }
}

interface Point {
  x: number
  y: number
}

const pointDist = (pt1: Point, pt2: Point): number => {
  const dx = pt1.x - pt2.x
  const dy = pt1.y - pt2.y
  return Math.sqrt(dx * dx + dy * dy)
}

interface ConePixelPoints {
  tl: Point
  bl: Point
  tr: Point
  apex: Point
  theta?: number
  isFez?: boolean
}

// Given the small and large radius, return the corners of the image target within the rainbow
// image as well as the apex and theta.  If the image is a fez, then the output is inverted on
// the y axis.
const computePixelPointsFromRadius = (
  topRadius: number, bottomRadius: number, width: number
): ConePixelPoints => {
  const isFez = topRadius < 0
  const absTopRadius = Math.abs(topRadius)

  // x delta from center to bottom corners
  const x0 = ((width / 2) * bottomRadius) / absTopRadius
  // calculates angle with opposite (half of the width of image) / hypotenuse (radius going
  // from the apex to the top left corner).  2x makes it the full arc's angle.
  const theta = Math.asin(width / 2 / absTopRadius) * 2
  // y delta between top of the image and top corners
  const topY = 2 * absTopRadius * Math.sin(theta / 4) ** 2

  // y delta from center to bottom corners
  const y0 = x0 / Math.tan(theta / 2)

  const tl = {x: 0, y: topY}
  const bl = {x: width / 2 - x0, y: absTopRadius - y0}
  const tr = {x: width - 1, y: topY}
  const apex = {x: width / 2, y: absTopRadius}

  return {tl, bl, tr, apex, theta, isFez}
}

interface UnconifiedData {
  topRadius: number
  bottomRadius: number
  theta: number
  outputHeight: number
}

const getUnconifiedData = (pixelPoints: ConePixelPoints, outputWidth: number): UnconifiedData => {
  const {tl, bl, tr, apex} = pixelPoints
  // This part is recalculating the inputs to computePixelPointsFromRadius
  // distance between the apex and top left corner, which is the large radius.
  const topRadius = pointDist(apex, tl)
  // distance between the apex and bottom left corner, which is the small radius.
  const bottomRadius = pointDist(apex, bl)
  // distance between top left and top right corner. Should be the same as the width of the image.
  const imageWidth = pointDist(tl, tr)
  // calculates angle with opposite (half of the width of image) / hypotenuse (radius going
  // from the apex to the top left corner).  2x makes it the full arc's angle.
  const theta = 2 * Math.asin(imageWidth / 2 / topRadius)

  // calculate the height of the unconed image.  The width will stay the same.
  const outputWidthHeightRatio = (topRadius * theta) / (topRadius - bottomRadius)
  const outputHeight = Math.ceil(outputWidth / outputWidthHeightRatio)
  return {topRadius, bottomRadius, theta, outputHeight}
}

// given a rainbow image and the values provided by computePixelPointsFromRadius, return the
// flattened image
const unconify = (
  imgData: ImageData, pixelPoints: ConePixelPoints, outputWidth: number
): ImageData => {
  const {tl, apex, isFez} = pixelPoints

  const {
    topRadius, bottomRadius, theta, outputHeight,
  } = getUnconifiedData(pixelPoints, outputWidth)

  // angle between the y-axis and the line (apex, tl).  Essentially, half of theta.
  const alpha = Math.asin((apex.x - tl.x) / topRadius)

  const samplingPoints: Point[] = new Array(outputHeight * outputWidth)
  for (let i = 0; i < outputHeight; i++) {
    for (let j = 0; j < outputWidth; j++) {
      // angle between the line (sampling point, apex) and the line (bl, apex)
      const beta = (theta * j) / (outputWidth - 1)

      // the radius that intersects with the sampled point.  bottomRadius <= rSampling <= topRadius
      const rSampling = lerp0(topRadius, bottomRadius, (outputHeight - 1 - i) / outputHeight)

      // angle between the x-axis and the line (apex, samplingPoint)
      const psi = Math.PI / 2 + alpha - beta

      const sampledX = apex.x + rSampling * Math.cos(psi)

      // is the image isFez, then all the values are flipped on the Y axis. Therefore, we need to
      // flip the sampled Y and output Y.
      if (isFez) {
        const sampledY = imgData.height - apex.y + rSampling * Math.sin(psi)
        samplingPoints[outputWidth * (outputHeight - i - 1) + j] = {x: sampledX, y: sampledY}
      } else {
        const sampledY = apex.y - rSampling * Math.sin(psi)
        samplingPoints[outputWidth * i + j] = {x: sampledX, y: sampledY}
      }
    }
  }

  const retBuffer = new ImageData(outputWidth, outputHeight)
  sampleImageWithLinearInterpolation(imgData, samplingPoints, retBuffer)

  return retBuffer
}

export {
  lerp0,
  unconify,
  computePixelPointsFromRadius,
  getUnconifiedData,
}

export type {
  Point,
  ConePixelPoints,
}
