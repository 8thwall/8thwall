import {join, dirname, basename} from 'path'

import {fileExt, joinExt} from '../editor/editor-common'

const ASSET_FOLDER_PREFIX = 'assets/'
const ASSET_FOLDER = ASSET_FOLDER_PREFIX.replace(/\/$/, '')

const BACKEND_FOLDER_PREFIX = 'backends/'
const BACKEND_FOLDER = BACKEND_FOLDER_PREFIX.replace(/\/$/, '')
const BACKEND_FILE_EXT = '.backend.json'

const DEPENDENCY_FOLDER_PREFIX = '.dependencies/'
const DEPENDENCY_FOLDER = DEPENDENCY_FOLDER_PREFIX.replace(/\/$/, '')
const CLIENT_FILE_PATH = '.client.json'
const MANIFEST_FILE_PATH = 'manifest.json'

// Files that should not be landed/published.
// Files that get a "3d model" icon.
const NORMAL_MODEL_FILES = ['glb', 'gltf']
// Files that get a "splat" icon.
const SPLAT_MODEL_FILES = ['spz', 'splat', 'ply']
// Files that get a "hologram person" icon.
const HOLOGRAM_MODEL_FILES = ['hcap', 'tvm']
// Model files that are not yet supported by the ModelPreview component.
const NO_PREVIEW_MODEL_FILES = ['tvm', 'splat', 'ply']
// Files that get an "audio" icon.
const AUDIO_FILES = ['m4a', 'mp3', 'wav', 'ogg', 'aac']
// Files that get a "css" icon.
const CSS_FILES = ['css', 'scss', 'sass']
// Files that get an "html" icon.
const HTML_FILES = ['html', 'vue']
// Files that get an "image" icon.
const IMAGE_FILES = ['jpg', 'jpeg', 'png', 'svg', 'ico', 'gif']
// Files that get an "image" icon but can't be rendered in an img tag.
const SPECIAL_IMAGE_FILES = ['hdr']
// Files that get a "JS" icon.
const RAW_JS_FILES = ['js', 'ts']
// Files that get a "JSON" icon.
const JSON_FILES = ['json']
// Files that get an "MD" icon.
const MD_FILES = ['md']
// Files that get a "React" icon.
const REACT_FILES = ['tsx', 'jsx']
// Files that get a "text" icon.
const TXT_FILES = ['txt']
// Files that get a "video" icon.
const VIDEO_FILES = ['mp4']
// Font files
const FONT_FILES = ['ttf', 'woff', 'woff2', 'otf', 'fnt', 'eot']
// Custom font files output from studio font conversion
const CUSTOM_FONT_FILES = ['font8']

// FILE_KIND actually maps to icon in file-list-icon.tsx
const FILE_KIND = {
  hologram: HOLOGRAM_MODEL_FILES,
  model: NORMAL_MODEL_FILES,
  splat: SPLAT_MODEL_FILES,
  audio: AUDIO_FILES,
  css: CSS_FILES,
  html: HTML_FILES,
  image: [].concat(IMAGE_FILES, SPECIAL_IMAGE_FILES),
  js: RAW_JS_FILES,
  json: JSON_FILES,
  md: MD_FILES,
  react: REACT_FILES,
  txt: TXT_FILES,
  video: VIDEO_FILES,
  font: [].concat(FONT_FILES, CUSTOM_FONT_FILES),
}

const JS_FILES = [].concat(RAW_JS_FILES, REACT_FILES)
// NOTE(dat): Make sure to check your file against the SPECIAL_TEXT_FILES list
const TEXT_FILES = [].concat(CSS_FILES, HTML_FILES, JS_FILES, JSON_FILES, MD_FILES, TXT_FILES)

const SPECIAL_TEXT_FILES = new Set(['LICENSE'])

const MAIN_FILES_ORDER = {
  'head.html': 0,
  'app.js': 1,
  'body.html': 2,
}

