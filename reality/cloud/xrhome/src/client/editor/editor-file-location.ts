import type {EditorRouteParams} from './editor-route'

type BaseFileLocation = string

type ScopedFileLocation = {
  filePath: string
  repoId?: string
}

type EditorFileLocation = BaseFileLocation | ScopedFileLocation

const extractFilePath = (location: EditorFileLocation) => (
  typeof location === 'object' ? location?.filePath || '' : location
)

const extractRepoId = (location: EditorFileLocation) => (
  typeof location === 'object' ? location?.repoId || null : null
)

// eslint-disable-next-line arrow-parens
const falsyCompare = <T>(left: T, right: T) => (!left && !right) || left === right

const editorFileLocationEqual = (left: EditorFileLocation, right: EditorFileLocation) => (
  falsyCompare(extractFilePath(left), extractFilePath(right)) &&
  falsyCompare(extractRepoId(left), extractRepoId(right))
)

const resolveEditorFileLocation = (
  parsedMatch: EditorRouteParams
): EditorFileLocation => parsedMatch || ''

const deriveEditorRouteParams = (
  location: EditorFileLocation
): EditorRouteParams => extractFilePath(location)

const extractScopedLocation = (location: EditorFileLocation): ScopedFileLocation => ({
  filePath: extractFilePath(location),
  repoId: extractRepoId(location),
})

// NOTE(christoph): If there's a location that includes the primary repo ID as the repoId,
// we need to remove it, because the canonical form of locations in the primary repo is unset.
// That way an unscoped file path like "my-file.ts" is equivalent to {filePath: "my-file.ts"}.
const stripPrimaryRepoId = (location: EditorFileLocation, primaryRepoId: string) => {
  const result = extractScopedLocation(location)
  if (result.repoId === primaryRepoId) {
    result.repoId = null
  }
  return result
}

const deriveLocationKey = (location: EditorFileLocation): string => {
  const {filePath, repoId} = extractScopedLocation(location)
  return repoId ? `.repos/${repoId}/${filePath}` : filePath
}

const deriveLocationFromKey = (key: string): EditorFileLocation => {
  const match = key?.match(/^.repos\/([^/]+)\/(.*)$/)
  if (match) {
    return {
      repoId: match[1],
      filePath: match[2],
    }
  } else {
    return key || ''
  }
}

export {
  deriveEditorRouteParams,
  extractFilePath,
  extractRepoId,
  editorFileLocationEqual,
  resolveEditorFileLocation,
  extractScopedLocation,
  stripPrimaryRepoId,
  deriveLocationKey,
  deriveLocationFromKey,
}

export type {
  EditorFileLocation,
  ScopedFileLocation,
}
