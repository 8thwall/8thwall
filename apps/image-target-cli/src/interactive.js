import sharp from 'sharp'

import path from 'path'
import os from 'os'

import {getDefaultCrop} from './crop.js'
import {applyCrop} from './apply.js'
import {computePixelPointsFromRadius, getUnconifiedData} from './unconify.js'
import {
  getCircumferenceRatio, getConinessForRadii, getTargetCircumferenceBottom,
} from './curved-geometry.js'
import {isDirectory, getImageFiles} from './files.js'
import {processBatch, printBatchSummary} from './batch.js'

/**
 * @import {
 *   ImageMetadata, CropResult, CylinderCropGeometry, CropGeometry, CliInterface,
 *   ConicalCropResult,
 *  } from "./types"
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
 * @param {ImageMetadata} imageMetadata
 * @returns {Promise<CylinderCropGeometry>}
 */
const selectCylindricalGeometry = async (rl, imageMetadata) => {
  const unit = await rl.choose(
    'Select the unit to measure physical lengths',
    ['mm', 'in'],
    true
  )
  const cylinderCircumference = await rl.promptFloat(
    `Enter the circumference of the cylinder in ${unit}: `
  )
  const targetCircumference = await rl.promptFloat(
    `Enter the width of the image in ${unit}: `
  )

  if (targetCircumference > cylinderCircumference) {
    throw new Error('Image width cannot be greater than cylinder circumference')
  }

  const arcAngle = (targetCircumference / cylinderCircumference) * 360
  const cylinderSideLength = (imageMetadata.height / imageMetadata.width) * targetCircumference

  const baseGeometry = await selectPlanarGeometry(rl, imageMetadata)

  return {
    ...baseGeometry,
    targetCircumferenceTop: targetCircumference,
    cylinderCircumferenceTop: cylinderCircumference,
    cylinderCircumferenceBottom: cylinderCircumference,
    cylinderSideLength,
    arcAngle,
    coniness: 0,
    inputMode: 'ADVANCED',
    unit,
  }
}

/**
 * @param {CliInterface} rl
 * @param {ImageMetadata} imageMetadata
 * @returns {Promise<ConicalCropResult['geometry']>}
 */
const selectConicalGeometry = async (rl, imageMetadata) => {
  const widerSide = await rl.choose('Which side of the cone is wider?', ['top', 'bottom'], true)

  const isFez = widerSide === 'bottom'

  const outerRadius = await rl.promptFloat('Enter the outer radius of the image')

  if (outerRadius <= 0) {
    throw new Error('Outer radius must be positive')
  }
  const innerRadius = await rl.promptFloat('Enter the inner radius of the image')
  if (innerRadius < 0) {
    throw new Error('Inner radius cannot be negative')
  }

  const arcAngle = await rl.promptFloat(
    'Enter the arc angle of the cone in degrees (e.g. 360 for a full cone)'
  )
  if (arcAngle <= 0 || arcAngle > 360) {
    throw new Error('Arc angle must be between 0 and 360 degrees')
  }
  const topRadius = isFez ? -outerRadius : outerRadius
  const bottomRadius = innerRadius

  const points = computePixelPointsFromRadius(topRadius, bottomRadius, imageMetadata.width)
  const {outputHeight} = getUnconifiedData(points, imageMetadata.width)
  const geometryMetdata = {
    width: imageMetadata.width,
    height: outputHeight,
  }

  const crop = await selectPlanarGeometry(rl, geometryMetdata)

  const cylinderCircumferenceTop = 100
  const targetCircumferenceTop = (cylinderCircumferenceTop * arcAngle) / 360
  const circumferenceRatio = getCircumferenceRatio(topRadius, bottomRadius)
  const cylinderCircumferenceBottom = cylinderCircumferenceTop / circumferenceRatio
  const coniness = getConinessForRadii(topRadius, bottomRadius)

  const widerTargetCircumference = Math.max(
    targetCircumferenceTop,
    getTargetCircumferenceBottom(
      targetCircumferenceTop,
      cylinderCircumferenceTop,
      cylinderCircumferenceBottom
    )
  )

  const cylinderSideLength = widerTargetCircumference *
    (geometryMetdata.height / geometryMetdata.width)

  return {
    ...crop,
    cylinderCircumferenceTop,
    cylinderCircumferenceBottom,
    targetCircumferenceTop,
    cylinderSideLength,
    coniness,
    arcAngle,
    inputMode: 'ADVANCED',
    unit: 'mm',
    topRadius,
    bottomRadius,
  }
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
        geometry: await selectCylindricalGeometry(rl, imageMetadata),
      }
    case 'cone':
      return {type: 'CONICAL', geometry: await selectConicalGeometry(rl, imageMetadata)}
    default:
      throw new Error(`Unknown type: ${type}`)
  }
}

