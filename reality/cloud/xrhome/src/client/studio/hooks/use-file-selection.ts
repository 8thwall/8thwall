import type React from 'react'

import {useCurrentGit} from '../../git/hooks/use-current-git'
import {ISplitFiles, splitFilesByType} from '../../git/split-files-by-type'
import {STUDIO_HIDDEN_FILE_PATHS} from '../common/studio-files'
import {useStudioStateContext} from '../studio-state-context'
import {isAssetPath, isFolderPath} from '../../common/editor-files'

const useFileSelection = (paths: ISplitFiles) => {
  const stateCtx = useStudioStateContext()
  const {selectedFiles, lastSelectedFileIndex} = stateCtx.state
  const allFilesByPath = useCurrentGit(git => git.filesByPath)
  const allChildrenByPath = useCurrentGit(git => git.childrenByPath)

  // Note(cindyhu): <FileBrowserList /> renders folders first then files.
  const getAllFiles = (currentPaths: ISplitFiles): string[] => {
    let result: string[] = []
    currentPaths.folderPaths.forEach((folderPath) => {
      const subPaths = splitFilesByType(allFilesByPath, allChildrenByPath[folderPath])
      result = [...result, folderPath, ...getAllFiles(subPaths)]
    })
    result = [
      ...result,
      ...currentPaths.filePaths.filter(filePath => !STUDIO_HIDDEN_FILE_PATHS.includes(filePath)),
    ]
    return result
  }

  const onFileClick = (filePath: string, event: React.MouseEvent) => {
    const allFiles = getAllFiles(paths)
    const index = allFiles.indexOf(filePath)

    if (event.shiftKey) {
      const hasPrevSelectionInSameSection = lastSelectedFileIndex !== null &&
      selectedFiles.length > 0 && (isAssetPath(selectedFiles[0]) === isAssetPath(filePath))
      if (hasPrevSelectionInSameSection) {
        const start = Math.min(lastSelectedFileIndex, index)
        const end = Math.max(lastSelectedFileIndex, index)
        const newSelection = allFiles.slice(start, end + 1)
        stateCtx.update(p => ({...p, selectedFiles: newSelection}))
      } else {
        stateCtx.update(p => ({...p, selectedFiles: [filePath], lastSelectedFileIndex: index}))
      }
    } else {
      if (!isFolderPath(filePath)) {
        stateCtx.update(p => ({...p, selectedFiles: [filePath]}))
      }
      stateCtx.update(p => ({...p, lastSelectedFileIndex: null}))
    }
  }

  return {
    onFileClick,
  }
}

export {useFileSelection}
