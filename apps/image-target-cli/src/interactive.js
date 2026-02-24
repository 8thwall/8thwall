import sharp from 'sharp'

import path from 'path'
import os from 'os'

import {getDefaultCrop} from './crop.js'
import {applyCrop} from './apply.js'

/**
 * @import { ImageMetadata, CropResult, CropGeometry, CliInterface } from "./types"
 */

/**
 * Prompts for orientation and crop geometry.
 * @param {CliInterface} rl
 * @param {ImageMetadata} imageMetadata
 * @returns {Promise<CropGeometry>}
 */
const selectPlanarGeometry = async (rl, imageMetadata) => {
  const sourceIsLandscape = imageMetadata.width >= imageMetadata.height
  const useDefaultCrop = await rl.confirm('Use default crop?', true)
  if (useDefaultCrop) {
    return getDefaultCrop(imageMetadata, sourceIsLandscape)
  }

  const orientationOptions = sourceIsLandscape
    ? ['landscape', 'portrait']
    : ['portrait', 'landscape']
  const isRotated = await rl.choose(
    'Select the image orientation of the trackable region:',
    orientationOptions,
    true
  ) === 'landscape'

  const visualTop = await rl.promptInteger('Enter the top offset of the crop')
  const visualLeft = await rl.promptInteger('Enter the left offset of the crop')
  const visualWidth = await rl.promptInteger('Enter the width of the crop')

  if (isRotated) {
    // NOTE(christoph): These values are in terms of the original image as it appears on a screen,
    // but when stored, the crop is in the coordinates of the image after it has been rotated.

    // NOTE(christoph): Since we're rotating the image, the origin of the crop is the bottom
    // left of the image, so we need to adjust for that here.
    const height = visualWidth
    const width = Math.round((height * 3) / 4)
    console.log('Computed height based on 4:3 aspect ratio:', width)
    return {
      top: visualLeft,
      left: imageMetadata.height - visualTop - width,
      width,
      height,
      isRotated,
      originalWidth: imageMetadata.height,
      originalHeight: imageMetadata.width,
    }
  } else {
    const height = Math.round((visualWidth * 4) / 3)
    console.log('Computed height based on 3:4 aspect ratio:', height)
    return {
      top: visualTop,
      left: visualLeft,
      width: visualWidth,
      height,
      isRotated,
      originalWidth: imageMetadata.width,
      originalHeight: imageMetadata.height,
    }
  }
}

/**
 * @param {CliInterface} rl
 * @returns {Promise<any>}
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const selectCylindricalGeometry = async (rl) => {
  throw new Error('TODO')
}

/**
 * @param {CliInterface} rl
 * @returns {Promise<any>}
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const selectConicalGeometry = async (rl) => {
  throw new Error('TODO')
}

/**
 *
 * @param {CliInterface} rl
 * @param {ImageMetadata} imageMetadata
 * @returns {Promise<CropResult>}
 */
const selectGeometry = async (rl, imageMetadata) => {
  const type = await rl.choose(
    'Select the image type:',
    ['flat', 'cylinder', 'cone'],
    true
  )
  switch (type) {
    case 'flat':
      return {
        type: 'PLANAR',
        geometry: await selectPlanarGeometry(rl, imageMetadata),
      }
    case 'cylinder':
      return {
        type: 'CYLINDER',
        geometry: await selectCylindricalGeometry(rl),
      }
    case 'cone':
      return {type: 'CONICAL', geometry: await selectConicalGeometry(rl)}
    default:
      throw new Error(`Unknown type: ${type}`)
  }
}

// Takes in a raw string which may contain quotes, backslashes, and ~,
// and returns a normalized path.
const normalizePath = (input) => {
  if (!input || typeof input !== 'string') {
    throw new Error('Invalid path input')
  }

  let p = input.trim()

  // Remove surrounding quotes
  p = p.replace(/^(['"])(.*)\1$/, '$2')

  // Only unescape "\ " on Unix-like systems
  if (process.platform !== 'win32') {
    p = p.replace(/\\ /g, ' ')
  }

  // Expand ~ on Unix-like systems
  if (process.platform !== 'win32' && p.startsWith('~')) {
    p = path.join(os.homedir(), p.slice(1))
  }

  return path.resolve(p)
}

/**
 * @param {CliInterface} rl
 * @returns {Promise<void>}
 */
const selectProcessorOptions = async (rl) => {
  const rawPath = (
    await rl.prompt('Enter the path to the image file: ')
  ).trim()
  const imagePath = normalizePath(rawPath)
  const image = sharp(imagePath)
  const imageMetadata = await image.metadata()

  const geometry = await selectGeometry(rl, imageMetadata)

  const folder = normalizePath(await rl.prompt('Enter the output folder: '))

  const name = await rl.prompt('Enter a name for the image target: ')
  const {dataPath} = await applyCrop(
    image,
    geometry,
    folder,
    name,
    process.env.OVERWRITE_FILES === 'true'
  )

  console.log('Image target data saved to:', dataPath)
}

export {
  normalizePath,
  selectPlanarGeometry,
  selectCylindricalGeometry,
  selectConicalGeometry,
  selectProcessorOptions,
}
