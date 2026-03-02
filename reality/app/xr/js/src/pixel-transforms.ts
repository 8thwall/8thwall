/**
 * Return scale factor for resizing input image to desired dimensions that can be cropped
 *   while maintain original aspect ratio
 * @param imageWidth the image width
 * @param imageHeight the image height
 * @param desiredWidth the desired width after scaling
 * @param desiredHeight the desired height after scaling
 */
const scaleFactorSameAspectRatioCrop = (
  imageWidth: number,
  imageHeight: number,
  desiredWidth: number,
  desiredHeight: number
) => {
  const wRatio = desiredWidth / imageWidth
  const hRatio = desiredHeight / imageHeight
  const resizeRatio = (wRatio > hRatio) ? wRatio : hRatio
  return resizeRatio
}

/**
 * Copy RGBAU8 pixels directly into destination image with letterboxing padded with black pixels.
 * @param imgData the input image
 * @param srcWidth the source image width without letterbox
 * @param srcHeight the source image height without letterbox
 * @param dstWidth the destination image width with letterbox,
 *                 should be equal or larger than srcWidth
 * @param dstHeight the destination image height with letterbox,
 *                  should be equal or larger than srcHeight
 * @param imgArr out param which we will fill with the resized image.
 */
const toImageArrayLetterbox = (
  imgData: Uint8Array,
  srcWidth: number,
  srcHeight: number,
  dstWidth: number,
  dstHeight: number,
  imgArr: Uint8Array
) => {
  const rowOffset = (dstHeight - srcHeight) / 2
  const colOffset = (dstWidth - srcWidth) / 2

  for (let r = 0; r < srcHeight; ++r) {
    let srcIdx = 4 * r * srcWidth
    let dstIdx = 4 * ((r + rowOffset) * dstWidth + colOffset)
    for (let c = 0; c < srcWidth; ++c) {
      imgArr[dstIdx] = imgData[srcIdx + 0]
      imgArr[dstIdx + 1] = imgData[srcIdx + 1]
      imgArr[dstIdx + 2] = imgData[srcIdx + 2]
      imgArr[dstIdx + 3] = imgData[srcIdx + 3]

      srcIdx += 4
      dstIdx += 4
    }
  }
}

/**
 * Rotate RGBA8888 image colorwise 90 degree with letterboxing.
 * Normally used for rotating images from ORIENTATION_LEFT landscape mode into portrait images.
 * @param srcWidth the source image width without letterbox
 * @param srcHeight the source image height without letterbox
 * @param dstWidth the destination image width with letterbox,
 *                 should be equal or larger than srcHeight because of rotation
 * @param dstHeight the destination image height with letterbox,
 *                  should be equal or larger than srcWidth because of rotation
 * @param imgArr out param which we will fill with the resized image.
 */
const toImageArrayRotateCW90 = (
  srcImgData: Uint8Array,
  srcWidth: number,
  srcHeight: number,
  dstWidth: number,
  dstHeight: number,
  imgArr: Uint8Array
) => {
  const srcRowBytes = 4 * srcWidth

  const dstColOffset = (dstWidth - srcHeight) / 2
  const dstRowOffset = (dstHeight - srcWidth) / 2
  const dstRowBytes = 4 * dstWidth

  for (let c = 0; c < srcWidth; c++) {
    let srcIdx = (srcHeight - 1) * srcRowBytes + 4 * c
    let dstIdx = dstRowBytes * (dstRowOffset + c) + 4 * dstColOffset
    for (let r = srcHeight - 1; r >= 0; r--) {
      imgArr[dstIdx + 0] = srcImgData[srcIdx + 0]
      imgArr[dstIdx + 1] = srcImgData[srcIdx + 1]
      imgArr[dstIdx + 2] = srcImgData[srcIdx + 2]
      imgArr[dstIdx + 3] = srcImgData[srcIdx + 3]
      srcIdx -= srcRowBytes
      dstIdx += 4
    }
  }
}

/**
 * Rotate RGBA8888 image counter-colorwise 90 degree with letterboxing.
 * Normally used for rotating images from ORIENTATION_RIGHT landscape mode into portrait images.
 * @param srcWidth the source image width without letterbox
 * @param srcHeight the source image height without letterbox
 * @param dstWidth the destination image width with letterbox,
 *                 should be equal or larger than srcHeight because of rotation
 * @param dstHeight the destination image height with letterbox,
 *                  should be equal or larger than srcWidth because of rotation
 * @param imgArr out param which we will fill with the resized image.
 */
const toImageArrayRotateCCW90 = (
  srcImgData: Uint8Array,
  srcWidth: number,
  srcHeight: number,
  dstWidth: number,
  dstHeight: number,
  imgArr: Uint8Array
) => {
  const srcRowBytes = 4 * srcWidth

  const dstColOffset = (dstWidth - srcHeight) / 2
  const dstRowOffset = (dstHeight - srcWidth) / 2
  const dstRowBytes = 4 * dstWidth

  for (let c = 0; c < srcWidth; c++) {
    let srcIdx = 4 * (srcWidth - 1 - c)
    let dstIdx = dstRowBytes * (dstRowOffset + c) + 4 * dstColOffset
    for (let r = srcHeight - 1; r >= 0; r--) {
      imgArr[dstIdx + 0] = srcImgData[srcIdx + 0]
      imgArr[dstIdx + 1] = srcImgData[srcIdx + 1]
      imgArr[dstIdx + 2] = srcImgData[srcIdx + 2]
      imgArr[dstIdx + 3] = srcImgData[srcIdx + 3]
      srcIdx += srcRowBytes
      dstIdx += 4
    }
  }
}

const normalizeColors = (arr: Float32Array) => {
  for (let i = 0; i < arr.length; i += 1) {
    arr[i] /= 255
  }
}

export {
  scaleFactorSameAspectRatioCrop,
  toImageArrayLetterbox,
  toImageArrayRotateCW90,
  toImageArrayRotateCCW90,
  normalizeColors,
}
