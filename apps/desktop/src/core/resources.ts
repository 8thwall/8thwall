import {app} from 'electron'
import path from 'path'

const RESOURCES_PATH = app.isPackaged
  ? process.resourcesPath
  : path.resolve(process.cwd(), 'build_package')

const NODE_MODULES_PATH = app.isPackaged
  ? path.resolve(process.resourcesPath, 'app.asar.unpacked', 'node_modules')
  : path.resolve(process.cwd(), 'node_modules')

const NEW_PROJECT_ZIP_PATH = path.join(RESOURCES_PATH, 'new-project.zip')

const PRELOAD_PATH = path.join(__dirname, 'preload.js')

const CLIENT_DIST_PATH = app.isPackaged
  // In packaged app, desktop-dist should be in Resources directory (outside asar)
  ? path.resolve(process.resourcesPath, 'desktop-dist')
  // In development, use the original path
  : path.resolve(process.cwd(), '../../reality/cloud/xrhome/desktop-dist')

export {
  RESOURCES_PATH,
  NODE_MODULES_PATH,
  NEW_PROJECT_ZIP_PATH,
  PRELOAD_PATH,
  CLIENT_DIST_PATH,
}
