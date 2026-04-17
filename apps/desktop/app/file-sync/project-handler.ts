import {dialog, shell, app} from 'electron'
import os from 'os'
import path from 'path'
import fs from 'fs/promises'
import log from 'electron-log'
import archiver from 'archiver'
import crypto from 'crypto'

import {makeRunQueue} from '@repo/reality/shared/run-queue'
import type {SceneGraph} from '@repo/c8/ecs/src/shared/scene-graph'
import type {RuntimeMetadata} from '@repo/c8/ecs/src/shared/runtime-version'

import type {InitializeResponse, Project} from '@repo/reality/shared/desktop/local-sync-types'

import {
  FixConfigParams,
  InitializeProjectParams, MoveProjectParams, ProjectRequestParams,
} from './project-handler-types'
import {
  upsertLocalProject, getLocalProject,
  getLocalProjects, deleteLocalProject as deleteLocalProjectEntry,
  bumpProjectAccessedAt,
  getLocalProjectByLocation,
} from '../../local-project-db'
import {makeCodedError, withErrorHandlingResponse} from '../../errors'
import {
  PROJECT_INIT_PATH, PROJECT_LIST_PATH, PROJECT_DELETE_PATH, PROJECT_REVEAL_IN_FINDER_PATH,
  PROJECT_STATUS_PATH, PROJECT_WATCH_PATH,
  PROJECT_PICK_NEW_LOCATION_PATH,
  PROJECT_MOVE_PATH,
  PROJECT_OPEN_PATH,
  PROJECT_OPEN_DISK_PATH,
  PROJECT_RECENT_PATH,
  PROJECT_BUILD_PATH,
  PROJECT_MIGRATE_PATH,
  PROJECT_RUNTIME_METADATA_PATH,
  PROJECT_CONFIG_PATH,
} from './paths'
import {makeJsonResponse} from '../../json-response'
import {getQueryParams} from '../../query-params'
import {projectSetup, unzipIntoFolder} from './create-project-files'
import {getProjectSrcPath} from '../../project-helpers'
import {createLocalServer, LocalServer} from '../../local-server'
import {openInCodeEditor} from '../preferences/code-editor'
import {runBuildCommand, runInstallCommand} from './run-commands'

// eslint-disable-next-line max-len
const RUNTIME_1_BUNDLE_URL = 'https://cdn.8thwall.com/web/offline-code-export/studio/runtime-1.1.0-standalone-mkhjh3i4.zip'

const locationPrompt = async (): Promise<string | undefined> => {
  const res = await dialog.showOpenDialog({
    properties: ['openDirectory', 'createDirectory'],
  })
  return res.canceled ? undefined : res.filePaths[0]
}

const localServerRunQueue = makeRunQueue()
const appKeyToLocalServerManager: Map<string, LocalServer> = new Map()

const recordLocalProject = (projectPath: string, initialization: Project['initialization']) => {
  const existingProject = getLocalProjectByLocation(projectPath)
  const appKey = existingProject?.appKey || crypto.randomUUID()
  upsertLocalProject(appKey, projectPath, initialization)

  const response: InitializeResponse = {
    projectPath,
    appKey,
    initialization,
    canceled: false,
  }
  return makeJsonResponse(response)
}

const getLocalProjectLocation = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = InitializeProjectParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  let outerFolder: string
  if (params.data.location === 'default') {
    outerFolder = path.join(os.homedir(), 'Documents', app.getName())
  } else {
    const selectedFolder = await locationPrompt()
    if (!selectedFolder) {
      throw makeCodedError('Failed to retrieve location to save', 404)
    }
    outerFolder = selectedFolder
  }

  const savePath = path.join(outerFolder, params.data.appName)

  let exists = false
  try {
    await fs.access(savePath)
    exists = true
  } catch (error: any) {
    if (error.code !== 'ENOENT') {
      throw makeCodedError(`Failed to access path: ${savePath}`, 500)
    }
  }

  if (exists) {
    const contents = await fs.readdir(savePath)
    if (contents.length > 0) {
      throw makeCodedError(`The provided path already exists and is not empty: ${savePath}`, 409)
    }
  }

  await projectSetup(savePath)

  return recordLocalProject(savePath, 'v2')
})