/**
 * Select geometry for batch mode, tracking if default crop was chosen
 * @param {CliInterface} rl
 * @param {ImageMetadata} imageMetadata
 * @returns {Promise<{geometry: CropResult, useDefaultCrop: boolean}>}
 */
const selectGeometryForBatch = async (rl, imageMetadata) => {
  const type = await rl.choose(
    'Select the image type:',
    ['flat', 'cylinder', 'cone'],
    true
  )

  // For flat images, track if user chose default crop
  if (type === 'flat') {
    const sourceIsLandscape = imageMetadata.width >= imageMetadata.height
    const useDefaultCrop = await rl.confirm('Use default crop? (recommended for batch with varying image sizes)', true)

    if (useDefaultCrop) {
      return {
        geometry: {
          type: 'PLANAR',
          geometry: getDefaultCrop(imageMetadata, sourceIsLandscape),
        },
        useDefaultCrop: true,
      }
    }

    // Manual crop - use exact values (may fail for different-sized images)
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

    let geometry
    if (isRotated) {
      const height = visualWidth
      const width = Math.round((height * 3) / 4)
      console.log('Computed height based on 4:3 aspect ratio:', width)
      geometry = {
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
      geometry = {
        top: visualTop,
        left: visualLeft,
        width: visualWidth,
        height,
        isRotated,
        originalWidth: imageMetadata.width,
        originalHeight: imageMetadata.height,
      }
    }

    console.log('\nWarning: Manual crop values will be applied to all images.')
    console.log('Images with different dimensions may fail processing.\n')

    return {
      geometry: {type: 'PLANAR', geometry},
      useDefaultCrop: false,
    }
  }

  // For cylinder/cone, use standard geometry selection (no per-image recalculation)
  if (type === 'cylinder') {
    return {
      geometry: {
        type: 'CYLINDER',
        geometry: await selectCylindricalGeometry(rl, imageMetadata),
      },
      useDefaultCrop: false,
    }
  }

  if (type === 'cone') {
    return {
      geometry: {type: 'CONICAL', geometry: await selectConicalGeometry(rl, imageMetadata)},
      useDefaultCrop: false,
    }
  }

  throw new Error(`Unknown type: ${type}`)
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
 * Process a single image (existing behavior)
 * @param {CliInterface} rl
 * @param {string} imagePath
 */
const processSingleMode = async (rl, imagePath) => {
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

/**
 * Process all images in a directory
 * @param {CliInterface} rl
 * @param {string} dirPath
 */
const processBatchMode = async (rl, dirPath) => {
  const imageFiles = await getImageFiles(dirPath)

  if (imageFiles.length === 0) {
    throw new Error(`No image files found in directory: ${dirPath}`)
  }

  console.log(`\nFound ${imageFiles.length} image(s) in directory:`)
  for (const file of imageFiles) {
    console.log(`  - ${path.basename(file)}`)
  }

  // Use first image as reference for geometry
  const referenceImagePath = imageFiles[0]
  const referenceImage = sharp(referenceImagePath)
  const imageMetadata = await referenceImage.metadata()

  console.log(`\nUsing "${path.basename(referenceImagePath)}" as reference for geometry settings.`)
  console.log(`Reference image dimensions: ${imageMetadata.width}x${imageMetadata.height}`)
  console.log('These settings will be applied to all images in the batch.\n')

  const {geometry, useDefaultCrop} = await selectGeometryForBatch(rl, imageMetadata)

  const folder = normalizePath(await rl.prompt('Enter the output folder: '))

  const overwrite = process.env.OVERWRITE_FILES === 'true'

  const onProgress = (current, total, filename) => {
    console.log(`\nProcessing image ${current} of ${total}: ${filename}`)
  }

  const results = await processBatch(
    imageFiles,
    geometry,
    folder,
    overwrite,
    useDefaultCrop,
    onProgress
  )

  printBatchSummary(results)
}

/**
 * @param {CliInterface} rl
 * @returns {Promise<void>}
 */
const selectProcessorOptions = async (rl) => {
  const mode = await rl.choose(
    'Select processing mode:',
    ['single', 'batch'],
    true
  )

  if (mode === 'batch') {
    const rawPath = (
      await rl.prompt('Enter the path to the directory containing images: ')
    ).trim()
    const dirPath = normalizePath(rawPath)

    if (!(await isDirectory(dirPath))) {
      throw new Error(`Not a directory: ${dirPath}`)
    }

    await processBatchMode(rl, dirPath)
  } else {
    const rawPath = (
      await rl.prompt('Enter the path to the image file: ')
    ).trim()
    const imagePath = normalizePath(rawPath)
    await processSingleMode(rl, imagePath)
  }
}

export {
  normalizePath,
  selectPlanarGeometry,
  selectCylindricalGeometry,
  selectConicalGeometry,
  selectProcessorOptions,
  processSingleMode,
  processBatchMode,
}
