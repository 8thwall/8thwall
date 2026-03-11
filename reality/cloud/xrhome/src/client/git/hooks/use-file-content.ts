import {
  extractFilePath, extractRepoId, type EditorFileLocation,
} from '../../editor/editor-file-location'
import {useCurrentRepoId} from '../repo-id-context'
import {useScopedGitFile} from './use-current-git'

const useFileContent = (fileLocation: EditorFileLocation) => {
  const primaryRepoId = useCurrentRepoId()
  const activeFilePath = extractFilePath(fileLocation)
  const repoId = extractRepoId(fileLocation) || primaryRepoId
  return useScopedGitFile(repoId, activeFilePath)?.content
}

export {
  useFileContent,
}
