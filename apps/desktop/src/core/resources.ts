import {app} from 'electron'
import path from 'path'

const RESOURCES_PATH = app.isPackaged
  ? process.resourcesPath
  : path.resolve(__dirname, '../..', 'build_package')

const NODE_MODULES_PATH = app.isPackaged
  ? path.resolve(process.resourcesPath, 'app.asar.unpacked', 'node_modules')
  : path.resolve(process.cwd(), 'node_modules')

const NEW_PROJECT_ZIP_PATH = path.join(RESOURCES_PATH, 'new-project.zip')

export {
  RESOURCES_PATH,
  NODE_MODULES_PATH,
  NEW_PROJECT_ZIP_PATH,
}
