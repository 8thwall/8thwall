import fs from 'fs/promises'
import path from 'path'
import {createRequire} from 'module'

import {validateCrop} from './crop.js'

const require = createRequire(import.meta.url)
const CONSTANTS = require('./constants.json')

/**
 * @param {import("sharp").Sharp} rawImage
 * @param {import("./types").CropResult} crop
 * @param {string} folder
 * @param {string} name
 * @param {boolean} overwriteFiles
 */
const applyCrop = async (rawImage, crop, folder, name, overwriteFiles) => {
  if (crop.type === 'CONICAL') {
    throw new Error('TODO: Add cone support')
  }
  const baseMetadata = await rawImage.metadata()
  const metadata = crop.geometry.isRotated
    ? {
      width: baseMetadata.height,
      height: baseMetadata.width,
    }
    : baseMetadata

  const issues = validateCrop(crop.geometry, metadata)
  if (issues.length) {
    throw new Error(`Invalid crop geometry:\n${issues.join('\n')}`)
  }

  const extension =
    baseMetadata.format === 'jpeg' ? 'jpg' : baseMetadata.format

  /** @type {import("./types").ReferencedResources} */
  const resources = {
    originalImage: `${name}_original.${extension}`,
    croppedImage: `${name}_cropped.${extension}`,
    thumbnailImage: `${name}_thumbnail.${extension}`,
    luminanceImage: `${name}_luminance.${extension}`,
  }

  // Final JSON
  /** @type {import("./types").ImageTargetData} */
  const data = {
    // NOTE(christoph): This is a URL, not a relative path
    imagePath: `image-targets/${resources.luminanceImage}`,
    metadata: null,
    name,
    type: crop.type,
    properties: crop.geometry,
    resources,
    created: Date.now(),
    updated: Date.now(),
  }

  const dataPath = path.join(folder, `${name}.json`)

  const originalImage = rawImage
    .clone()
    .autoOrient()
    .rotate(crop.geometry.isRotated ? 90 : 0)
  const croppedImage = originalImage.clone().extract(crop.geometry)

  const thumbnailImage = croppedImage
    .clone()
    .resize({height: CONSTANTS.thumbnailHeight})

  const luminanceImage = croppedImage
    .clone()
    .resize({height: CONSTANTS.luminanceHeight})
    .grayscale()

  const plannedPaths = [
    ...Object.values(resources).map(filename => path.join(folder, filename)),
    dataPath,
  ]

  if (!overwriteFiles) {
    for (const plannedPath of plannedPaths) {
      try {
        await fs.access(plannedPath)
        throw new Error(`File already exists, overwrite is disabled: ${plannedPath}`)
      } catch (err) {
        if (err.code !== 'ENOENT') {
          throw err
        }
      }
    }
  }

  await fs.mkdir(folder, {recursive: true})

  await Promise.all([
    originalImage.toFile(path.join(folder, resources.originalImage)),
    thumbnailImage.toFile(path.join(folder, resources.thumbnailImage)),
    luminanceImage.toFile(path.join(folder, resources.luminanceImage)),
    croppedImage.toFile(path.join(folder, resources.croppedImage)),
    fs.writeFile(dataPath, `${JSON.stringify(data, null, 2)}\n`),
  ])

  return {
    dataPath,
  }
}

export {applyCrop}
