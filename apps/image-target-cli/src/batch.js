import sharp from 'sharp'
import path from 'path'
import {createRequire} from 'module'

import {applyCrop} from './apply.js'
import {getTargetName} from './files.js'
import {getDefaultCrop} from './crop.js'

const require = createRequire(import.meta.url)
const CONSTANTS = require('./constants.json')

/**
 * @typedef {Object} BatchResult
 * @property {string} imagePath
 * @property {string} targetName
 * @property {boolean} success
 * @property {string} [dataPath]
 * @property {string} [error]
 */

/**
 * Calculate the scale factor needed to meet minimum crop dimensions
 * @param {number} width - Image width
 * @param {number} height - Image height
 * @returns {number} - Scale factor (1 if no scaling needed)
 */
const getUpscaleFactor = (width, height) => {
  // Calculate what the default crop would be (3:4 aspect ratio)
  let cropWidth, cropHeight
  if (width / 3 > height / 4) {
    // Constrained by height
    cropHeight = height
    cropWidth = Math.round((height * 3) / 4)
  } else {
    // Constrained by width
    cropWidth = width
    cropHeight = Math.round((width * 4) / 3)
  }

  // Calculate scale factor needed to meet minimums
  const widthScale = CONSTANTS.minimumWidth / cropWidth
  const heightScale = CONSTANTS.minimumHeight / cropHeight
  const scaleFactor = Math.max(widthScale, heightScale, 1)

  return scaleFactor
}

/**
 * Process multiple images with the same geometry settings
 * @param {string[]} imagePaths - Array of absolute paths to images
 * @param {import("./types").CropResult} geometry - Geometry settings to apply
 * @param {string} outputFolder - Output folder for all processed images
 * @param {boolean} overwrite - Whether to overwrite existing files
 * @param {boolean} useDefaultCrop - Recalculate default crop per image
 * @param {(current: number, total: number, name: string) => void} [onProgress] - Progress callback
 * @returns {Promise<BatchResult[]>}
 */
const processBatch = async (imagePaths, geometry, outputFolder, overwrite, useDefaultCrop, onProgress) => {
  const results = []
  const total = imagePaths.length

  for (let i = 0; i < imagePaths.length; i++) {
    const imagePath = imagePaths[i]
    const targetName = getTargetName(imagePath)

    if (onProgress) {
      onProgress(i + 1, total, path.basename(imagePath))
    }

    try {
      let image = sharp(imagePath)
      let imageMetadata = await image.metadata()

      // Upscale image if needed to meet minimum crop dimensions
      const scaleFactor = getUpscaleFactor(imageMetadata.width, imageMetadata.height)
      if (scaleFactor > 1) {
        const newWidth = Math.round(imageMetadata.width * scaleFactor)
        const newHeight = Math.round(imageMetadata.height * scaleFactor)
        // Force resize to complete by converting to buffer, so applyCrop sees correct dimensions
        const resizedBuffer = await image.resize(newWidth, newHeight).toBuffer()
        image = sharp(resizedBuffer)
        imageMetadata = await image.metadata()
      }

      // Recalculate geometry for this image if using default crop (only for PLANAR)
      let imageGeometry = geometry
      if (useDefaultCrop && geometry.type === 'PLANAR') {
        const sourceIsLandscape = imageMetadata.width >= imageMetadata.height
        imageGeometry = {
          type: 'PLANAR',
          geometry: getDefaultCrop(imageMetadata, sourceIsLandscape),
        }
      }

      const {dataPath} = await applyCrop(
        image,
        imageGeometry,
        outputFolder,
        targetName,
        overwrite
      )

      results.push({
        imagePath,
        targetName,
        success: true,
        dataPath,
      })
    } catch (err) {
      results.push({
        imagePath,
        targetName,
        success: false,
        error: err.message,
      })
    }
  }

  return results
}

/**
 * Print batch processing summary
 * @param {BatchResult[]} results
 */
const printBatchSummary = (results) => {
  const successful = results.filter((r) => r.success)
  const failed = results.filter((r) => !r.success)

  console.log('\n--- Batch Processing Summary ---')
  console.log(`Total: ${results.length}`)
  console.log(`Successful: ${successful.length}`)
  console.log(`Failed: ${failed.length}`)

  if (failed.length > 0) {
    console.log('\nFailed images:')
    for (const result of failed) {
      console.log(`  - ${path.basename(result.imagePath)}: ${result.error}`)
    }
  }

  if (successful.length > 0) {
    console.log('\nOutput files created in:', path.dirname(successful[0].dataPath))
  }
}

export {processBatch, printBatchSummary}
