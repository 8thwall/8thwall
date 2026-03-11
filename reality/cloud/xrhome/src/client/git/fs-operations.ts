import type {DeepReadonly as RO} from 'ts-essentials'

import type {IGit, IGitFile} from './g8-dto'
import {MODE_DIR_777, MODE_FILE_666} from '../common/file-mode'

type FsOperation = (old: RO<IGit>) => Promise<IGit['filesByPath']> | IGit['filesByPath']

const saveFiles = (files: {filePath: string; content: string}[]): FsOperation => (prev) => {
  const newFiles = {...prev.filesByPath}
  for (const {filePath, content} of files) {
    newFiles[filePath] = {
      content,
      filePath,
      mode: MODE_FILE_666,
      repoId: prev.repo.repoId,
      repositoryName: prev.repo.repositoryName,
      isDirectory: false,
      timestamp: new Date(),
    }
  }
  return newFiles
}

const deleteFiles = (filePaths: RO<string[]>): FsOperation => (prev) => {
  const newFiles = {...prev.filesByPath}
  for (const filePath of filePaths) {
    delete newFiles[filePath]
  }
  return newFiles
}

type FileTransform = (file: IGitFile) => string | Promise<string>
interface FileMutation {
  filePath: string
  transform: FileTransform
  generate?: () => string | Promise<string>  // Optional, but required if the file doesn't exist yet
  newPath?: string
}

const mutateFile = ({
  filePath, transform, generate, newPath,
}: FileMutation): FsOperation => async (prev) => {
  const prevFile = prev.filesByPath[filePath]

  const newContent = await (prevFile ? transform(prevFile) : generate!())

  const newFiles = {...prev.filesByPath}
  delete newFiles[filePath]
  if (newContent !== null) {
    const newFilePath = newPath || filePath
    newFiles[newFilePath] = {
      content: newContent,
      filePath: newFilePath,
      mode: MODE_FILE_666,
      repoId: prev.repo.repoId,
      repositoryName: prev.repo.repositoryName,
      isDirectory: false,
      timestamp: new Date(),
    }
  }
  return newFiles
}

const createFolder = (folderPath: string): FsOperation => (prev) => {
  const newFiles = {...prev.filesByPath}
  newFiles[folderPath] = {
    content: '',
    filePath: folderPath,
    mode: MODE_DIR_777,
    repoId: prev.repo.repoId,
    repositoryName: prev.repo.repositoryName,
    isDirectory: true,
    timestamp: new Date(),
  }
  return newFiles
}

const renameFile = (oldPath: string, newPath: string): FsOperation => (prev) => {
  const newFiles = {...prev.filesByPath}

  const prefix = `${oldPath}/`
  for (const [path, file] of Object.entries(prev.filesByPath)) {
    let updatedPath: string | undefined
    if (path === oldPath) {
      updatedPath = newPath
    } else if (path.startsWith(prefix)) {
      updatedPath = `${newPath}/${path.slice(prefix.length)}`
    }
    if (updatedPath) {
      delete newFiles[path]
      newFiles[updatedPath] = {
        ...file,
        filePath: updatedPath,
      }
    }
  }
  return newFiles
}

export {
  saveFiles,
  deleteFiles,
  createFolder,
  mutateFile,
  renameFile,
}

export type {
  FsOperation,
}
