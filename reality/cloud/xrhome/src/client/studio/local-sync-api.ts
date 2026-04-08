import type {RuntimeMetadata} from '@ecs/shared/runtime-version'

import type {
  InitializeResponse, FileSnapshotResponse,
  ListProjectsResponse,
  NewProjectLocationResponse,
  CanceledInitializeResponse,
} from '../../shared/desktop/local-sync-types'
import type {RequestInit} from '../common/public-api-fetch'
import {isAssetPath} from '../common/editor-files'
import {basename} from '../editor/editor-common'

const API = Build8.PLATFORM_TARGET === 'desktop' ? 'file-sync://' : 'https://0.0.0.0:9033'

type ApiFetchError = Error & {
  res?: Response
}

// eslint-disable-next-line arrow-parens
const fetchJson = async <T>(url: string, options?: RequestInit): Promise<T> => {
  const response = await fetch(url, options)
  if (!response.ok) {
    throw Object.assign(
      new Error(`fetch error status code: ${response.status}, ${response.statusText}`),
      {res: response}
    )
  }
  return response.json()
}

const initializeLocal = (appName: string, location: 'default' | 'prompt') => {
  const params = new URLSearchParams({
    appName,
    location,
  })
  return fetchJson<InitializeResponse>(`${API}/project/init-local?${params}`, {method: 'POST'})
}

const notifyProjectAccess = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,
  })
  return fetchJson<{}>(`${API}/project/recent?${params}`, {method: 'POST'})
}

const watchLocal = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })
  return fetchJson<{}>(`${API}/project/watch-local?${params}`, {
    method: 'POST',
    body: JSON.stringify({appKey}),
  })
}

const stopWatchLocal = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,
  })
  return fetchJson<{}>(`${API}/project/watch-local?${params}`, {
    method: 'DELETE',
    body: JSON.stringify({appKey}),
  })
}

const pushFile = async (
  appKey: string, path: string, content: RequestInit['body']
) => {
  const params = new URLSearchParams({
    appKey,
    path,
  })

  return fetchJson<{}>(`${API}/file?${params}`, {
    method: 'POST',
    body: content,
  })
}

const makeLocalAssetUrl = (appKey: string, path: string, version: string | undefined) => (
  `${API}/file/direct/${encodeURIComponent(appKey)}/${version || 0}/${path}`
)

const makeLocalFileUrl = (appKey: string, path: string, version: string | undefined) => {
  const params = new URLSearchParams({
    appKey,
    path,

  })
  if (version) {
    params.append('v', version)
  }
  return `${API}/file?${params}`
}

const pullSrcFile = async (appKey: string, path: string): Promise<string> => {
  if (isAssetPath(path) && basename(path) !== '.main') {
    throw new Error('Cannot pull asset files using pullSrcFile')
  }

  const res = await fetch(makeLocalFileUrl(appKey, path, undefined))

  if (!res.ok) {
    throw new Error(`fetch error status code: ${res.status}, ${res.statusText}`)
  }

  return res.text()
}

const pullAssetFile = async (appKey: string, path: string): Promise<Blob> => {
  if (!isAssetPath(path)) {
    throw new Error('Cannot pull non-assets files using pullAssetFile')
  }

  const res = await fetch(makeLocalFileUrl(appKey, path, undefined))

  if (!res.ok) {
    throw new Error(`fetch error status code: ${res.status}, ${res.statusText}`)
  }

  return res.blob()
}

const deleteLocalFile = (appKey: string, path: string) => {
  const params = new URLSearchParams({
    appKey,
    path,

  })
  return fetchJson<{}>(`${API}/file?${params}`, {method: 'DELETE'})
}

const getFileStateSnapshot = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })
  return fetchJson<FileSnapshotResponse>(`${API}/file/snapshot?${params}`)
}

const getFileMetadata = () => {
  // TODO
}

interface ProjectStatusResponse {
  buildUrl: string
  buildRemoteUrl: string
}

const getProjectStatus = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })
  return fetchJson<ProjectStatusResponse>(`${API}/project/project-status?${params}`)
}

