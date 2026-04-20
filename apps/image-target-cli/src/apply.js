import fs from 'fs/promises'
import path from 'path'
import {createRequire} from 'module'
import sharp from 'sharp'

import {validateCrop} from './crop.js'
import {computePixelPointsFromRadius, unconify} from './unconify.js'
import {ImageData} from './image-data.js'

const require = createRequire(import.meta.url)
const CONSTANTS = require('./constants.json')

/**
 * @param {import("sharp").Sharp} rawImage
 * @param {import("./types").CropResult} crop
 * @param {string} folder
 * @param {string} name
 * @param {boolean} overwriteFiles
 * @param {Omit<Partial<import("./types").ImageTargetData>, 'type' | 'properties'>} [extraData]
 */
const applyCrop = async (rawImage, crop, folder, name, overwriteFiles, extraData) => {
  rawImage.autoOrient()
  const {autoOrient: baseMetadata, format} = (await rawImage.metadata())

  /** @type {import("./types").ImageMetadata} */
  let metadata

  /** @type {import("sharp").Sharp | undefined} */
  let originalImage

  /** @type {import("sharp").Sharp | undefined} */
  let geometryImage

  if (crop.type === 'CONICAL') {
    const originalImageData = new ImageData(baseMetadata.width, baseMetadata.height)
    originalImageData.data.set(await rawImage.clone().raw().toBuffer())
    const points = computePixelPointsFromRadius(
      crop.properties.topRadius,
      crop.properties.bottomRadius,
      baseMetadata.width
    )
    const geometryImageData = unconify(originalImageData, points, baseMetadata.width)
    originalImage = rawImage
    geometryImage = sharp(geometryImageData.data, {
      raw: {
        width: geometryImageData.width,
        height: geometryImageData.height,
        channels: 4,
      },
    })
    if (crop.properties.isRotated) {
      geometryImage = geometryImage.rotate(90)
    }

    metadata = crop.properties.isRotated
      ? {
        width: geometryImageData.height,
        height: geometryImageData.width,
      }
      : {
        width: geometryImageData.width,
        height: geometryImageData.height,
      }
  } else if (crop.properties.isRotated) {
    originalImage = rawImage.clone().rotate(90)
    metadata = {width: baseMetadata.height, height: baseMetadata.width}
  } else {
    originalImage = rawImage
    metadata = baseMetadata
  }

  const issues = validateCrop(crop.properties, metadata)
  if (issues.length) {
    throw new Error(`Invalid crop geometry:\n${issues.join('\n')}`)
  }

  const extension = format === 'jpeg' ? 'jpg' : format

  /** @type {import("./types").ReferencedResources} */
  const resources = {
    originalImage: `${name}_original.${extension}`,
    croppedImage: `${name}_cropped.${extension}`,
    thumbnailImage: `${name}_thumbnail.${extension}`,
    luminanceImage: `${name}_luminance.${extension}`,
  }

  if (geometryImage) {
    resources.geometryImage = `${name}_geometry.${extension}`
  }

  // Final JSON
  /** @type {import("./types").ImageTargetData} */
  const data = {
    ...crop,
    // NOTE(christoph): This is a URL, not a relative path
    imagePath: `image-targets/${resources.luminanceImage}`,
    metadata: null,
    name,
    resources,
    created: Date.now(),
    updated: Date.now(),
    ...extraData,
  }

  const dataPath = path.join(folder, `${name}.json`)

  const cropSourceImage = geometryImage || originalImage
  const croppedImage = cropSourceImage.clone().extract(crop.properties)

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
    (geometryImage && resources.geometryImage)
      ? geometryImage.toFile(path.join(folder, resources.geometryImage))
      : null,
    fs.writeFile(dataPath, `${JSON.stringify(data, null, 2)}\n`),
  ])

  return {
    dataPath,
  }
}

export {applyCrop}
