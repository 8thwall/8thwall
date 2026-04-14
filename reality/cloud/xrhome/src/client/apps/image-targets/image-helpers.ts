import {MINIMUM_SHORT_LENGTH, MINIMUM_LONG_LENGTH} from '../../../shared/xrengine-config'
import type {CropAreaPixels} from '../../common/image-cropper'
import type {CanvasPool} from '../../common/resource-pool'
import {computePixelPointsFromRadius, getUnconifiedData} from './conical/unconify'

type ImageInfo = {url: string, data: ImageData}

const getMaxZoom = (baseWidth, baseHeight, isLandscape): number => (isLandscape
  ? Math.min(baseHeight / MINIMUM_SHORT_LENGTH, baseWidth / MINIMUM_LONG_LENGTH)
  : Math.min(baseWidth / MINIMUM_SHORT_LENGTH, baseHeight / MINIMUM_LONG_LENGTH))

// Returns true if dimensions is allowable, in any rotation
const isUsableDimensions = (width, height) => Math.min(width, height) >= MINIMUM_SHORT_LENGTH &&
  Math.max(width, height) >= MINIMUM_LONG_LENGTH

// Returns true if dimensions are allowable as is.
const isValidDimensions = (width, height, isLandscape): boolean => {
  if ((isLandscape ? height : width) < MINIMUM_SHORT_LENGTH) {
    return false
  }
  if ((isLandscape ? width : height) < MINIMUM_LONG_LENGTH) {
    return false
  }
  return true
}

// returns the largest crop area with the given ratio that fits, centered
const getMaximumCropAreaPixels = (width, height, aspect): CropAreaPixels => {
  const maxCropWidth = Math.min(width, height * aspect)
  const maxCropHeight = Math.min(height, width / aspect)
  return {
    left: Math.floor((width - maxCropWidth) / 2),
    top: Math.floor((height - maxCropHeight) / 2),
    width: Math.floor(maxCropWidth),
    height: Math.floor(maxCropHeight),
  }
}

// places the bottom arc such that it aligns with a 'rainbow' that fills the space
// https://www.desmos.com/calculator/68lbnsd6dv
const getDefaultBottomRadius = (width, height, rTop): number => {
  const pX = width / 2
  const pY = Math.sqrt(rTop ** 2 - pX ** 2)
  const qY = rTop - height
  const qX = (pX / pY) * qY
  const rBottom = Math.sqrt(qX ** 2 + qY ** 2)
  const minUnconifiedHeight = height > width ? MINIMUM_LONG_LENGTH : MINIMUM_SHORT_LENGTH
  return Math.min(rBottom, rTop - minUnconifiedHeight)
}

const loadImage = (url: string, imgPool): Promise<HTMLImageElement> => new Promise((resolve) => {
  const img = imgPool.get()
  img.onload = () => {
    resolve(img)
  }
  img.src = url
})

const getImageBlobFromUrl = async (
  img: HTMLImageElement, w: number, h: number, fileType: string, canvasPool
): Promise<Blob> => {
  const canvas = canvasPool.get()
  const canvasCtx = canvas.getContext('2d')
  canvas.width = w
  canvas.height = h
  canvasCtx.drawImage(img, 0, 0)

  return new Promise((resolve) => {
    canvas.toBlob((blob) => {
      canvas.release()
      resolve(blob)
    }, fileType)
  })
}

const getDownScaledImage = async (
  img: HTMLImageElement,
  maxW: number,
  maxH: number,
  fileType: string,
  canvasPool,
  imgPool
): Promise<HTMLImageElement> => {
  let {naturalWidth, naturalHeight} = img
  if (naturalWidth > naturalHeight) {
    if (naturalWidth > maxW) {
      naturalHeight *= maxW / naturalWidth
      naturalWidth = maxW
    }
  } else if (naturalHeight > maxH) {
    naturalWidth *= maxH / naturalHeight
    naturalHeight = maxH
  } else {
    // Image doesn't need to be scaled down.
    return Promise.resolve(img)
  }

  const canvas = canvasPool.get()
  const reducedImg = imgPool.get()
  const canvasCtx = canvas.getContext('2d')
  canvas.width = naturalWidth
  canvas.height = naturalHeight
  canvasCtx.drawImage(img, 0, 0, naturalWidth, naturalHeight)

  const blob: Blob = await new Promise((resolve) => {
    canvas.toBlob((blob_) => {
      canvas.release()
      resolve(blob_)
    }, fileType)
  })

  return new Promise((resolve) => {
    const url = URL.createObjectURL(blob)
    reducedImg.onload = () => {
      URL.revokeObjectURL(url)
      resolve(reducedImg)
    }
    reducedImg.src = url
  })
}

// Rotation angle is rotation * 90 deg
const getImageDataRotated =
  (img: HTMLImageElement, rotation: number, fileType: string, canvasPool) => {
    const canvas = canvasPool.get()
    const ctx = canvas.getContext('2d')

    // Canvas dimensions are swapped if rotation is 90 or 270 deg
    if (rotation % 2 === 1) {
      canvas.width = img.naturalHeight
      canvas.height = img.naturalWidth
    } else {
      canvas.width = img.naturalWidth
      canvas.height = img.naturalHeight
    }

    ctx.save()
    ctx.translate(canvas.width / 2, canvas.height / 2)
    ctx.rotate((Math.PI / 2) * rotation)
    ctx.translate(-img.naturalWidth / 2, -img.naturalHeight / 2)

    ctx.drawImage(img, 0, 0)

    ctx.restore()

    canvas.release()
    return canvas.toDataURL(fileType)
  }

