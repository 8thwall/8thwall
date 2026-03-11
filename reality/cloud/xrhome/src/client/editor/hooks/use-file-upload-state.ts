import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useDispatch} from 'react-redux'

import {sanitizeFilePath} from '../../common/editor-files'
import useActions from '../../common/use-actions'
import gitUploadActions, {
  getFileType, getFolderDest,
} from '../../git/git-upload-actions'
import {makeRunQueue} from '../../../shared/run-queue'
import {errorAction} from '../../common/error-action'
import {useSelector} from '../../hooks'
import {EditorFileLocation, extractFilePath, extractRepoId} from '../editor-file-location'
import {getRepoState} from '../../git/repo-state'
import {useAssetConverter} from '../../studio/hooks/use-asset-converter'

type UploadState = {
  uploadTotalNumFiles: number
  uploadNumFilesUploaded: number
  uploadFiles: File[]
  uploadTotalBytes: number
  uploadBytesUploaded: number
  uploadErrorMessage: string
}

type StatePatch = (prev: DeepReadonly<UploadState>) => Partial<UploadState> | null

type ProgressHandle = (progress: number) => void

const useFileUploadState = (
  primaryRepoId: string, assetLimitOverrides: string, convertFont?: boolean
) => {
  const globalGit = useSelector(s => s.git)
  const dispatch = useDispatch()
  const uploadActions = useActions(gitUploadActions)
  const {convertAssets} = useAssetConverter()
  const fontStateRef = React.useRef<{
    [fileName: string]: {isLoading: boolean; ttfFile: File, repoId: string}
  }>({})

  const waitForGeneratingMtsdf = (filename: string) => new Promise<void>((resolve) => {
    const interval = setInterval(() => {
      if (!fontStateRef.current[filename]) {
        clearInterval(interval)
        resolve()
      }
    }, 1000)
  })

  const [fileUploadState, setUploadState] = React.useState({
    uploadErrorMessage: '',
    uploadTotalNumFiles: 0,  // Total # of files to be uploaded
    uploadNumFilesUploaded: 0,  // # of files have been uploaded
    uploadFiles: [],  // Files it's uploading
    uploadTotalBytes: 0,  // # of bytes to be uploaded
    uploadBytesUploaded: 0,  // # of bytes have been uploaded
  })

  const patchUploadState = (applyPatch: StatePatch) => {
    setUploadState((s) => {
      const update = applyPatch(s)
      if (!update) {
        return s
      } else {
        return {...s, ...update}
      }
    })
  }

  const uploadFile = async (
    file: File,
    folderLocation: EditorFileLocation = null,
    onProgress: ProgressHandle
  ) => {
    const filetype = getFileType(file.name)
    const folderDest = getFolderDest(filetype, extractFilePath(folderLocation))
    const sanitizedFilePath = sanitizeFilePath(file.name)
    const sanitizedTargetPath = folderDest
      ? `${folderDest}/${sanitizedFilePath}`
      : sanitizedFilePath

    const repoId = extractRepoId(folderLocation) || primaryRepoId
    const repo = getRepoState({git: globalGit}, repoId)?.repo
    if (!repo) {
      throw new Error('Cannot upload file to unknown repo')
    }
    await uploadActions.uploadFile(
      repo, repoId, file, filetype, sanitizedTargetPath, onProgress
    )
  }

  const uploadFiles = async (
    filesAsArray: File[],
    folderPath: EditorFileLocation = null
  ) => {
    const repoId = extractRepoId(folderPath) || primaryRepoId
    patchUploadState(s => ({
      uploadTotalNumFiles: s.uploadTotalNumFiles + filesAsArray.length,
      uploadTotalBytes: filesAsArray.reduce(
        (seq, file) => (seq + file.size),
        s.uploadTotalBytes
      ),
    }))

    // Convert file array to a promise chain in async/await
    try {
      const runQueue = makeRunQueue()
      await Promise.all(filesAsArray.map(file => runQueue.next(async () => {
        // Use percentage from progress callback to accumulate file upload size
        let currentPercentage = 0
        const onProgress = async (percentage: number) => {
          const percentDelta = (percentage - currentPercentage) / 100
          const newBytesUploaded = Math.floor(file.size * percentDelta)
          patchUploadState(s => ({
            uploadBytesUploaded: s.uploadBytesUploaded + newBytesUploaded,
          }))
          currentPercentage = percentage
        }

        patchUploadState(s => ({
          uploadFiles: [...s.uploadFiles, file],
        }))

        if (BuildIf.FONT8_CONVERSION_20260211 && file.name.endsWith('.ttf') && convertFont) {
          fontStateRef.current[file.name] = {isLoading: false, ttfFile: file, repoId}
          await convertAssets([file], 'fontToMtsdf')
          await waitForGeneratingMtsdf(file.name)
        } else {
          await uploadFile(file, folderPath, onProgress)
        }
        patchUploadState(s => ({
          uploadNumFilesUploaded: s.uploadNumFilesUploaded + 1,
        }))
      })))

      // Only clean up everything when all promise chain(s) are done

      // A new set of file uploads may have started after this current set, so don't reset
      // state while other uploads are still in progress.
      setUploadState((state) => {
        if (state.uploadTotalNumFiles === state.uploadNumFilesUploaded) {
          return {
            uploadTotalNumFiles: 0,
            uploadNumFilesUploaded: 0,
            uploadFiles: [],
            uploadTotalBytes: 0,
            uploadBytesUploaded: 0,
            uploadErrorMessage: '',
          }
        } else {
          return state
        }
      })
    } catch (err) {
      const errMsg: string = (err instanceof Error && err.message) || err.toString()
      dispatch(errorAction(errMsg))
      setUploadState({
        uploadTotalNumFiles: 0,
        uploadNumFilesUploaded: 0,
        uploadFiles: [],
        uploadTotalBytes: 0,
        uploadBytesUploaded: 0,
        uploadErrorMessage: errMsg,
      })
    }
  }

  return {fileUploadState, uploadFiles}
}

export {
  useFileUploadState,
}

export type {
  UploadState,
}
