import type {DeepReadonly} from 'ts-essentials'

import {isOptimizedMeshUrl} from '../../shared/genai/constants'
import type {FileAssetInput, AssetInput, UuidAssetInput} from '../../shared/genai/types/base'
import type {AssetGeneration, AssetRequest} from '../common/types/db'

type AssetPromptInput = File | string | null

const makeUuidInput = (uuid: string): UuidAssetInput => (
  {type: 'uuid', value: uuid}
)

const makeFileInput = (file: File): FileAssetInput => (
  {type: 'file', value: file}
)

const promptInputToInputs = (promptImage: AssetPromptInput): AssetInput[] => {
  if (!promptImage) {
    return []
  }

  if (typeof promptImage === 'string') {
    return [makeUuidInput(promptImage)]
  } else if (promptImage instanceof File) {
    return [makeFileInput(promptImage)]
  }
  return []
}

const promptInputsToInputs = (promptImages: AssetPromptInput[]): AssetInput[] => {
  if (!promptImages || !promptImages.length) {
    return []
  }
  return promptImages.flatMap(promptInputToInputs)
}

const urlToUuid = (urlWithQuery: string): string => {
  if (!urlWithQuery) {
    return null
  }
  const url = urlWithQuery.split('?')[0]
  if (isOptimizedMeshUrl(url)) {
    // remove /optimized.glb extension for uuid
    const urlWithoutOptimized = url.substring(0, url.length - 14)
    return urlWithoutOptimized.split('/').pop()
  }
  // Our assetGen URL has .glb for mesh but not for images
  if (url.endsWith('.glb')) {
    // remove .glb extension for uuid
    const urlWithoutGlb = url.substring(0, url.length - 4)
    return urlWithoutGlb.split('/').pop()
  }
  return url.split('/').pop()
}

const extractUuidInput = (assetRequest: DeepReadonly<AssetRequest>) => {
  if (assetRequest?.type === 'MESH_TO_ANIMATION') {
    // NOTE(kyle): It's easier to always return an array even though we do not support multiple
    // mesh inputs yet.
    return [urlToUuid(assetRequest?.input?.meshUrl)]
  }

  return (assetRequest?.input?.imageUrls || []).map(urlToUuid)
}

// TODO(kyle): Replace extractFblr with extractUuidInput
const extractFblr = (aq: DeepReadonly<AssetRequest>, ag?: AssetGeneration): string[] => {
  const imageUrls = aq?.input?.imageUrls as string[] || ag?.metadata?.image_urls as string[] || []
  if (!imageUrls.length) {
    return []
  }
  return imageUrls.map(urlToUuid).filter(Boolean)
}

export {
  makeUuidInput,
  makeFileInput,
  promptInputToInputs,
  promptInputsToInputs,
  extractFblr,
  extractUuidInput,
  urlToUuid,
}

export type {
  AssetPromptInput,
}
