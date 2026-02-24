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
  } else {
    const orientationOptions = sourceIsLandscape
      ? ['landscape', 'portrait']
      : ['portrait', 'landscape']
    const isRotated = await rl.choose(
      'Select the image orientation of the trackable region:',
      orientationOptions,
      true
    ) === 'landscape'

    const top = await rl.promptInteger('Enter the top offset of the crop')
    const left = await rl.promptInteger('Enter the left offset of the crop')
    const width = await rl.promptInteger('Enter the width of the crop')
    const height = isRotated
      ? Math.round((width * 3) / 4)
      : Math.round((width * 4) / 3)
    console.log('Height, determined by fixed aspect ratio:', height)

    const [originalWidth, originalHeight] = isRotated
      ? [imageMetadata.height, imageMetadata.width]
      : [imageMetadata.width, imageMetadata.height]
    return {
      top,
      left,
      width,
      height,
      isRotated,
      originalWidth,
      originalHeight,
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