const isFolderPath = (filePath: string) => {
  if (SPECIAL_TEXT_FILES.has(filePath)) {
    return false
  }

  const fileBasename = basename(filePath)
  return !!fileBasename && !fileBasename.includes('.')
}

const isModuleMainPath = (filePath: string) => ['module.js', MANIFEST_FILE_PATH].includes(filePath)

const isAssetPath = (filePath: string) => !!(
  filePath && (filePath === ASSET_FOLDER || filePath.startsWith(ASSET_FOLDER_PREFIX))
)

const isBackendPath = (filePath: string) => !!(
  filePath && (filePath === BACKEND_FOLDER || filePath.startsWith(BACKEND_FOLDER_PREFIX)) &&
  filePath.endsWith(BACKEND_FILE_EXT)
)

const isDependencyPath = (filePath: string) => !!(
  filePath && (filePath === DEPENDENCY_FOLDER || filePath.startsWith(DEPENDENCY_FOLDER_PREFIX))
)

const isHiddenPath = (filePath: string) => !!(
  filePath && (filePath.startsWith('.'))
)

const isTextPath = (filePath: string, checkMainPath: (path: string) => boolean) => !(
  isAssetPath(filePath) || checkMainPath(filePath) || isBackendPath(filePath) ||
  isDependencyPath(filePath) || (!BuildIf.EXPERIMENTAL && filePath === CLIENT_FILE_PATH)
)

const sanitizeFilePath = (filePath: string) => {
  let sanitizedBaseName = basename(filePath)
  const directories = dirname(filePath)
  const ext = fileExt(filePath)
  if (ext) {
    sanitizedBaseName = sanitizedBaseName.replace(/\.[^/.]+$/, '')  // Remove extension.
  }

  sanitizedBaseName = sanitizedBaseName
    .replace(/\s+/g, '_')           // Replace spaces with underscore.
    .replace(/[^A-Z0-9_-]+/ig, '')  // Only allow alphanumeric, underscore, or dashes.

  return join(directories, joinExt(sanitizedBaseName, ext))
}

// Get the new path for a file that was dragged around the file tree.
const getDraggedFilePath = (filePath: string, folderPath: string = null) => {
  let folderDest = folderPath

  if (isAssetPath(filePath) && !folderDest) {
    // If the file is an asset, dragging to the "top level" should drag to the top level of the
    // assets folder instead.
    folderDest = ASSET_FOLDER
  }

  const name = basename(filePath)
  return folderDest ? join(folderDest, name) : name
}

// Change the name of a file while keeping it in the same enclosing folder.
const getRenamedFilePath = (newName: string, filePath: string) => {
  const pathComponents = filePath.split('/')
  pathComponents[pathComponents.length - 1] = newName
  return pathComponents.join('/')
}

const containsFolderEntry = (items: DataTransferItemList) => (
  items && Array.from(items).some(item => item.webkitGetAsEntry?.()?.isDirectory)
)

export {
  ASSET_FOLDER,
  ASSET_FOLDER_PREFIX,
  AUDIO_FILES,
  BACKEND_FILE_EXT,
  BACKEND_FOLDER,
  BACKEND_FOLDER_PREFIX,
  DEPENDENCY_FOLDER,
  FILE_KIND,
  IMAGE_FILES,
  JS_FILES,
  MAIN_FILES_ORDER,
  MANIFEST_FILE_PATH,
  NORMAL_MODEL_FILES,
  NO_PREVIEW_MODEL_FILES,
  SPECIAL_IMAGE_FILES,
  SPECIAL_TEXT_FILES,
  SPLAT_MODEL_FILES,
  TEXT_FILES,
  VIDEO_FILES,
  containsFolderEntry,
  getDraggedFilePath,
  getRenamedFilePath,
  isAssetPath,
  isBackendPath,
  isFolderPath,
  isHiddenPath,
  isModuleMainPath,
  isTextPath,
  sanitizeFilePath,
}
