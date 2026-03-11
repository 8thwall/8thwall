import path from 'path'
import {promises as fs} from 'fs'

import type {EcsManifest} from './ecs-manifest'
import {isValidManifest} from './validate-ecs-manifest'

type LoadRequest = {
  srcPath: string
  manifestFile: string
}

type LoadResult = {
  errors: string[]
  manifest?: EcsManifest
}

const loadEcsManifest = async ({srcPath, manifestFile}: LoadRequest): Promise<LoadResult> => {
  let manifestContents: string
  try {
    manifestContents = await fs.readFile(path.join(srcPath, manifestFile), 'utf8')
  } catch (error) {
    if ((error as any).code === 'ENOENT') {
      return {manifest: undefined, errors: []}
    } else {
      return {
        manifest: undefined,
        errors: [`Failed to read manifest.json: ${(error as Error).message}`],
      }
    }
  }

  if (!manifestContents) {
    return {manifest: undefined, errors: []}
  }

  let manifest: EcsManifest

  try {
    manifest = JSON.parse(manifestContents)
  } catch (error) {
    return {errors: ['failed to parse manifest.json']}
  }

  if (!isValidManifest(manifest)) {
    // eslint-disable-next-line no-console
    console.log('Invalid manifest:', manifest)
    return {errors: ['manifest.json does not pass validation.']}
  }

  return {errors: [], manifest}
}

export {
  loadEcsManifest,
}
