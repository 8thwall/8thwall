import fs from 'fs/promises'
import path from 'path'

// Supported image extensions
const IMAGE_EXTENSIONS = ['.jpg', '.jpeg', '.png', '.webp', '.gif', '.tiff', '.tif']

/**
 * Check if a path is a directory
 * @param {string} inputPath
 * @returns {Promise<boolean>}
 */
const isDirectory = async (inputPath) => {
  try {
    const stats = await fs.stat(inputPath)
    return stats.isDirectory()
  } catch {
    return false
  }
}

/**
 * Get all image files in a directory
 * @param {string} dirPath
 * @returns {Promise<string[]>} - Array of absolute paths to image files
 */
const getImageFiles = async (dirPath) => {
  const entries = await fs.readdir(dirPath, {withFileTypes: true})
  const imageFiles = entries
    .filter((entry) => {
      if (!entry.isFile()) return false
      const ext = path.extname(entry.name).toLowerCase()
      return IMAGE_EXTENSIONS.includes(ext)
    })
    .map((entry) => path.join(dirPath, entry.name))
    .sort()

  return imageFiles
}

/**
 * Extract target name from filename (without extension, lowercase)
 * @param {string} filePath
 * @returns {string}
 */
const getTargetName = (filePath) => {
  return path.basename(filePath, path.extname(filePath)).toLowerCase()
}

export {isDirectory, getImageFiles, getTargetName, IMAGE_EXTENSIONS}
