import type {GraphObject, SceneGraph, Space} from '@ecs/shared/scene-graph'

import type {DeepReadonly} from 'ts-essentials'

import {getParentPrefabId, resolveSpaceForObject} from '@ecs/shared/object-hierarchy'

import {fileExt} from '../../editor/editor-common'
import type {IGit} from '../../git/g8-dto'
import {isAssetPath} from '../../common/editor-files'
import type {DerivedScene} from '../derive-scene'

const ASSET_EXT_TO_KIND = {
  'glb': 'model',
  'gltf': 'model',
  'hcap': 'hologram',
  'tvm': 'hologram',
  'm4a': 'audio',
  'mp3': 'audio',
  'wav': 'audio',
  'ogg': 'audio',
  'aac': 'audio',
  'jpg': 'image',
  'jpeg': 'image',
  'png': 'image',
  'svg': 'image',
  'ico': 'image',
  'gif': 'image',
  'hdr': 'special_image',
  'mp4': 'video',
  'ttf': 'font',
  'woff': 'font',
  'woff2': 'font',
  'otf': 'font',
  'fnt': 'font',
  'eot': 'font',
  'spz': 'splat',
  'font8': 'font8',
} as const

const EXPANSE_FILE_PATH = '.expanse.json'
const EXPANSE_FILE_REGEX = '^\\.expanse\\.json$'
const NAME_MATCH_BASE_REGEX = / \(\d+\)$/
const NAME_MATCH_SEARCH_REGEX = / \((\d+)\)$/
const STUDIO_HIDDEN_FILE_PATHS = [EXPANSE_FILE_PATH]

type Kind = typeof ASSET_EXT_TO_KIND[keyof typeof ASSET_EXT_TO_KIND]

const getFilesByKind = (filesByPath: IGit['filesByPath'], kind: Kind | Kind[]) => {
  if (!filesByPath) {
    return []
  }
  return (
    Object.values(filesByPath).filter(({filePath, isDirectory}) => {
      if (isDirectory) {
        return false
      }
      if (!isAssetPath(filePath)) {
        return false
      }
      const ext = fileExt(filePath)
      if (Array.isArray(kind)) {
        return kind.includes(ASSET_EXT_TO_KIND[ext])
      } else if (kind) {
        return ASSET_EXT_TO_KIND[ext] === kind
      } else {
        return true
      }
    }).map(f => f.filePath)
  )
}

type WithName = DeepReadonly<{
  name: string
}> & GraphObject

const makeObjectName = (
  name: string, scene: DeepReadonly<SceneGraph>, spaceId: string | undefined, prefab?: string,
  newPrefab?: boolean
) => {
  if (!name) {
    return ''
  }

  const baseName = name.replace(NAME_MATCH_BASE_REGEX, '')

  const objects = Object.values(scene.objects)
    .filter((object): object is WithName => {
      const matchesBaseName = object.name === baseName || object.name?.startsWith(`${baseName} `)

      if (!matchesBaseName) {
        return false
      }

      if (newPrefab) {
        return object.prefab
      }

      const prefabParent = getParentPrefabId(scene, object.id)
      if (prefab) {
        return prefabParent === prefab
      }

      return resolveSpaceForObject(scene, object.id)?.id === spaceId && prefabParent === undefined
    })

  if (objects.length === 0) {
    return name
  }

  const existingNumbers = objects
    .map((object) => {
      const match = object.name.match(NAME_MATCH_SEARCH_REGEX)
      return match ? parseInt(match[1], 10) : 0
    })
  const nextNumber = existingNumbers.length ? Math.max(...existingNumbers) + 1 : 1

  return `${baseName} (${nextNumber})`
}

const prefabNameExists = (derivedScene: DerivedScene, name: string, id?: string) => {
  if (!name) {
    return false
  }
  return derivedScene.getPrefabs().some(prefab => prefab.name === name && prefab.id !== id)
}

const makeDuplicatedSpaceName = (spaceName: string, spaces: DeepReadonly<Space>[]) => {
  const baseName = spaceName.replace(NAME_MATCH_BASE_REGEX, '')

  const filteredSpaces = spaces.filter(s => (s.name === baseName ||
      s.name?.startsWith(`${baseName} `)))

  if (filteredSpaces.length === 0) {
    return spaceName
  }

  const existingNumbers = spaces.map((s) => {
    const match = s.name.match(NAME_MATCH_SEARCH_REGEX)
    return match ? parseInt(match[1], 10) : 0
  })

  const nextNumber = existingNumbers.length ? Math.max(...existingNumbers) + 1 : 1

  return `${baseName} (${nextNumber})`
}

export type {
  Kind,
}

export {
  ASSET_EXT_TO_KIND,
  EXPANSE_FILE_PATH,
  EXPANSE_FILE_REGEX,
  STUDIO_HIDDEN_FILE_PATHS,
  getFilesByKind,
  makeObjectName,
  prefabNameExists,
  makeDuplicatedSpaceName,
}
