import {ISplitFiles, splitFilesByType} from '../split-files-by-type'
import {useCurrentGit} from './use-current-git'

const useFolderChildren = (folderPath: string) => (
  useCurrentGit((git): ISplitFiles => (
    splitFilesByType(git.filesByPath, git.childrenByPath[folderPath])
  ))
)

export {
  useFolderChildren,
}
