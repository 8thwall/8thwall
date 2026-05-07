import {dialog, shell, app} from 'electron'
import os from 'os'
import path from 'path'
import fs from 'fs/promises'
import log from 'electron-log'
import archiver from 'archiver'
import crypto from 'crypto'

import {makeRunQueue} from '@repo/reality/shared/run-queue'
import type {RuntimeMetadata} from '@repo/c8/ecs/src/shared/runtime-version'

import type {
  InitializeResponse, Project, ProjectConfigResponse,
} from '@repo/reality/shared/desktop/local-sync-types'

import {
  FixConfigParams, InstallRequest,
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
  PROJECT_STATUS_PATH, PROJECT_WATCH_PATH, PROJECT_INSTALL_PATH,
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
import {projectSetup} from './create-project-files'
import {createLocalServer, LocalServer} from '../../local-server'
import {openInCodeEditor} from '../preferences/code-editor'
import {runBuildCommand, runInstallCommand} from './run-commands'
import {branches, methods, RequestHandler} from '../../requests'

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

const checkProjectMigrated = async (projectPath: string): Promise<boolean> => {
  try {
    const inlineRuntimeFolder = await fs.stat(path.join(projectPath, 'external/runtime'))
    if (inlineRuntimeFolder.isDirectory()) {
      return true
    }
  } catch (err) {
    // No inline folder
  }

  try {
    const packageJsonString = await fs.readFile(path.join(projectPath, 'package.json'), 'utf8')
    const packageJsonData = JSON.parse(packageJsonString)
    const packages = {...packageJsonData.dependencies, ...packageJsonData.devDependencies}
    if (packages['@8thwall/ecs']) {
      return true
    }
  } catch (err) {
    // No package/invalid package
  }

  return false
}

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

  const isMigrated = await checkProjectMigrated(projectPath)

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

  try {
    const info = await fs.stat(project.location)
    if (!info.isDirectory()) {
      throw makeCodedError('Project is not a directory', 400)
    } else {
      shell.openPath(project.location)
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

  let shouldUpdateIndexHtml = false
  let foldersToDelete: string[] = []

  switch (project.initialization) {
    case 'done':
      shouldUpdateIndexHtml = true
      foldersToDelete = ['.gen']
      break
    case 'v2':
      foldersToDelete = ['external']
      break
    default:
      throw makeCodedError('Project has unexpected status for migrate', 400)
  }

  await projectSetup(project.location, (filePath) => {
    if (filePath === 'src/index.html') {
      return shouldUpdateIndexHtml
    }
    if (filePath.startsWith('src/')) {
      return false
    }
    return true
  })

  await Promise.all(foldersToDelete.map(folder => (
    fs.rm(path.join(project.location, folder), {recursive: true, force: true})
  )))

  upsertLocalProject(project.appKey, project.location, 'v2')

  return makeJsonResponse({})
})

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

  const metadataPaths = [
    'node_modules/@8thwall/ecs/metadata.json',
    'external/runtime/metadata.json',
  ]

  for (const possiblePath of metadataPaths) {
    const runtimePath = path.join(project.location, possiblePath)
    try {
      // eslint-disable-next-line no-await-in-loop
      const metadataContent = await fs.readFile(runtimePath, 'utf-8')
      const metadata: RuntimeMetadata = JSON.parse(metadataContent)
      return makeJsonResponse(metadata)
    } catch (error: any) {
      if (error.code !== 'ENOENT') {
        throw error
      }
    }
  }
  throw makeCodedError('Runtime metadata not found', 404)
})

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

const BAD_COPY_PLUGIN_CONFIG = `\
      patterns: [
        {
          from: path.join(rootPath, 'node_modules/@8thwall/ecs/dist'),
          to: path.join(distPath, 'external/runtime'),
        },
        {
          from: path.join(rootPath, 'node_modules/@8thwall/engine-binary/dist'),
          to: path.join(distPath, 'external/xr'),
        },
        {
          from: path.join(rootPath, 'image-targets'),
          to: path.join(distPath, 'image-targets'),
          noErrorOnMissing: true,
        },
      ],`