const openDiskLocation = withErrorHandlingResponse(async () => {
  const projectPath = await locationPrompt()

  if (!projectPath) {
    return makeJsonResponse({canceled: true})
  }

  let isValid = false
  try {
    isValid = (await fs.stat(path.join(projectPath, 'src/.expanse.json'))).isFile()
  } catch (error: any) {
    if (error.code === 'ENOENT') {
      let containsPackageJsonAndReadme = false
      try {
        await fs.stat(path.join(projectPath, 'package.json'))
        await fs.stat(path.join(projectPath, 'README.md'))
        containsPackageJsonAndReadme = true
      } catch (err) {
        // Ignore
      }
      return makeJsonResponse({
        message: `The provided path does not contain an expanse file: ${projectPath}`,
        containsPackageJsonAndReadme,
      }, 409)
    }
    throw makeCodedError(`Failed to access folder: ${projectPath}: ${error.message}`, 500)
  }

  if (!isValid) {
    throw makeCodedError(`The provided path does not contain a valid project: ${projectPath}`, 409)
  }

  let isMigrated = false
  try {
    isMigrated = (await fs.stat(path.join(projectPath, 'external/runtime'))).isDirectory()
  } catch (error: any) {
    // Not migrated
  }

  return recordLocalProject(projectPath, isMigrated ? 'v2' : 'done')
})

const startWatch = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  const {appKey} = params.data
  if (!appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const projectEntry = getLocalProject(appKey)
  if (!projectEntry) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  return localServerRunQueue.next(async () => {
    const serverManager = appKeyToLocalServerManager.get(appKey)

    if (serverManager) {
      const isRunning = await serverManager.checkRunning()
      if (isRunning) {
        return makeJsonResponse({})
      } else {
        serverManager.stop()
      }
    }

    try {
      const newManager = await createLocalServer(projectEntry.location)
      appKeyToLocalServerManager.set(appKey, newManager)
      const running = await newManager.checkRunning()
      if (!running) {
        throw new Error('Failed to start local server')
      }
      return makeJsonResponse({})
    } catch (error: any) {
      log.info(`Error starting local server: ${error}`)
      throw makeCodedError(`Failed to start watch server: ${error.message}`, 500)
    }
  })
})

const stopWatch = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  return localServerRunQueue.next(async () => {
    const manager = appKeyToLocalServerManager.get(params.data.appKey)
    appKeyToLocalServerManager.delete(params.data.appKey)
    if (manager) {
      await manager.stop()
    }
    return makeJsonResponse({})
  })
})

const getProjectStatus = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const projectEntry = getLocalProject(params.data.appKey)
  if (!projectEntry) {
    throw makeCodedError('Project for appKey not found', 404)
  }
  const serverManager = appKeyToLocalServerManager.get(params.data.appKey)
  const buildUrl = await serverManager?.getLocalBuildUrl()
  const buildRemoteUrl = await serverManager?.getLocalBuildRemoteUrl()
  return makeJsonResponse({buildUrl, buildRemoteUrl})
})

