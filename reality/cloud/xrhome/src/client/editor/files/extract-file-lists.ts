import type {DeepReadonly} from 'ts-essentials'

import {
  ASSET_FOLDER, isTextPath, MAIN_FILES_ORDER, isModuleMainPath, DEPENDENCY_FOLDER,
  MANIFEST_FILE_PATH, BACKEND_FOLDER,
} from '../../common/editor-files'
import type {IGit} from '../../git/g8-dto'
import {ISplitFiles, splitFilesByType} from '../../git/split-files-by-type'

const filterMainFiles = (
  filePaths: DeepReadonly<string[]>, isMainPath: (path: string) => boolean
) => (
  filePaths
    .filter(isMainPath)
    .sort((a, b) => (MAIN_FILES_ORDER[a] - MAIN_FILES_ORDER[b]))
)

interface IProjectFileLists {
  main: ISplitFiles
  text: ISplitFiles
  assets: ISplitFiles
  dependencies?: ISplitFiles
  backends?: ISplitFiles
}

const extractProjectFileLists = (
  git: DeepReadonly<IGit>, isMainPath: (path: string) => boolean
): IProjectFileLists => {
  const {filesByPath, topLevelPaths, childrenByPath} = git

  const mainPaths = filterMainFiles(topLevelPaths, isMainPath)
  const dependenciesPaths = childrenByPath[DEPENDENCY_FOLDER] || []
  const textPaths = topLevelPaths.filter(path => isTextPath(path, isMainPath))
  const assetPaths = childrenByPath[ASSET_FOLDER] || []

  return {
    main: splitFilesByType(filesByPath, mainPaths),
    text: splitFilesByType(filesByPath, textPaths),
    assets: splitFilesByType(filesByPath, assetPaths),
    dependencies: splitFilesByType(filesByPath, dependenciesPaths),
  }
}

const extractModuleFileLists = (
  git: DeepReadonly<IGit>, includeManifest = true
): IProjectFileLists => {
  const {filesByPath, topLevelPaths, childrenByPath} = git

  const mainPaths: string[] = []
  const textPaths: string[] = []

  topLevelPaths.forEach((path) => {
    if (path === MANIFEST_FILE_PATH) {
      if (includeManifest) {
        mainPaths.push(path)
      }
    } else if (isModuleMainPath(path)) {
      mainPaths.push(path)
    } else if (path !== ASSET_FOLDER && path !== BACKEND_FOLDER) {
      textPaths.push(path)
    }
  })

  const assetPaths = childrenByPath[ASSET_FOLDER] || []
  const backendsPaths = childrenByPath[BACKEND_FOLDER] || []

  return {
    main: splitFilesByType(filesByPath, mainPaths),
    text: splitFilesByType(filesByPath, textPaths),
    assets: splitFilesByType(filesByPath, assetPaths),
    backends: splitFilesByType(filesByPath, backendsPaths),
  }
}

export {
  IProjectFileLists,
  extractProjectFileLists,
  extractModuleFileLists,
}
