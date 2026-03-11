// Checks is filePath is a valid member of containingPath.
// A path is a valid path member of itself.
import {
  EditorFileLocation, extractFilePath, extractRepoId, ScopedFileLocation,
} from '../editor-file-location'

// e.g. me/boo/foo/too.js is a valid path in me/boo
const matchPath = (filePath: string, containingPath: string) => {
  if (!filePath) {
    return false
  }
  const filePathComponents = filePath.split('/')
  const matchingFilePathComponents = containingPath.split('/')
  for (let i = 0; i < matchingFilePathComponents.length; i++) {
    if (filePathComponents[i] !== matchingFilePathComponents[i]) {
      return false
    }
  }
  return true
}

// This updates the full path or partial path prefix.
const updatePath = (filePath: string, matchingFilePath: string, replacingFilePath: string) => (
  filePath && filePath.replace(new RegExp(`^${matchingFilePath}`), replacingFilePath)
)

// This replaces full path or partial path prefix if found.
const replacePath = (matchingFilePath: string, replacingFilePath: string) => (filePath: string) => (
  matchPath(filePath, matchingFilePath)
    ? updatePath(filePath, matchingFilePath, replacingFilePath)
    : filePath
)

const matchLocation = (file: EditorFileLocation, location: EditorFileLocation) => {
  if (extractRepoId(file) === extractRepoId(location)) {
    return matchPath(extractFilePath(file), extractFilePath(location))
  }
  return false
}

const replaceLocation = <T extends ScopedFileLocation>(
  // eslint-disable-next-line arrow-parens
  file: T, oldLocation: EditorFileLocation, newLocation: EditorFileLocation
): T => {
  if (matchLocation(file, oldLocation)) {
    const oldPath = extractFilePath(oldLocation)
    const newPath = extractFilePath(newLocation)

    const replacedLocation = {
      ...file,
      filePath: updatePath(file.filePath, oldPath, newPath),
      repoId: extractRepoId(newLocation) || undefined,
    }

    const newRepoId = extractRepoId(newLocation)
    if (newRepoId) {
      replacedLocation.repoId = newRepoId
    } else {
      delete replacedLocation.repoId
    }
    return replacedLocation
  }
  return file
}

export {
  matchPath,
  updatePath,
  replacePath,
  matchLocation,
  replaceLocation,
}
