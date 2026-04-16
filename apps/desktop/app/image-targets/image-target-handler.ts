import path from 'path'
import fs from 'fs/promises'
import * as TargetApi from '@repo/reality/shared/desktop/image-target-api'
import {makeRunQueue} from '@repo/reality/shared/run-queue'
import type {Project} from '@repo/reality/shared/desktop/local-sync-types'
import {applyCrop} from '@repo/apps/image-target-cli/src/apply'
import sharp from 'sharp'

import {makeCodedError, withErrorHandlingResponse} from '../../errors'
import {branches, methods, RequestHandler} from '../../requests'
import {getLocalProject} from '../../local-project-db'
import {makeJsonResponse} from '../../json-response'
import {
  GetTextureParams, ListTargetsParams, UploadTargetParams, CropResult, DeleteTargetParams,
  UpdateTargetRequest,
} from './image-target-types'
import {makeStreamFileResponse} from '../../stream-file-response'
import {getQueryParams} from '../../query-params'

const loadProject = async (appKey: string) => {
  const project = getLocalProject(appKey)
  if (!project) {
    throw makeCodedError('Project not found', 404)
  }
  return project
}

const getTargetPath = (project: Project, name: string) => (
  path.join(project.location, 'image-targets', `${name}.json`)
)

const readTarget = async (targetPath: string): Promise<TargetApi.ImageTargetData> => {
  const dataString = await fs.readFile(targetPath, 'utf8')
  return JSON.parse(dataString)
}

const writeTarget = async (targetPath: string, data: TargetApi.ImageTargetData): Promise<void> => {
  await fs.writeFile(targetPath, `${JSON.stringify(data, null, 2)}\n`)
}

const handleListTargets: RequestHandler = async (req) => {
  const url = new URL(req.url)
  const parsedParams = ListTargetsParams.safeParse(getQueryParams(url))
  if (!parsedParams.data) {
    throw makeCodedError('Invalid params', 400)
  }
  const project = await loadProject(parsedParams.data.appKey)
  const folder = path.join(project.location, 'image-targets')
  try {
    const contents = await fs.readdir(folder)
    const runQueue = makeRunQueue(10)
    const targets: TargetApi.ImageTargetData[] = []
    const invalidPaths: string[] = []

    await Promise.all(
      contents.filter(e => e.endsWith('.json')).map(async filename => runQueue.next(async () => {
        try {
          targets.push(await readTarget(path.join(folder, filename)))
        } catch (err) {
          invalidPaths.push(filename)
        }
      }))
    )

    return makeJsonResponse({targets, invalidPaths})
  } catch (err: any) {
    if (err.code !== 'ENOENT') {
      throw err
    }
    return makeJsonResponse({targets: []})
  }
}

const handleUpload: RequestHandler = async (req) => {
  const url = new URL(req.url)
  const parsedParams = UploadTargetParams.safeParse(getQueryParams(url))
  if (!parsedParams.data) {
    return makeJsonResponse({
      message: 'Invalid upload params',
      issues: parsedParams.error.issues,
    }, 400)
  }

  const parsedCrop = CropResult.safeParse(JSON.parse(parsedParams.data.crop))
  if (!parsedCrop.data) {
    return makeJsonResponse({
      message: 'Invalid crop params',
      issues: parsedCrop.error.issues,
    }, 400)
  }

  const project = await loadProject(parsedParams.data.appKey)
  await applyCrop(
    sharp(await req.arrayBuffer()),
    parsedCrop.data,
    path.join(project.location, 'image-targets'),
    parsedParams.data.name,
    true /* overwrite */
  )
  return makeJsonResponse(await readTarget(getTargetPath(project, parsedParams.data.name)))
}

const extractImagePath = (target: TargetApi.ImageTargetData, type: TargetApi.TargetTextureType) => {
  switch (type) {
    case 'cropped':
      return target.resources?.croppedImage
    case 'luminance':
      return target.resources?.luminanceImage
    case 'geometry':
      return target.resources?.geometryImage
    case 'original':
      return target.resources?.originalImage
    case 'thumbnail':
      return target.resources?.thumbnailImage
    default:
      return null
  }
}

const resolveImagePath = async (
  targetPath: string,
  target: TargetApi.ImageTargetData,
  type: TargetApi.TargetTextureType
) => {
  const relativePath = extractImagePath(target, type)
  if (!relativePath) {
    const extensionOptions = ['.jpg', '.png', '.jpeg']
    const basePath = path.join(path.dirname(targetPath), `${target.name}_${type}`)
    for (const extension of extensionOptions) {
      try {
        const fullPath = basePath + extension
        // eslint-disable-next-line no-await-in-loop
        await fs.stat(fullPath)
        return fullPath
      } catch (err: any) {
        if (err.code !== 'ENOENT') {
          throw err
        }
      }
    }
    return null
  }
  return path.join(path.dirname(targetPath), relativePath)
}

