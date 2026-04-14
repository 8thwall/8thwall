import {shell} from 'electron'
import {createWriteStream, Stats} from 'fs'
import {mkdir, rm, rename, readdir, stat} from 'fs/promises'
import path from 'path'
import {Readable} from 'stream'
import {createHash} from 'crypto'
import mime from 'mime-types'
import type {FileSnapshotResponse} from '@repo/reality/shared/desktop/local-sync-types'
import {toUnixPath} from '@repo/reality/shared/desktop/unix-path'

import {
  FilePushParams, FilePullParams, FileDeleteParams, FileStateParams,
  FileRenameParams,
} from './file-handler-types'
import {
  MAX_TEXT_FILE_UPLOAD_IN_BYTES,
} from '../../constants'
import {getProjectSrcPath, isIgnoredFile, isAssetPath} from '../../project-helpers'
import {getLocalProject} from '../../local-project-db'
import {makeCodedError, withErrorHandlingResponse} from '../../errors'
import {
  FILE_PATH, FILE_STATE_SNAPSHOT_PATH, FILE_METADATA_PATH, FILE_HASH_SHA256_PATH,
  FILE_SHOW_PATH, FILE_OPEN_PATH, FILE_DIRECTORY_PATH,
  FILE_RENAME_PATH,
} from './paths'
import {makeJsonResponse} from '../../json-response'
import {getQueryParams} from '../../query-params'
import {openInCodeEditor} from '../preferences/code-editor'
import {createReadStreamPromise, makeStreamFileResponse} from '../../stream-file-response'

const getLocalFile = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)

  if (!isAssetPath(toUnixPath(filePath))) {
    try {
      const stats = await stat(fullPath)
      const sizeLimit = MAX_TEXT_FILE_UPLOAD_IN_BYTES

      if (stats.size > sizeLimit) {
        return new Response('File size exceeds limit', {status: 413})
      }
    } catch (error) {
      if (error instanceof Error) {
        if ('code' in error && error.code === 'ENOENT') {
          throw makeCodedError('File not found', 404)
        }
      }
      throw makeCodedError('Error reading file data', 500)
    }
  }

  return makeStreamFileResponse(fullPath)
})

const headLocalFile = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)

  let stats: Stats
  try {
    stats = await stat(fullPath)
  } catch (error) {
    if (error instanceof Error) {
      if ('code' in error && error.code === 'ENOENT') {
        throw makeCodedError('File not found', 404)
      }
    }
    throw makeCodedError('Error reading file data', 500)
  }

  return new Response('', {
    headers: {
      'Content-Type': mime.lookup(filePath) || 'application/octet-stream',
      'Content-Length': stats.size.toString(),
    },
  })
})

const saveFile = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FilePushParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }

  const {body} = req

  if (typeof body === 'undefined' || body === null) {
    throw makeCodedError('Missing request body', 400)
  }

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)
  const dirPath = path.dirname(fullPath)
  await mkdir(dirPath, {recursive: true})

  const res = await new Promise<{}>((resolve, reject) => {
    const stream = createWriteStream(fullPath)
    stream.on('finish', () => {
      resolve({})
    })
    stream.on('error', (error) => {
      // eslint-disable-next-line no-console
      console.error('Error writing file:', error)
      reject(makeCodedError('Error writing file', 500))
    })
    Readable.fromWeb(body).pipe(stream)
  })

  return makeJsonResponse(res)
})

const deleteFile = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FileDeleteParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)
  await rm(fullPath, {recursive: true, force: true})
  return makeJsonResponse({})
})

const getFileStateSnapshot = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FileStateParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey} = params.data

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const contents = await readdir(projectPath, {recursive: true})

  const response: FileSnapshotResponse = {
    timestampsByPath: {},
  }

  await Promise.all(contents.map(async (filePath) => {
    const unixPath = toUnixPath(filePath)
    if (isIgnoredFile(unixPath)) {
      return
    }
    const fileInfo = await stat(path.join(projectPath, filePath))
    if (!fileInfo.isFile()) {
      return
    }
    response.timestampsByPath[unixPath] = fileInfo.mtimeMs
  }))

  return makeJsonResponse(response)
})

const getFileHashSha256 = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }
  const {appKey, path: filePath} = params.data
  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }
  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }
  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)
  const contentStream = await createReadStreamPromise(fullPath)
  const hasher = createHash('sha256')
  const hash = await new Promise<string>((resolve, reject) => {
    contentStream.on('data', chunk => hasher.update(chunk))
    contentStream.on('end', () => resolve(hasher.digest('hex')))
    contentStream.on('error', (error) => {
      if ('code' in error && error.code === 'ENOENT') {
        reject(makeCodedError('File not found', 404))
      } else {
        reject(error)
      }
    })
  })
  return makeJsonResponse({hash})
})

// TODO
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const getFileMetadata = withErrorHandlingResponse(async (req: Request) => makeJsonResponse({}))

