// The starting state of a freshly mounted BrowsableMultitabEditorView
// NOTE(pawel) This is intended to be a temporary stepping stone in breaking up the component logic.
// It's the most straightforward way of bringing all the state into the functional component.
import {RICH_SUFFIX} from './editor-utils'
import {
  EditorFileLocation, extractFilePath, extractRepoId, ScopedFileLocation,
} from './editor-file-location'
import type {GitReduxState} from '../git/git-reducer'
import {getRepoState} from '../git/repo-state'

// The following functions are used by BrowsableMultitabEditorView as calculations and not for
// interactivity purposes.

const getFileInGit = (git: GitReduxState, primaryRepoId: string, location: EditorFileLocation) => {
  const filePath = extractFilePath(location)
  if (!filePath) {
    return null
  }
  const repoId = extractRepoId(location) || primaryRepoId
  return getRepoState({git}, repoId).filesByPath?.[filePath]
}

const removeRichSuffix = (filePath: string) => {
  if (filePath && filePath.endsWith(RICH_SUFFIX)) {
    return filePath.slice(0, -RICH_SUFFIX.length)
  }
  return filePath
}

const removeRichSuffixScoped = (location: EditorFileLocation): ScopedFileLocation => ({
  filePath: removeRichSuffix(extractFilePath(location)),
  repoId: extractRepoId(location),
})

const fileExistsInGit = (
  git: GitReduxState, primaryRepoId: string, location: EditorFileLocation, subRepoIds: Set<string>
) => {
  const repoId = extractRepoId(location)
  return (!repoId || subRepoIds?.has(repoId)) &&
  !!getFileInGit(git, primaryRepoId, removeRichSuffixScoped(location))
}

export {
  removeRichSuffix,
  removeRichSuffixScoped,
  fileExistsInGit,
}
