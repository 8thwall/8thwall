import {dispatchify} from '../common/index'
import type {IRepo} from './g8-dto'
import {
  ASSET_FOLDER, isAssetPath, SPECIAL_TEXT_FILES, TEXT_FILES,
} from '../common/editor-files'
import {fileExt} from '../editor/editor-common'
import {withRunQueue} from './tab-synchronization'
import type {AsyncThunk} from '../common/types/actions'
import {showErr, createFile} from './core-git-actions'
import {pushFile} from '../studio/local-sync-api'

enum UploadFileType {
  Text = 'text',
  Asset = 'asset',
}

const getFileType = (fileName: string): UploadFileType => {
  const fileext = fileExt(fileName)
  return TEXT_FILES.includes(fileext) || SPECIAL_TEXT_FILES.has(fileName)
    ? UploadFileType.Text
    : UploadFileType.Asset
}

const getFolderDest = (filetype: UploadFileType, folderPath: string) => {
  const isTextFile = filetype === 'text'
  const isTextFolder = !isAssetPath(folderPath)

  // Upload files to the correct place if they are dragged into an invalid target
  let folderDest
  if (isTextFile === isTextFolder) {
    folderDest = folderPath
  } else if (!isTextFile) {
    folderDest = ASSET_FOLDER
  }
  return folderDest
}

type ProgressCallback = (percent: number) => void

const readTextBlob = (
  file: Blob, onProgress?: ProgressCallback
) => new Promise<string>((resolve) => {
  const r = new FileReader()
  r.onload = () => {
    if (typeof r.result !== 'string') {
      throw new Error('Expected file reader result to be a string')
    } else {
      resolve(r.result)
    }
  }
  r.onprogress = (e) => {
    if (e.lengthComputable && onProgress) {
      onProgress?.((e.loaded / e.total) * 100)
    }
  }
  r.readAsText(file)
})

const uploadFile = (
  repo: IRepo,
  repoId: string,
  file: Blob,
  filetype: UploadFileType,
  targetPath: string,
  onProgress?: ProgressCallback
): AsyncThunk<Blob> => async (dispatch) => {
  // If this is just a text file, we don't actually need to upload it. Just add the file and its
  // contents to the list of git files.
  // 20% reading file progress + 20% reading file processing + 60% creating file in IndexedDB
  if (filetype === UploadFileType.Text) {
    try {
      const content = await readTextBlob(file, percent => onProgress?.(percent * 0.2))
      onProgress?.(40)
      await dispatch(createFile(repo, targetPath, content))
      onProgress?.(100)
      return file
    } catch (err) {
      dispatch(showErr(repo.repoId, `Couldn't create ${targetPath}`, err))
      throw err
    }
  }

  // 80% progress uploading file + 10% multer processing/s3.putObject + 10% create file in IndexedDB
  try {
    await pushFile(repoId, targetPath, file)
    onProgress?.(100)
    return file
  } catch (err) {
    dispatch(showErr(repo.repoId, 'Couldn\'t upload file', err))
    throw err
  }
}

// NOTE(johnny): repo needs to be added to all the functions because syncRepoStateFromDisk needs
// repo. We are unable to extract repo from the args before passing it into the functions
// since uploadFile needs repo.

const rawActions = {
  uploadFile: withRunQueue(uploadFile),
}

export default dispatchify(rawActions)

export {
  UploadFileType,
  getFileType,
  getFolderDest,
}
