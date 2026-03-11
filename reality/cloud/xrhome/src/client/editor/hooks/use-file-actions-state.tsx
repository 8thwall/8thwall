import React, {useRef} from 'react'

import {GlobalHotKeys} from 'react-hotkeys'

import {
  extractRepoId, type EditorFileLocation,
  extractFilePath, stripPrimaryRepoId,
} from '../editor-file-location'
import type {PersistentEditorSession} from './use-persistent-editor-session'
import {getRepoState} from '../../git/repo-state'
import {useCurrentGit} from '../../git/hooks/use-current-git'
import {useSelector} from '../../hooks'
import {useTabActions} from './use-tab-actions'
import coreGitActions from '../../git/core-git-actions'
import useActions from '../../common/use-actions'
import {
  containsFolderEntry, getDraggedFilePath, getRenamedFilePath,
} from '../../common/editor-files'
import type {IFileActionsContext, NewItem} from '../files/file-actions-context'
import appsActionsProvider from '../../apps/apps-actions'
import {FOLDER_UPLOAD_ERROR} from '../editor-errors'
import {useFileUploadState} from './use-file-upload-state'
import DeleteFileModal from '../modals/delete-file-modal'
import {useCurrentRepoId} from '../../git/repo-id-context'
import FbxToGlbModal from '../modals/fbx-to-glb-modal'

type BeforeFileSelectHandler = (
  file: EditorFileLocation,
) => boolean

type FileRenameHandler = (
  oldLocation: EditorFileLocation,
  newLocation?: EditorFileLocation
) => void

interface IFileActionsState {
  editorSession: PersistentEditorSession
  checkProtectedFile: (filePath: string) => boolean
  onBeforeFileSelect?: BeforeFileSelectHandler
  onFileRename?: FileRenameHandler
}