const handleProjectInitRequest = (request: Request) => {
  if (request.method === 'POST') {
    return getLocalProjectLocation(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const handleProjectWatchRequest = (request: Request) => {
  if (request.method === 'POST') {
    return startWatch(request)
  } else if (request.method === 'DELETE') {
    return stopWatch(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const buildZip = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  await runInstallCommand(project.location)
  await runBuildCommand(project.location)

  const distPath = path.join(project.location, 'dist')
  try {
    const stat = await fs.stat(distPath)
    if (!stat.isDirectory()) {
      throw makeCodedError('dist folder does not exist or is not a directory', 404)
    }
  } catch (err: any) {
    if (err.code === 'ENOENT') {
      throw makeCodedError('dist folder does not exist', 404)
    }
    throw makeCodedError(`Failed to access dist folder: ${err.message}`, 500)
  }

  const archive = archiver('zip', {zlib: {level: 9}})

  // Pipe archive data to the PassThrough stream
  archive.directory(distPath, false)

  const response = new Response(archive, {
    status: 200,
    headers: {
      'Content-Type': 'application/zip',
    },
  })
  archive.finalize()

  return response
})

const handleProjectBuildRequest = (request: Request) => {
  if (request.method === 'POST') {
    return buildZip(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

const handleProjectStatusRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getProjectStatus(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const isValidProject = async (location: string) => {
  try {
    const srcPath = path.join(location, 'src')
    const stats = await fs.stat(srcPath)
    return stats.isDirectory()
  } catch {
    return false
  }
}

const getAllProjects = withErrorHandlingResponse(async () => {
  const projects = getLocalProjects().filter(p => p.initialization !== 'needs-initialization')
  const projectEntries = await Promise.all(projects.map(async ({appKey, ...rest}) => {
    const isValid = await isValidProject(rest.location)
    return [appKey, {...rest, validLocation: isValid}]
  }))
  const projectByAppKey = Object.fromEntries(projectEntries)

  return makeJsonResponse({projectByAppKey})
})

const handleProjectListRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getAllProjects(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const postRevealProject = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  const projectSrcPath = getProjectSrcPath(project.location)

  try {
    const info = await fs.stat(projectSrcPath)
    if (!info.isDirectory()) {
      throw makeCodedError('Project is not a directory', 400)
    } else {
      shell.openPath(projectSrcPath)
    }
    return makeJsonResponse({})
  } catch (error) {
    if (error instanceof Error) {
      if ('code' in error && error.code === 'ENOENT') {
        throw makeCodedError('Project not found', 404)
      }
    }
    throw error
  }
})

const handleProjectRevealRequest = (request: Request) => {
  if (request.method === 'POST') {
    return postRevealProject(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const postOpenProject = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  try {
    const info = await fs.stat(project.location)
    if (!info.isDirectory()) {
      throw makeCodedError('Project is not a directory', 400)
    } else {
      await openInCodeEditor(project.location)
    }
    return makeJsonResponse({})
  } catch (error) {
    if (error instanceof Error) {
      if ('code' in error && error.code === 'ENOENT') {
        throw makeCodedError('Project not found', 404)
      }
    }
    throw error
  }
})

const handleProjectOpenRequest = (request: Request) => {
  if (request.method === 'POST') {
    return postOpenProject(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const deleteLocalProject = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  deleteLocalProjectEntry(params.data.appKey)

  return makeJsonResponse({})
})

const handleProjectDeleteRequest = (request: Request) => {
  if (request.method === 'DELETE') {
    return deleteLocalProject(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const pickNewProjectLocation = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const projectEntry = getLocalProject(params.data.appKey)
  if (!projectEntry) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  const dialogResult = await dialog.showOpenDialog({
    properties: ['openDirectory'],
    defaultPath: projectEntry.location,
  })

  if (dialogResult.canceled) {
    throw makeCodedError('Failed to retrieve new location', 404)
  }

  const newLocation = dialogResult.filePaths[0]
  const projectFolderName = path.basename(projectEntry.location)
  const newProjectLocationPath = path.join(newLocation, projectFolderName)

  if (newProjectLocationPath === projectEntry.location) {
    throw makeCodedError('The selected location is the same as the current project location', 400)
  }

  return makeJsonResponse({projectPath: newProjectLocationPath})
})

const handleProjectPickNewLocationRequest = (request: Request) => {
  if (request.method === 'PATCH') {
    return pickNewProjectLocation(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const isValidNewLocation = async (newLocation: string) => {
  try {
    const stat = await fs.stat(newLocation)
    // NOTE(johnny): If newLocation is a directory and not empty, throw error
    if (!stat.isDirectory()) {
      throw makeCodedError(`The provided path is not a directory: ${newLocation}`, 409)
    }
    const contents = await fs.readdir(newLocation)
    // Filter out common hidden files that shouldn't prevent folder use
    const visibleContents = contents.filter(item => !item.startsWith('.') || item === '.gitkeep')
    if (visibleContents.length > 0) {
      const itemsList = visibleContents.join(', ')
      const message = `The provided path already exists and is not empty: ${newLocation} ` +
          `(contains: ${itemsList})`
      throw makeCodedError(message, 409)
    }
  } catch (error: any) {
    if (error.code === 'ENOENT') {
      // Path doesn't exist - this is valid, we can create it
      return
    }
    // Re-throw any other errors from our validation above
    if (error.message && error.message.includes('The provided path')) {
      throw error
    }
    throw makeCodedError(`Failed to access path: ${newLocation}: ${error.message}`, 500)
  }
}

const changeProjectLocation = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = MoveProjectParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const projectEntry = getLocalProject(params.data.appKey)
  if (!projectEntry) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  let {newLocation} = params.data

  if (!newLocation) {
    newLocation = await locationPrompt()
    if (!newLocation) {
      throw makeCodedError('Failed to retrieve new location', 404)
    }
  }

  const isCurrentLocationValid = projectEntry.location &&
  await isValidProject(projectEntry.location)

  // NOTE(johnny): If the project is invalid we are in the "locate" flow.
  if (!isCurrentLocationValid || projectEntry.initialization === 'needs-initialization') {
    throw makeCodedError('Current project location is invalid', 400)
  } else {  // NOTE(johnny):  We are in the "Change disk location" flow.
    await isValidNewLocation(newLocation)

    try {
      await fs.rename(projectEntry.location, newLocation)
    } catch (error: any) {
      throw makeCodedError(`Failed to move project files: ${error.message}`, 500)
    }

    upsertLocalProject(params.data.appKey, newLocation, projectEntry.initialization)
  }

  return makeJsonResponse({})
})

const handleProjectMoveRequest = (request: Request) => {
  if (request.method === 'PATCH') {
    return changeProjectLocation(request)
  }

  return new Response('Method Not Allowed', {status: 405})
}

const handleOpenDiskRequest = (request: Request) => {
  if (request.method === 'POST') {
    return openDiskLocation(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

const handleRecentProjectPost = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  bumpProjectAccessedAt(params.data.appKey)

  return makeJsonResponse({})
})

const handleRecentProjectRequest = async (request: Request) => {
  switch (request.method) {
    case 'POST':
      return handleRecentProjectPost(request)
    default:
      return new Response('Method Not Allowed', {status: 405})
  }
}

const handleProjectMigratePost = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  if (project.initialization !== 'done') {
    throw makeCodedError('Only projects with "done" status can be migrated', 400)
  }

  // TODO(christoph): Pull in 1.x.x runtime if specified in .expanse.json
  let useRuntimeVersion1 = false
  try {
    const expanseConfigPath = path.join(project.location, 'src', '.expanse.json')
    const expanseConfigContent = await fs.readFile(expanseConfigPath, 'utf-8')
    const expanseConfig: SceneGraph = JSON.parse(expanseConfigContent)
    if (!expanseConfig.runtimeVersion || expanseConfig.runtimeVersion.major === 1) {
      useRuntimeVersion1 = true
    }
  } catch (error) {
    log.warn(`Failed to read .expanse.json for project ${project.appKey}: ${error}`)
  }

  await projectSetup(project.location, (filePath) => {
    if (filePath === 'src/index.html') {
      return true
    }
    if (filePath.startsWith('src/')) {
      return false
    }
    if (useRuntimeVersion1 && filePath.startsWith('external/runtime/')) {
      return false  // Install separately
    }
    return true
  })

  if (useRuntimeVersion1) {
    const zipFetch = await fetch(RUNTIME_1_BUNDLE_URL)
    if (!zipFetch.ok) {
      throw new Error(
        `Failed to download runtime bundle: ${zipFetch.status} ${zipFetch.statusText}`
      )
    }
    const zipBuffer = await zipFetch.arrayBuffer()
    await unzipIntoFolder(path.join(project.location, 'external/runtime'), zipBuffer)
  }

  const foldersToDelete = ['.gen']

  await Promise.all(foldersToDelete.map(folder => (
    fs.rm(path.join(project.location, folder), {recursive: true, force: true})
  )))

  upsertLocalProject(project.appKey, project.location, 'v2')

  return makeJsonResponse({})
})

const handleProjectMigrateRequest = async (request: Request) => {
  switch (request.method) {
    case 'POST':
      return handleProjectMigratePost(request)
    default:
      return new Response('Method Not Allowed', {status: 405})
  }
}

const getRuntimeMetadata = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }

  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }
  const runtimePath = path.join(project.location, 'external/runtime/metadata.json')
  try {
    const metadataContent = await fs.readFile(runtimePath, 'utf-8')
    const metadata: RuntimeMetadata = JSON.parse(metadataContent)
    return makeJsonResponse(metadata)
  } catch (error: any) {
    if (error.code === 'ENOENT') {
      throw makeCodedError('Runtime metadata not found', 404)
    }
    throw error
  }
})

const handleProjectRuntimeMetadataRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getRuntimeMetadata(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

const BAD_INJECT_CONFIG = `new HtmlWebpackPlugin({
      template: path.join(srcPath, 'index.html'),
      filename: 'index.html',
      scriptLoading: 'blocking',
    })`

const GOOD_INJECT_CONFIG = `new HtmlWebpackPlugin({
      template: path.join(srcPath, 'index.html'),
      filename: 'index.html',
      scriptLoading: 'blocking',
      inject: false,
    })`

const WEBPACK_CONFIG_PATH = 'config/webpack.config.js'

const getProjectConfig = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = ProjectRequestParams.safeParse(getQueryParams(requestUrl))

  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }

  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }
  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }

  let needsInjectFix = false
  try {
    const configPath = path.join(project.location, WEBPACK_CONFIG_PATH)
    const configContent = await fs.readFile(configPath, 'utf-8')
    needsInjectFix = configContent.includes(BAD_INJECT_CONFIG)
  } catch (error) {
    // Ignore
  }

  return makeJsonResponse({needsInjectFix})
})

const modifyProjectConfig = withErrorHandlingResponse(async (req: Request) => {
  const requestUrl = new URL(req.url)
  const params = FixConfigParams.safeParse(getQueryParams(requestUrl))
  if (!params.success) {
    throw makeCodedError('Invalid query params', 400)
  }
  if (!params.data.appKey) {
    throw makeCodedError('Missing appKey', 400)
  }
  const project = getLocalProject(params.data.appKey)
  if (!project) {
    throw makeCodedError('Project for appKey not found', 404)
  }
  switch (params.data.fix) {
    case 'inject': {
      const configPath = path.join(project.location, WEBPACK_CONFIG_PATH)
      const configContent = await fs.readFile(configPath, 'utf-8')
      const fixedContent = configContent.replace(BAD_INJECT_CONFIG, GOOD_INJECT_CONFIG)
      await fs.writeFile(configPath, fixedContent, 'utf-8')
      break
    }
    default:
      throw makeCodedError('Unknown config fix type', 400)
  }
  return makeJsonResponse({})
})

const handleProjectConfigRequest = (request: Request) => {
  if (request.method === 'GET') {
    return getProjectConfig(request)
  } else if (request.method === 'POST') {
    return modifyProjectConfig(request)
  }
  return new Response('Method Not Allowed', {status: 405})
}

const handleProjectRequest = (request: Request) => {
  const requestUrl = new URL(request.url)
  const {pathname} = requestUrl

  switch (pathname) {
    case PROJECT_INIT_PATH:
      return handleProjectInitRequest(request)
    case PROJECT_WATCH_PATH:
      return handleProjectWatchRequest(request)
    case PROJECT_BUILD_PATH:
      return handleProjectBuildRequest(request)
    case PROJECT_STATUS_PATH:
      return handleProjectStatusRequest(request)
    case PROJECT_LIST_PATH:
      return handleProjectListRequest(request)
    case PROJECT_REVEAL_IN_FINDER_PATH:
      return handleProjectRevealRequest(request)
    case PROJECT_OPEN_PATH:
      return handleProjectOpenRequest(request)
    case PROJECT_DELETE_PATH:
      return handleProjectDeleteRequest(request)
    case PROJECT_PICK_NEW_LOCATION_PATH:
      return handleProjectPickNewLocationRequest(request)
    case PROJECT_MOVE_PATH:
      return handleProjectMoveRequest(request)
    case PROJECT_OPEN_DISK_PATH:
      return handleOpenDiskRequest(request)
    case PROJECT_RECENT_PATH:
      return handleRecentProjectRequest(request)
    case PROJECT_MIGRATE_PATH:
      return handleProjectMigrateRequest(request)
    case PROJECT_RUNTIME_METADATA_PATH:
      return handleProjectRuntimeMetadataRequest(request)
    case PROJECT_CONFIG_PATH:
      return handleProjectConfigRequest(request)
    default: {
      // eslint-disable-next-line no-console
      console.error('Unknown project request:', pathname)
      return new Response('Not Found', {status: 404})
    }
  }
}

app.on('before-quit', async (event) => {
  if (appKeyToLocalServerManager.size > 0) {
    event.preventDefault()

    await Promise.all(
      Array.from(appKeyToLocalServerManager.values()).map(manager => manager.stop())
    )
    appKeyToLocalServerManager.clear()

    app.quit()
  }
})

export {
  handleProjectRequest,
}
