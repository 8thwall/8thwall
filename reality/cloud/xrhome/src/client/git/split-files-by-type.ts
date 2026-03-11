import type {DeepReadonly} from 'ts-essentials'

import type {IGitFile} from './g8-dto'

interface ISplitFiles {
  folderPaths: string[]
  filePaths: string[]
}

const splitFilesByType = (
  fileData: Record<string, IGitFile>, paths: DeepReadonly<string[]>
): ISplitFiles => {
  const filePaths: string[] = []
  const folderPaths: string[] = []

  paths.forEach((path) => {
    if (fileData[path].isDirectory) {
      folderPaths.push(path)
    } else {
      filePaths.push(path)
    }
  })

  return {
    filePaths,
    folderPaths,
  }
}

export {
  ISplitFiles,
  splitFilesByType,
}