const useFileActionsState = ({
  editorSession, checkProtectedFile, onBeforeFileSelect, onFileRename,
}: IFileActionsState) => {
  const git = useCurrentGit()
  const {repo} = git
  const {error: showError} = useActions(appsActionsProvider)
  const globalGit = useSelector(s => s.git)
  const repoId = useCurrentRepoId()
  const {
    createFolder, createFile,
    deleteFile, renameFile: renameFileAction,
  } = useActions(coreGitActions)

  const repoStateForFile = (fileLocation: EditorFileLocation) => (
    getRepoState({git: globalGit}, extractRepoId(fileLocation) || repoId)
  )

  const {
    closeTabById, closeTab, switchTab, updateTab, handleSwitchTabs,
  } = useTabActions(editorSession)

  /* ==================== FBX to GLB modal ==================== */
  const [showFbxToGlbModal, setShowFbxToGlbModal] = React.useState(false)
  const [fbxFilesToConvert, setFbxFilesToConvert] = React.useState<File[]>([])

  const handleCancelFbxToGlb = () => {
    setShowFbxToGlbModal(false)
    setFbxFilesToConvert([])
  }

  const maybeHandleFbxToGlb = (files: File[]) => {
    if (!BuildIf.FBX_CONVERSION_20260211) {
      return files
    }
    const fbxFiles = files.filter(file => file.name.endsWith('.fbx'))
    const restFiles = files.filter(file => !file.name.endsWith('.fbx'))
    if (fbxFiles.length === 0) {
      return files
    }
    setFbxFilesToConvert(fbxFiles)
    setShowFbxToGlbModal(true)
    return restFiles
  }

  /* ==================== file actions ==================== */
  const [deletingFile, setDeletingFile] = React.useState<EditorFileLocation>('')

  const renameFile = async (newName: string, prevLocation_: EditorFileLocation) => {
    const prevLocation = stripPrimaryRepoId(prevLocation_, repoId)

    const newPath = getRenamedFilePath(newName, extractFilePath(prevLocation))
    const fileRepo = repoStateForFile(prevLocation)?.repo
    if (!fileRepo) {
      // eslint-disable-next-line no-console
      console.error('Cannot perform rename with missing repo for', prevLocation)
      return
    }
    const newLocation = {...prevLocation, filePath: newPath}
    await renameFileAction(fileRepo, extractFilePath(prevLocation), newPath)

    updateTab(prevLocation, newLocation)
    onFileRename?.(prevLocation, newLocation)
  }

  const createItem = async (newItem: NewItem, folderLocation_?: EditorFileLocation) => {
    const folderLocation = stripPrimaryRepoId(folderLocation_, repoId)

    const folderPath = extractFilePath(folderLocation)
    const itemPath = folderPath ? `${folderPath}/${newItem.name}` : newItem.name
    const fileRepo = repoStateForFile(folderLocation)?.repo
    if (!fileRepo) {
      // eslint-disable-next-line no-console
      console.error('Cannot create file with missing repo for', folderPath)
      return
    }
    if (newItem.isFile) {
      await createFile(fileRepo, itemPath, newItem.fileContent || '')
      const newLocation = {repoId: extractRepoId(folderLocation), filePath: itemPath}
      switchTab(newLocation)
    } else {
      createFolder(fileRepo, itemPath)
    }
  }

  /* handle upload files */
  const uploadTargetLocation = useRef<EditorFileLocation>(null)
  const uploadDropRef = useRef<HTMLInputElement>(null)

  const {fileUploadState, uploadFiles} = useFileUploadState(
    repo.repoId,
    null,
    true
  )

  const handleUploadStart = (folderPath?: EditorFileLocation) => {
    uploadTargetLocation.current = folderPath
    uploadDropRef.current?.click()
  }

  const handleFileUpload = (e: React.ChangeEvent<HTMLInputElement>) => {
    const filesAsArray: File[] = Array.from(e.target.files)
    const restFiles = maybeHandleFbxToGlb(filesAsArray)
    uploadFiles(restFiles, uploadTargetLocation.current)
  }

  const moveFileToFolder = async (
    fileLocation: EditorFileLocation, folderLocation: EditorFileLocation
  ) => {
    const prevLocation = stripPrimaryRepoId(fileLocation, repoId)

    const filePath = extractFilePath(fileLocation)
    const folderPath = extractFilePath(folderLocation)
    const newPath = getDraggedFilePath(filePath, folderPath)

    const newLocation = stripPrimaryRepoId(
      {repoId: extractRepoId(folderLocation), filePath: newPath},
      repoId
    )

    await renameFileAction(repoId, extractFilePath(prevLocation), newPath)

    updateTab(prevLocation, newLocation)
    onFileRename?.(prevLocation, newLocation)
  }

  // Note (cindyhu): If file paths include a folder and its children files, we only want to
  // perform file actions on the folder
  const reduceToTopLevelFiles = (filePaths: string[]): string[] => {
    const sorted = [...filePaths].sort((a, b) => a.length - b.length)
    const result: string[] = []
    sorted.forEach((path: string) => {
      if (!result.some(parent => path !== parent && path.startsWith(`${parent}/`))) {
        result.push(path)
      }
    })
    return result
  }

  // e is a drop event that may contain files or the filePath of an existing file to move
  // folderPath is the target folder's path
  const handleDropEvent = (e: React.DragEvent, folderLocation: EditorFileLocation = null) => {
    const filePaths = e.dataTransfer.getData('filePaths')
    if (filePaths) {
      const topLevelFiles = reduceToTopLevelFiles(JSON.parse(filePaths))
      topLevelFiles.forEach((path: string) => {
        moveFileToFolder({filePath: path, repoId: e.dataTransfer.getData('repoId')}, folderLocation)
      })
      return
    }

    const filePath = e.dataTransfer.getData('filePath')
    if (filePath) {
      moveFileToFolder({filePath, repoId: e.dataTransfer.getData('repoId')}, folderLocation)
      return
    }

    if (containsFolderEntry(e.dataTransfer.items)) {
      showError(FOLDER_UPLOAD_ERROR)
      return
    }

    if (e.dataTransfer.files?.length > 0) {
      const filesAsArray: File[] = Array.from(e.dataTransfer.files)
      const restFiles = maybeHandleFbxToGlb(filesAsArray)
      // TODO(christoph): Handle upload within module
      uploadFiles(restFiles, folderLocation)
    }
  }
  /* end of handle upload files */

  const actionsContext: IFileActionsContext = {
    onCreate: createItem,
    onDrop: handleDropEvent,
    onSelect: (loc, options, paneId) => {
      const shouldSkip = onBeforeFileSelect?.(loc)
      if (shouldSkip) {
        return
      }
      switchTab(loc, options, paneId)
    },
    onUploadStart: handleUploadStart,
    onDelete: (loc: EditorFileLocation) => setDeletingFile(loc),
    onRename: renameFile,
    isProtectedFile: checkProtectedFile,
    onClose: closeTabById,
  }
  /*  ==================== end of file actions ==================== */

  return {
    actionsContext,
    fileUploadState,
    handleFileUpload,
    uploadDropRef,
    fileActionModals:
  <>
    {/* TODO(Dale): Refactor to react-hotkeys-hook */}
    <GlobalHotKeys
      keyMap={{
        tabRight: ['alt+=', 'alt+='],
        tabLeft: ['alt+-', 'alt+-'],
      }}
      handlers={{
        tabRight: e => handleSwitchTabs(e, 'right'),
        tabLeft: e => handleSwitchTabs(e, 'left'),
      }}
    />
    {showFbxToGlbModal &&
      <FbxToGlbModal
        files={fbxFilesToConvert}
        onClose={handleCancelFbxToGlb}
      />
    }
    {!!deletingFile &&
      <DeleteFileModal
        confirmSwitch={async () => {
          const fileRepo = repoStateForFile(deletingFile)?.repo
          if (!fileRepo) {
            // eslint-disable-next-line no-console
            console.error('Cannot perform delete with missing repo for', deletingFile)
            return
          }
          closeTab(stripPrimaryRepoId(deletingFile, repo.repoId))
          await deleteFile(fileRepo, extractFilePath(deletingFile))
          setDeletingFile('')
        }}
        rejectSwitch={() => setDeletingFile('')}
        file={extractFilePath(deletingFile)}
      />
    }
  </>,
  }
}

export {
  useFileActionsState,
}

export type {
  BeforeFileSelectHandler,
  FileRenameHandler,
}