const handleFileOperationRequest = (request: Request) => {
  switch (request.method) {
    case 'HEAD':
      return headLocalFile(request)
    case 'GET':
      return getLocalFile(request)
    case 'POST':
      return saveFile(request)
    case 'DELETE':
      return deleteFile(request)
    default:
      return new Response('Method Not Allowed', {status: 405})
  }
}

const postShowFile = withErrorHandlingResponse(async (req) => {
  const requestUrl = new URL(req.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)

  try {
    const info = await stat(fullPath)
    if (info.isDirectory()) {
      const error = await shell.openPath(fullPath)
      if (error) {
        throw makeCodedError(`Error opening directory: ${error}`, 500)
      }
    } else {
      shell.showItemInFolder(fullPath)
    }
    return makeJsonResponse({})
  } catch (error) {
    if (error instanceof Error) {
      if ('code' in error && error.code === 'ENOENT') {
        throw makeCodedError('File not found', 404)
      }
    }
    throw error
  }
})

const handleFileShowRequest = (request: Request) => {
  switch (request.method) {
    case 'POST':
      return postShowFile(request)
    default:
      return new Response('Method Not Allowed', {status: 405})
  }
}

const postOpenFile = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data
  if (isIgnoredFile(filePath)) {
    throw makeCodedError('Invalid file', 400)
  }

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)

  try {
    await openInCodeEditor(project.location, fullPath)
  } catch (err) {
    throw makeCodedError(`Error opening file: ${err}`, 500)
  }

  return makeJsonResponse({})
})

const handleFileOpenRequest = (request: Request) => {
  switch (request.method) {
    case 'POST':
      return postOpenFile(request)
    default:
      return new Response('Method Not Allowed', {status: 405})
  }
}

const handleFileSnapshotRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getFileStateSnapshot(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const handleFileMetadataRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getFileMetadata(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const handleFileHashSha256Request = (request: Request) => {
  if (request.method === 'GET') {
    return getFileHashSha256(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

// NOTE(christoph): Asset bundles use relative routing so we need a way to directly address files
// without query params.
const handleDirectFileOperationRequest = (request: Request) => {
  const requestUrl = new URL(request.url)
  const [
    encodedAppKey, /* cache bust */, ...pathParts
  ] = requestUrl.pathname.split('/').slice(3)  // skip leading slash, 'file' and 'direct'
  requestUrl.searchParams.set('appKey', decodeURIComponent(encodedAppKey))
  requestUrl.searchParams.set('path', pathParts.map(e => decodeURIComponent(e)).join('/'))
  requestUrl.pathname = FILE_PATH
  return handleFileOperationRequest(new Request(requestUrl.toString(), {
    method: request.method,
    body: request.body,
    headers: request.headers,
  }))
}

const getFileDirectory = withErrorHandlingResponse(async (request) => {
  const requestUrl = new URL(request.url)
  const params = FilePullParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey, path: filePath} = params.data

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullPath = path.join(projectPath, filePath)
  const contents = await readdir(fullPath, {recursive: true})
  return makeJsonResponse({
    contents: contents.filter(f => !isIgnoredFile(f)).map(toUnixPath),
  })
})

const handleFileDirectoryRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getFileDirectory(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const renameFile = withErrorHandlingResponse(async (request) => {
  const requestUrl = new URL(request.url)
  const params = FileRenameParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }
  const {appKey, oldPath, newPath} = params.data

  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }

  const projectPath = getProjectSrcPath(project.location)
  const fullOldPath = path.join(projectPath, oldPath)
  const fullNewPath = path.join(projectPath, newPath)

  try {
    await mkdir(path.dirname(fullNewPath), {recursive: true})
    await rename(fullOldPath, fullNewPath)
  } catch (error) {
    if (error instanceof Error) {
      if ('code' in error && error.code === 'ENOENT') {
        throw makeCodedError('File not found', 404)
      }
    }
    throw error
  }
  return makeJsonResponse({})
})

const handleFileRenameRequest = (request: Request) => {
  if (request.method === 'POST') {
    return renameFile(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

const handleFileRequest = (request: Request) => {
  const requestUrl = new URL(request.url)
  const {pathname} = requestUrl

  if (pathname.startsWith('/file/direct/')) {
    return handleDirectFileOperationRequest(request)
  }

  switch (pathname) {
    case FILE_PATH:
      return handleFileOperationRequest(request)
    case FILE_STATE_SNAPSHOT_PATH:
      return handleFileSnapshotRequest(request)
    case FILE_DIRECTORY_PATH:
      return handleFileDirectoryRequest(request)
    case FILE_METADATA_PATH:
      return handleFileMetadataRequest(request)
    case FILE_HASH_SHA256_PATH:
      return handleFileHashSha256Request(request)
    case FILE_OPEN_PATH:
      return handleFileOpenRequest(request)
    case FILE_SHOW_PATH:
      return handleFileShowRequest(request)
    case FILE_RENAME_PATH:
      return handleFileRenameRequest(request)
    default: {
      // eslint-disable-next-line no-console
      console.error('Unknown file sync request:', pathname)
      return new Response('Not Found', {status: 404})
    }
  }
}

export {
  handleFileRequest,
}
