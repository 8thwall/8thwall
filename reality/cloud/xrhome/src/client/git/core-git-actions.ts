import type {DeepReadonly} from 'ts-essentials'

import type {
  IGit,
  IGitFile,
  IRepo,
} from './g8-dto'
import {isAssetPath} from '../common/editor-files'
import type {Dispatch, Thunk} from '../common/types/actions'
import type {AsyncThunk} from '../common/types/actions'
import {dispatchify} from '../common'
import {withRunQueue} from './tab-synchronization'
import * as DirectFs from './fs-operations'
import {gitUpdateAction} from './direct-git-actions'
import {errorAction} from '../common/error-action'
import {getRepoState} from './repo-state'
import type {FsOperation} from './fs-operations'
import {MODE_FILE_666} from '../common/file-mode'
import * as LocalSyncApi from '../studio/local-sync-api'

const logErrorDetails = (
  repoId: string, msgstr: string, err?: Error
): Thunk<string> => (dispatch, getState) => {
  const sep = (err && '; ') || ''
  const errmsg = (err && err.message) || (err && JSON.stringify(err)) || ''
  const msg = `${msgstr}${sep}${errmsg}`
  // Add extra context to the message for logging.
  const repoState = getRepoState(getState, repoId)
  const {handle, repositoryName} = repoState?.repo || {}
  // @ts-expect-error TODO(christoph): Clean up
  const client = repoState?.clients?.find(c => c.active)?.name || 'noclient'
  const logmessage = `${msg} {${repositoryName || 'no.repo'}:${handle || 'nohandle'}-${client}}`
  if (err?.stack) {
    err.message = logmessage
    // eslint-disable-next-line no-console
    console.error(err)
  } else {
    // eslint-disable-next-line no-console
    console.error(logmessage)
  }
  return msg
}
// This displays the error banner on top of the screen
const showErr = (repoId: string, msgstr: string, err?: Error): Thunk => (dispatch) => {
  const msg = dispatch(logErrorDetails(repoId, msgstr, err))
  dispatch(errorAction(msg))
}

const getParent = (filePath: string): string | null => {
  const lastIndexOfSlash = filePath.lastIndexOf('/')
  if (lastIndexOfSlash === -1) {
    return null
  }
  return filePath.slice(0, lastIndexOfSlash)
}

const produceFiles = (
  filesByPath: Record<string, IGitFile>
): Partial<IGit> => {
  const files = Object.values(filesByPath).sort((a, b) => a.filePath.localeCompare(b.filePath))
  const filePaths: Set<string> = new Set(files.map(f => f.filePath))
  // update files, filePaths, childrenByPath, topLevelPaths
  const childrenByPath: {[dirPath: string]: string[]} = {}

  const topLevelPaths = new Set<string>()

  const addChild = (parentPath: string| null, childPath: string) => {
    if (parentPath === null) {
      topLevelPaths.add(childPath)
      return
    }
    if (!childrenByPath[parentPath]) {
      childrenByPath[parentPath] = []
    }
    if (!childrenByPath[parentPath].includes(childPath)) {
      childrenByPath[parentPath].push(childPath)
    }
  }

  const addDirectory = (filePath: string) => {
    const parent = getParent(filePath)
    if (!filePaths.has(filePath)) {
      filePaths.add(filePath)
      const directory: IGitFile = {
        content: '',
        filePath,
        mode: MODE_FILE_666,
        repoId: files[0]?.repoId || '',
        repositoryName: files[0]?.repositoryName || '',
        isDirectory: true,
        timestamp: new Date(),
      }
      files.push(directory)
      filesByPath[filePath] = directory
      const split = filePath.split('/')
      topLevelPaths.add(split[0])
      if (!childrenByPath[filePath]) {
        childrenByPath[filePath] = []
      }
      addChild(parent, directory.filePath)
    }
    if (parent) {
      addDirectory(parent)
    }
  }

  files.forEach(({filePath, isDirectory}) => {
    filePaths.add(filePath)
    const split = filePath.split('/')
    topLevelPaths.add(split[0])

    const parent = getParent(filePath)
    addChild(parent, filePath)
    if (parent) {
      addDirectory(parent)
    }

    if (isDirectory && !childrenByPath[filePath]) {
      childrenByPath[filePath] = []
    }
  })

  return {
    files,
    filesByPath,
    childrenByPath,
    filePaths: Array.from(filePaths),
    topLevelPaths: Array.from(topLevelPaths),
  }
}

