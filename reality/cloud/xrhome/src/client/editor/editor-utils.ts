import {dirname, basename} from 'path'

import {ASSET_FOLDER_PREFIX, FILE_KIND} from '../common/editor-files'

const RICH_SUFFIX = ';rich-preview'
const NEW_TAB_PATH = ''

const LEFT_PANE_EXPANDED_SIZE = 213

interface IFile {
  filePath: string
  shortFilePath?: string
}

type FileReference = {
  fileIndex: number
  filePath: string
  fileSubPaths?: string[]
}

const updateToShortFilePaths = (files: IFile[]) => {
  // Create file path map
  const filePathMap = new Map<string, FileReference[]>()
  files.forEach((file: IFile, i: number) => {
    const fileName = basename(file.filePath)
    const f: FileReference = {
      fileIndex: i,
      filePath: file.filePath,
    }
    if (filePathMap.has(fileName)) {
      filePathMap.get(fileName).push(f)
    } else {
      filePathMap.set(fileName, [f])
    }
  })

  // Create short file paths
  const ellipsisPath = '.../'

  Array.from(filePathMap.entries()).forEach(([fileName, fileArr]) => {
    if (fileArr.length === 1) {
      const nameInfo = files[fileArr[0].fileIndex]
      nameInfo.shortFilePath = nameInfo.shortFilePath || fileName
      return
    }

    // Only get sub paths for files with the same name
    fileArr.forEach((file) => { file.fileSubPaths = dirname(file.filePath).split('/').reverse() })
    /**
     * Using reverse order to find the first different sub path
     * Only show that sub path and use ellipsis for the rest of them:
     *
     * Loop#(excl. name): 0↓  1↓  2↓  3↓
     * file1 = config.js / d / b / a / root
     * file2 = config.js / d / c / root
     *
     * with final short file paths:
     * file1 = .../b/.../config.js
     * file2 = .../c/.../config.js
     */
    fileArr.forEach((file, i) => {
      const nameInfo = files[file.fileIndex]
      if (nameInfo.shortFilePath) {
        return
      }

      for (let j = 0; j < file.fileSubPaths.length; j++) {
        let uniqueSubPath = true
        for (let k = 0; k < fileArr.length; k++) {
          const anotherFile = fileArr[k]
          if (i !== k && j < anotherFile.fileSubPaths.length &&
            file.fileSubPaths[j] === anotherFile.fileSubPaths[j]) {
            uniqueSubPath = false
            break
          }
        }
        if (uniqueSubPath) {
          nameInfo.shortFilePath =
            `${j !== file.fileSubPaths.length - 1 ? ellipsisPath : ''}${file.fileSubPaths[j]}/` +
            `${j !== 0 ? ellipsisPath : ''}${fileName}`
          break
        }
      }
    })
  })
}

const isMarkdownPath = (filePath: string) => (
  filePath && FILE_KIND.md.some(suffix => filePath.endsWith(suffix))
)

const isAceFilepath = (filePath: string) => (
  filePath !== NEW_TAB_PATH &&
  !filePath.startsWith(ASSET_FOLDER_PREFIX)
)

const aceSessionKey = (filepath: string) => (filepath || '')

// Parse hash fragment and return line number and column number.
const parseHashFragment = (hash) => {
  const found = hash.match(/^#L([0-9]+)C?([0-9]+)?$/)
  if (found) {
    const [, line, column] = found
    return {line: parseInt(line, 10) || undefined, column: parseInt(column, 10) || undefined}
  }
  return {line: undefined, column: undefined}
}

export {
  NEW_TAB_PATH,
  LEFT_PANE_EXPANDED_SIZE,
  RICH_SUFFIX,

  updateToShortFilePaths,
  isMarkdownPath,
  isAceFilepath,
  aceSessionKey,
  parseHashFragment,
}