const getFileHash = async (appKey: string, path: string): Promise<string> => {
  const params = new URLSearchParams({
    appKey,
    path,

  })

  const res = await fetch(`${API}/file/hash/sha256?${params}`)
  if (!res.ok) {
    throw new Error(`fetch error status code: ${res.status}, ${res.statusText}`)
  }
  const data = await res.json()
  if (typeof data.hash !== 'string') {
    throw new Error('Invalid response from file hash endpoint')
  }
  return data.hash
}

const showFile = (appKey: string, path: string) => {
  const params = new URLSearchParams({
    appKey,
    path,

  })
  return fetchJson<{}>(`${API}/file/show?${params}`, {method: 'POST'})
}

const openFile = (appKey: string, path: string) => {
  const params = new URLSearchParams({
    appKey,
    path,

  })
  return fetchJson<{}>(`${API}/file/open?${params}`, {method: 'POST'})
}

const listProjects = () => fetchJson<ListProjectsResponse>(
  `${API}/project/list`, {method: 'GET'}
)

const showProject = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<{}>(`${API}/project/reveal-in-finder?${params}`, {method: 'POST'})
}

const openProject = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<{}>(`${API}/project/open?${params}`, {method: 'POST'})
}

const deleteProject = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<{}>(
    `${API}/project/delete?${params}`, {method: 'DELETE'}
  )
}
const listFileDirectory = (appKey: string, path: string) => {
  const params = new URLSearchParams({
    appKey,
    path,

  })
  return fetchJson<{contents: string[]}>(`${API}/file/directory?${params}`)
}

const moveProject = (appKey: string, newLocation?: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  if (newLocation) {
    params.append('newLocation', newLocation)
  }

  return fetchJson<{}>(
    `${API}/project/move?${params}`, {method: 'PATCH'}
  )
}

const pickNewProjectLocation = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<NewProjectLocationResponse>(
    `${API}/project/pick-new-location?${params}`, {method: 'PATCH'}
  )
}

const openDiskLocation = () => {
  const params = new URLSearchParams({

  })
  return fetchJson<InitializeResponse | CanceledInitializeResponse>(
    `${API}/project/open-disk?${params}`, {method: 'POST'}
  )
}

const buildZip = async (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  const response = await fetch(`${API}/project/build?${params}`, {method: 'POST'})
  if (!response.ok) {
    throw new Error(`fetch error status code: ${response.status}, ${response.statusText}`)
  }

  return response.blob()
}

const migrateProject = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<{}>(
    `${API}/project/migrate?${params}`, {method: 'POST'}
  )
}

const getRuntimeMetadata = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<RuntimeMetadata>(
    `${API}/project/runtime-metadata?${params}`, {method: 'GET'}
  )
}

const renameFile = (appKey: string, oldPath: string, newPath: string) => {
  const params = new URLSearchParams({
    appKey,
    oldPath,
    newPath,

  })

  return fetchJson<{}>(`${API}/file/rename?${params}`, {method: 'POST'})
}

const checkConfigStatus = (appKey: string) => {
  const params = new URLSearchParams({
    appKey,

  })

  return fetchJson<{needsInjectFix: boolean}>(
    `${API}/project/config?${params}`, {method: 'GET'}
  )
}

const applyProjectConfigFix = (appKey: string, fix: 'inject') => {
  const params = new URLSearchParams({

    appKey,
    fix,
  })
  return fetchJson<{}>(`${API}/project/config?${params}`, {method: 'POST'})
}

export {
  initializeLocal,
  watchLocal,
  stopWatchLocal,
  getFileStateSnapshot,
  pushFile,
  pullSrcFile,
  pullAssetFile,
  getFileHash,
  deleteLocalFile,
  getFileMetadata,
  getProjectStatus,
  makeLocalFileUrl,
  showFile,
  openFile,
  listProjects,
  showProject,
  openProject,
  deleteProject,
  makeLocalAssetUrl,
  moveProject,
  pickNewProjectLocation,
  listFileDirectory,
  openDiskLocation,
  notifyProjectAccess,
  buildZip,
  migrateProject,
  getRuntimeMetadata,
  renameFile,
  checkConfigStatus,
  applyProjectConfigFix,
}

export type {
  ApiFetchError,
}