const GOOD_COPY_PLUGIN_CONFIG = `\
      patterns: [
        {
          from: path.join(rootPath, 'node_modules/@8thwall/ecs/dist'),
          to: path.join(distPath, 'external/runtime'),
        },
        {
          from: path.join(rootPath, 'node_modules/@8thwall/engine-binary/dist'),
          to: path.join(distPath, 'external/xr'),
        },
        {
          from: path.join(srcPath, 'assets'),
          to: path.join(distPath, 'assets'),
          noErrorOnMissing: true,
        },
        {
          from: path.join(rootPath, 'image-targets'),
          to: path.join(distPath, 'image-targets'),
          noErrorOnMissing: true,
        },
      ],`

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
  let needsCopyPluginFix = false
  let missingDev8 = false
  try {
    const configPath = path.join(project.location, WEBPACK_CONFIG_PATH)
    const configContent = await fs.readFile(configPath, 'utf-8')
    needsInjectFix = configContent.includes(BAD_INJECT_CONFIG)
    needsCopyPluginFix = configContent.includes(BAD_COPY_PLUGIN_CONFIG)
    missingDev8 = !configContent.includes('dev8')
  } catch (error) {
    // Ignore
  }

  const response: ProjectConfigResponse = {
    needsInjectFix,
    needsCopyPluginFix,
    missingDev8,
  }

  return makeJsonResponse(response)
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
    case 'copy-plugin': {
      const configPath = path.join(project.location, WEBPACK_CONFIG_PATH)
      const configContent = await fs.readFile(configPath, 'utf-8')
      const fixedContent = configContent.replace(BAD_COPY_PLUGIN_CONFIG, GOOD_COPY_PLUGIN_CONFIG)
      await fs.writeFile(configPath, fixedContent, 'utf-8')
      break
    }
    default:
      throw makeCodedError('Unknown config fix type', 400)
  }
  return makeJsonResponse({})
})

const installPackages: RequestHandler = async (req) => {
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
  const parsedBody = InstallRequest.safeParse(await req.json())
  if (!parsedBody.success) {
    throw makeCodedError('Invalid request body', 400)
  }
  const parts = parsedBody.data.packages.map(e => `${e.name}@${e.version}`)
  await runInstallCommand(project.location, parts)
  return makeJsonResponse({})
}

const handleProjectRequest = withErrorHandlingResponse(branches({
  [PROJECT_INIT_PATH]: methods({POST: getLocalProjectLocation}),
  [PROJECT_WATCH_PATH]: methods({
    POST: startWatch,
    DELETE: stopWatch,
  }),
  [PROJECT_BUILD_PATH]: methods({POST: buildZip}),
  [PROJECT_STATUS_PATH]: methods({GET: getProjectStatus}),
  [PROJECT_LIST_PATH]: methods({GET: getAllProjects}),
  [PROJECT_REVEAL_IN_FINDER_PATH]: methods({POST: postRevealProject}),
  [PROJECT_OPEN_PATH]: methods({POST: postOpenProject}),
  [PROJECT_DELETE_PATH]: methods({DELETE: deleteLocalProject}),
  [PROJECT_PICK_NEW_LOCATION_PATH]: methods({PATCH: pickNewProjectLocation}),
  [PROJECT_MOVE_PATH]: methods({PATCH: changeProjectLocation}),
  [PROJECT_OPEN_DISK_PATH]: methods({POST: openDiskLocation}),
  [PROJECT_RECENT_PATH]: methods({POST: handleRecentProjectPost}),
  [PROJECT_MIGRATE_PATH]: methods({POST: handleProjectMigratePost}),
  [PROJECT_RUNTIME_METADATA_PATH]: methods({GET: getRuntimeMetadata}),
  [PROJECT_INSTALL_PATH]: methods({POST: installPackages}),
  [PROJECT_CONFIG_PATH]: methods({
    GET: getProjectConfig,
    POST: modifyProjectConfig,
  }),
}))

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