const handleGetTexture: RequestHandler = async (req) => {
  const url = new URL(req.url)
  const parsedParams = GetTextureParams.safeParse(getQueryParams(url))
  if (!parsedParams.data) {
    throw makeCodedError(`Invalid params: ${parsedParams.error.toString()}`, 400)
  }
  const project = await loadProject(parsedParams.data.appKey)
  const targetPath = getTargetPath(project, parsedParams.data.name)
  const target = await readTarget(targetPath)
  const imagePath = await resolveImagePath(targetPath, target, parsedParams.data.type)
  if (!imagePath) {
    throw makeCodedError('Not found', 404)
  }
  return makeStreamFileResponse(imagePath)
}

const handleTargetDelete: RequestHandler = async (req) => {
  const url = new URL(req.url)
  const parsedParams = DeleteTargetParams.safeParse(getQueryParams(url))
  if (!parsedParams.data) {
    throw makeCodedError('Invalid params', 400)
  }
  const project = await loadProject(parsedParams.data.appKey)
  const filePath = getTargetPath(project, parsedParams.data.name)

  const target = await readTarget(filePath)

  const filesToDelete = [filePath]

  if (target.resources) {
    filesToDelete.push(
      ...Object.values(target.resources).map(e => path.join(path.dirname(filePath), e))
    )
  }
  await Promise.allSettled(filesToDelete.map(e => fs.unlink(e)))
  return makeJsonResponse({})
}

const renameResource = async (
  targetPath: string,
  target: TargetApi.ImageTargetData,
  newName: string,
  type: TargetApi.TargetTextureType
) => {
  const oldPath = await resolveImagePath(targetPath, target, type)
  if (!oldPath) {
    return undefined
  }
  const newBasePath = `${newName}_${type}${path.extname(oldPath)}`
  const newPath = path.join(path.dirname(targetPath), newBasePath)
  await fs.rename(oldPath, newPath)
  return newBasePath
}

const renameResources = async (
  targetPath: string,
  target: TargetApi.ImageTargetData,
  newName: string
) => {
  const resourceTypes = [
    'luminance', 'geometry', 'original', 'cropped', 'thumbnail',
  ] as const
  const [
    luminanceImage, geometryImage, originalImage, croppedImage, thumbnailImage,
  ] = await Promise.all(resourceTypes.map(e => renameResource(targetPath, target, newName, e)))

  return {
    imagePath: `image-targets/${luminanceImage}`,
    resources: {
      originalImage,
      croppedImage,
      thumbnailImage,
      luminanceImage,
      geometryImage,
    },
  }
}

const handleTargetPatch: RequestHandler = async (req) => {
  const params = new URL(req.url).searchParams
  const project = await loadProject(params.get('appKey')!)
  const sourcePath = getTargetPath(project, params.get('name')!)
  const parsedBody = UpdateTargetRequest.safeParse(await req.json())
  if (parsedBody.error) {
    throw makeCodedError(`Invalid update data: ${parsedBody.error.toString()}`, 400)
  }
  const oldData = await readTarget(sourcePath)
  const newData: TargetApi.ImageTargetData = {
    ...oldData,
    updated: Date.now(),
    ...parsedBody.data,
  }

  // NOTE(christoph): At one point during the sunset period, exported image targets were including
  // this parameter, but it is not required going forward since we're storing the (user) metadata
  // field as the final object, not stringifying to fit into a database column.
  // @ts-expect-error
  delete newData.userMetadataIsJson

  const targetPath = getTargetPath(project, newData.name)

  if (sourcePath !== targetPath) {
    let exists = false
    try {
      await fs.stat(targetPath)
      exists = true
    } catch (err: any) {
      if (err.code !== 'ENOENT') {
        throw err
      }
    }
    if (exists) {
      throw makeCodedError('A target with this name already exists, cannot rename', 409)
    }
  }

  if (newData.name !== oldData.name) {
    Object.assign(newData, await renameResources(sourcePath, oldData, newData.name))
    await fs.rm(sourcePath)
  }

  await writeTarget(targetPath, newData)
  return makeJsonResponse(newData)
}

const handleImageTargetRequest = withErrorHandlingResponse(branches({
  [TargetApi.LIST_PATH]: methods({
    GET: handleListTargets,
  }),
  [TargetApi.TEXTURE_PATH]: methods({
    GET: handleGetTexture,
  }),
  [TargetApi.UPLOAD_PATH]: methods({
    POST: handleUpload,
  }),
  [TargetApi.TARGET_PATH]: methods({
    DELETE: handleTargetDelete,
    PATCH: handleTargetPatch,
  }),
}))

export {
  handleImageTargetRequest,
}
