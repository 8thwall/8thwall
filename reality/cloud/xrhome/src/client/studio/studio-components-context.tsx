import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {StudioComponentError, StudioComponentMetadata} from '@ecs/shared/studio-component'

import type {IGitFile} from '../git/g8-dto'
import {JS_FILES} from '../common/editor-files'
import {fileExt} from '../editor/editor-common'
import {
  EditorFileLocation,
  deriveLocationFromKey,
  deriveLocationKey,
  editorFileLocationEqual,
  stripPrimaryRepoId,
} from '../editor/editor-file-location'
import * as StudioComponents from './studio-components-reducer'
import {getStudioComponents} from '../editor/tooling/analysis'
import {getBuiltinComponentMetadata} from './built-in-schema'
import {useCurrentRepoId} from '../git/repo-id-context'
import {useCurrentGit} from '../git/hooks/use-current-git'

interface IStudioComponentsContext {
  getComponentSchema: (name: string) => DeepReadonly<StudioComponentMetadata> |
  undefined
  listComponents: () => DeepReadonly<string[]>
  getComponentLocation: (name: string) => DeepReadonly<EditorFileLocation> | undefined
  getComponentsFromLocation: (location: EditorFileLocation) => DeepReadonly<string[]>
  errors: DeepReadonly<Record<string, StudioComponentError[]>>
}

const StudioComponentsContext = React.createContext<IStudioComponentsContext>(null)

const StudioComponentsContextProvider: React.FC<React.PropsWithChildren> = ({children}) => {
  const [{studioComponents, parsingFileCount, errors}, dispatch] = React.useReducer(
    StudioComponents.reducer,
    {
      studioComponents: {},
      parsingFileCount: 0,
      errors: {},
    }
  )

  const primaryRepoId = useCurrentRepoId()
  const gitFiles = useCurrentGit(git => git.files)
  const prevJsFileMapRef = React.useRef<Record<string, IGitFile>>({})
  const pendingFilesRef = React.useRef<Record<string, {
    fileContent: string, isParsing: boolean, parseAgain: boolean
  }>>({})

  const removeComponentsFromLocation = (locationKey: string) => {
    dispatch(StudioComponents.addComponents(locationKey, [], []))
  }

  const maybeAddComponentsFromContent = async (locationKey: string, fileContent: string) => {
    if (pendingFilesRef.current[locationKey]?.isParsing) {
      pendingFilesRef.current[locationKey].fileContent = fileContent
      pendingFilesRef.current[locationKey].parseAgain = true
      return
    }
    pendingFilesRef.current[locationKey] = {fileContent, isParsing: true, parseAgain: false}
    dispatch(StudioComponents.incrementParsingFileCount())
    const {componentData, errors: fileErrors} = await getStudioComponents(fileContent)
    dispatch(StudioComponents.decrementParsingFileCount())

    pendingFilesRef.current[locationKey].isParsing = false
    if (pendingFilesRef.current[locationKey].parseAgain) {
      pendingFilesRef.current[locationKey].parseAgain = false
      maybeAddComponentsFromContent(locationKey, pendingFilesRef.current[locationKey].fileContent)
      return
    }
    delete pendingFilesRef.current[locationKey]
    dispatch(StudioComponents.addComponents(locationKey, componentData, fileErrors))
  }

  React.useEffect(() => {
    const jsFileMap: Record<string, IGitFile> = {}

    gitFiles.forEach((file) => {
      if (JS_FILES.includes(fileExt(file.filePath))) {
        jsFileMap[deriveLocationKey({filePath: file.filePath, repoId: file.repoId})] = file
      }
    })

    // NOTE(johnny) Compare the previous state of the files to the current state to see if
    // anything changed.
    const allKeys = new Set([...Object.keys(jsFileMap), ...Object.keys(prevJsFileMapRef.current)])
    Array.from(allKeys).forEach((key) => {
      const contents = jsFileMap[key]?.content
      const prevContents = prevJsFileMapRef.current[key]?.content

      if ((contents !== undefined) !== (prevContents !== undefined)) {
        // NOTE(johnny): The file either got added or removed.
        if (contents === undefined) {
          // NOTE(johnny): Remove the components from that file.
          removeComponentsFromLocation(key)
        } else {
          // NOTE(johnny): Add the components from that file.
          maybeAddComponentsFromContent(key, contents)
        }
      } else if (contents !== prevContents) {
        // NOTE(johnny): The file got updated. Remove the old components and add the new ones.
        maybeAddComponentsFromContent(key, contents)
      }
    })
    prevJsFileMapRef.current = jsFileMap
  }, [gitFiles])

  const value = {
    getComponentSchema: (name: string) => (
      getBuiltinComponentMetadata(name) || studioComponents[name]?.metadata
    ),
    listComponents: () => Object.keys(studioComponents),
    getComponentLocation: (name: string) => {
      const locationKey = studioComponents[name]?.locationKey
      return locationKey
        ? stripPrimaryRepoId(deriveLocationFromKey(locationKey), primaryRepoId)
        : undefined
    },
    getComponentsFromLocation: (location: EditorFileLocation) => (
      Object.keys(studioComponents).filter(c => editorFileLocationEqual(
        deriveLocationFromKey(studioComponents[c].locationKey), location
      ))
    ),
    isLoading: parsingFileCount > 0,
    errors,
  }

  return (
    <StudioComponentsContext.Provider value={value}>
      {children}
    </StudioComponentsContext.Provider>
  )
}

const useStudioComponentsContext = () => React.useContext(StudioComponentsContext)

export {
  StudioComponentsContextProvider,
  useStudioComponentsContext,
}

export type {
  IStudioComponentsContext,
}
