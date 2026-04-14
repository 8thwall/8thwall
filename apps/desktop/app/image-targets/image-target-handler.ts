import path from 'path'
import fs from 'fs/promises'
import * as TargetApi from '@repo/reality/shared/desktop/image-target-api'
import {makeRunQueue} from '@repo/reality/shared/run-queue'
import type {Project} from '@repo/reality/shared/desktop/local-sync-types'

import {makeCodedError, withErrorHandlingResponse} from '../../errors'
import {branches, methods, RequestHandler} from '../../requests'
import {getLocalProject} from '../../local-project-db'
import {makeJsonResponse} from '../../json-response'
import {
  GetTextureParams, ListTargetsParams,
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

const resolveImagePath = async (targetPath: string, type: TargetApi.TargetTextureType) => {
  const target = await readTarget(targetPath)
  const relativePath = extractImagePath(target, type)
  if (!relativePath) {
    return null
    // TODO(christoph): Add fallback heuristic for legacy image format
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
  const imagePath = await resolveImagePath(targetPath, parsedParams.data.type)
  if (!imagePath) {
    throw makeCodedError('Not found', 404)
  }
  return makeStreamFileResponse(imagePath)
}

const handleImageTargetRequest = withErrorHandlingResponse(branches({
  [TargetApi.LIST_PATH]: methods({
    GET: handleListTargets,
  }),
  [TargetApi.TEXTURE_PATH]: methods({
    GET: handleGetTexture,
  }),
}))

export {
  handleImageTargetRequest,
}