const getImageDataWithBackground =
  (img: HTMLImageElement, fillColor: string, isRotated: boolean, fileType: string, canvasPool) => {
    const canvas = canvasPool.get()
    const ctx = canvas.getContext('2d')
    if (isRotated) {
      canvas.width = img.naturalHeight
      canvas.height = img.naturalWidth
    } else {
      canvas.width = img.naturalWidth
      canvas.height = img.naturalHeight
    }

    ctx.fillStyle = fillColor
    ctx.fillRect(0, 0, canvas.width, canvas.height)

    if (isRotated) {
      ctx.save()
      ctx.rotate(Math.PI / 2)
      ctx.translate(0, -canvas.width)
    }

    ctx.drawImage(img, 0, 0)
    const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height)

    if (isRotated) {
      ctx.restore()
    }

    canvas.release()
    return {dataURL: canvas.toDataURL(fileType), imageData}
  }

const getLuminosity = (img: HTMLImageElement, canvasPool): number => {
  const canvas = canvasPool.get()
  const ctx = canvas.getContext('2d')

  canvas.width = 1
  canvas.height = 1
  ctx.clearRect(0, 0, 1, 1)
  ctx.drawImage(img, 0, 0, 1, 1)
  const canvasImageData = ctx.getImageData(0, 0, 1, 1).data
  const luminosity =
    0.21 * canvasImageData[0] + 0.72 * canvasImageData[1] + 0.07 * canvasImageData[2]

  canvas.release()
  return luminosity
}

// Rotation angle is rotation * 90 deg
const processImageData = (
  img: ImageData, canvasPool: CanvasPool, rotation: number,
  crop = {left: 0, top: 0, width: img.width, height: img.height}
): ImageInfo => {
  const buffer = canvasPool.get()
  const bufferCtx = buffer.getContext('2d')
  buffer.width = img.width
  buffer.height = img.height
  bufferCtx.putImageData(img, 0, 0)

  const canvas = canvasPool.get()
  const ctx = canvas.getContext('2d')
  canvas.width = (rotation % 2 === 0) ? crop.width : crop.height
  canvas.height = (rotation % 2 === 0) ? crop.height : crop.width

  ctx.save()
  ctx.translate(canvas.width / 2, canvas.height / 2)
  ctx.rotate((Math.PI / 2) * rotation)
  ctx.translate(-crop.width / 2, -crop.height / 2)
  ctx.drawImage(buffer, crop.left, crop.top, crop.width, crop.height,
    0, 0, crop.width, crop.height)
  ctx.restore()

  const data = ctx.getImageData(0, 0, canvas.width, canvas.height)
  canvas.release()
  buffer.release()
  return {url: canvas.toDataURL(), data}
}

const getImageFromTag = (img: HTMLImageElement, canvasPool: CanvasPool): ImageInfo => {
  const canvas = canvasPool.get()
  const ctx = canvas.getContext('2d')
  canvas.width = img.naturalWidth
  canvas.height = img.naturalHeight
  ctx.drawImage(img, 0, 0)
  const data = ctx.getImageData(0, 0, canvas.width, canvas.height)
  canvas.release()
  return {url: canvas.toDataURL(), data}
}

const getUnconifiedHeight = (
  topRadius: number, bottomRadius: number, outputWidth: number
): number => getUnconifiedData(
  computePixelPointsFromRadius(topRadius, bottomRadius, outputWidth), outputWidth
).outputHeight

// Given a top radius, width, and desired output height, calculate the necessary bottom radius.
// outputWidthHeightRatio = (topRadius * theta) / (topRadius - bottomRadiuus)
// outputHeight = Math.ceil(outputWidth / outputWidthHeightRatio)
// solve for bottom radius
const getBottomRadius = (
  topRadius: number, outputWidth: number, outputHeight: number
): number => {
  const absTopRadius = Math.abs(topRadius)
  const theta = 2 * Math.asin(outputWidth / 2 / absTopRadius)
  return absTopRadius - (absTopRadius * theta * outputHeight) / outputWidth
}

// Solve for when the above equation is defined and positive
const getMinimumTopRadius = (
  outputWidth: number, outputHeight: number
): number => {
  const zeroLimit = outputWidth / (2 * Math.sin(outputWidth / (2 * outputHeight)))
  const undefinedLimit = outputWidth / 2
  return (outputWidth / outputHeight < Math.PI) ? zeroLimit : undefinedLimit
}

export type {ImageInfo}

export {
  getMaxZoom,
  isUsableDimensions,
  isValidDimensions,
  getMaximumCropAreaPixels,
  getDefaultBottomRadius,
  loadImage,
  getImageBlobFromUrl,
  getDownScaledImage,
  getImageDataRotated,
  getImageDataWithBackground,
  getLuminosity,
  processImageData,
  getImageFromTag,
  getUnconifiedHeight,
  getBottomRadius,
  getMinimumTopRadius,
}