const dispatchDirectAction = (repoId: string, action: FsOperation): AsyncThunk<void> => async (
  dispatch, getState
) => {
  const repoState = getRepoState(getState, repoId)
  const newFiles = await action(repoState)
  dispatch(gitUpdateAction(repoId, 'DIRECT_ACTION', produceFiles(newFiles)))
}

const listFiles = (repo: IRepo): AsyncThunk<readonly string[]> => async (_, getState) => {
  const repoState = getRepoState(getState, repo.repoId)
  return repoState.filePaths
}

const createFile = (
  repo: IRepo,
  filePath: string,
  content?: string
) => async (dispatch: Dispatch) => {
  await dispatch(dispatchDirectAction(repo.repoId, DirectFs.mutateFile({
    filePath,
    generate: () => content || '',
    transform: () => content || '',
  })))
}

const validateFileMove = (fileSrc: string, fileDest: string) => {
  if (isAssetPath(fileSrc) !== isAssetPath(fileDest)) {
    return 'Can\'t move in or out of assets.'
  }
  return null
}

const renameFile = (
  repo: IRepo | string,
  fileSrc: string,
  fileDest: string
): AsyncThunk => async (dispatch) => {
  if (fileSrc === fileDest) {
    return
  }
  const repoId = typeof repo === 'string' ? repo : repo.repoId

  const moveError = validateFileMove(fileSrc, fileDest)
  if (moveError) {
    dispatch(errorAction(moveError))
    throw new Error(moveError)
  }

  if (isAssetPath(fileSrc)) {
    await LocalSyncApi.renameFile(repoId, fileSrc, fileDest)
  } else {
    await dispatch(dispatchDirectAction(repoId, DirectFs.renameFile(fileSrc, fileDest)))
  }
}

const createFolder = (repo: IRepo, folderPath: string) => dispatch => (
  dispatch(dispatchDirectAction(repo.repoId, DirectFs.createFolder(folderPath)))
)

type FileSaveInfo = {
  filePath: string
  content: string
  timestamp?: number
}

const saveFiles = (repo: IRepo, files: FileSaveInfo[]) => (
  dispatchDirectAction(repo.repoId, DirectFs.saveFiles(files))
)

type FileTransform = (file: IGitFile) => string | Promise<string>
interface FileMutation {
  filePath: string
  transform: FileTransform
  generate?: () => string | Promise<string>  // Optional, but required if the file doesn't exist yet
  newPath?: string
}
const mutateFile = (repo: IRepo, {filePath, transform, generate, newPath}: FileMutation) => (
  dispatchDirectAction(repo.repoId, DirectFs.mutateFile({
    filePath,
    transform,
    generate,
    newPath,
  }))

)

const transformFile = (
  repo: IRepo, filePath: string, transform: FileTransform, ignoreNonexistent?: boolean
) => (mutateFile(repo, {
  filePath,
  transform,
  generate: () => {
    if (!ignoreNonexistent) {
      throw new Error(`Failed to transform ${filePath}`)
    }
    return null
  },
})
)

const deleteFile = (repo: IRepo, filePath: string) => (dispatch) => {
  if (!filePath) {
    dispatch(showErr(repo.repoId, 'You need to specify a file path', null))
    return Promise.resolve()
  }
  if (!repo.repositoryName) {
    dispatch(showErr(repo.repoId, 'You need to specify a repository name', null))
    return Promise.resolve()
  }
  return dispatch(dispatchDirectAction(repo.repoId, DirectFs.deleteFiles([filePath])))
}

const deleteFiles = (
  repo: IRepo,
  filePaths: DeepReadonly<Array<string>>
): AsyncThunk<void> => async (dispatch) => {
  dispatch(dispatchDirectAction(repo.repoId, DirectFs.deleteFiles(filePaths)))
}

const rawActions = {
  // Files
  mutateFile: withRunQueue(mutateFile),
  transformFile: withRunQueue(transformFile),
  saveFiles: withRunQueue(saveFiles),
  createFolder: withRunQueue(createFolder),
  renameFile: withRunQueue(renameFile),
  deleteFile: withRunQueue(deleteFile),
  deleteFiles: withRunQueue(deleteFiles),
  createFile: withRunQueue(createFile),
  listFiles: withRunQueue(listFiles),
}

const actions = dispatchify(rawActions)

export {
  showErr,
  createFile,
  actions as default,
}
