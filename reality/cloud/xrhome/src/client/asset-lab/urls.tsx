import {getMeshUrl, getImageUrl, getOptimizedMeshUrl} from '../../shared/genai/constants'
import type {AssetGeneration} from '../common/types/db'
import {useSelector} from '../hooks'
import {urlToUuid} from './generate-request'
import {isAssetGenerationMesh} from './types'

const getAssetGenUrl = (assetGeneration: AssetGeneration) => {
  if (!assetGeneration) {
    return null
  }
  const {AccountUuid, uuid} = assetGeneration
  // TODO(dat): Get the URL template as a config from the server

  if (isAssetGenerationMesh(assetGeneration)) {
    if (assetGeneration.metadata?.optimizer?.completed) {
      return getOptimizedMeshUrl(AccountUuid, uuid,
        assetGeneration.metadata?.optimizer?.timestampMs)
    } else {
      return getMeshUrl(AccountUuid, uuid)
    }
  }
  return getImageUrl(AccountUuid, uuid)
}

// Get the image that can be used to display in the library
const useAssetGenImageUrlRaw = (assetGeneration: AssetGeneration) => {
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)
  const assetRequests = useSelector(s => s.assetLab.assetRequests)
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration?.RequestUuid])
  const requestType = assetRequest?.type || assetGeneration?.metadata?.type

  if (requestType === 'TEXT_TO_IMAGE' || requestType === 'IMAGE_TO_IMAGE') {
    return getImageUrl(assetGeneration?.AccountUuid, assetGeneration?.uuid)
  }

  if (requestType === 'IMAGE_TO_MESH') {
    return assetRequest.input?.imageUrls?.[0] || assetGeneration?.metadata?.image_urls?.[0]
  }

  const meshUrl = assetRequest?.input?.meshUrl || assetGeneration?.metadata?.unrigged_mesh_url
  const meshGenUuid = urlToUuid(meshUrl as string)
  const meshGen = assetGenerations[meshGenUuid]
  const meshRequestUuid = meshGen?.RequestUuid
  return assetRequests[meshRequestUuid]?.input?.imageUrls?.[0] || meshGen?.metadata?.image_urls?.[0]
}

// eslint-disable-next-line max-len
const fallBackBase64Img = 'data:image/jpeg;base64,iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAIAAADTED8xAAADMElEQVR4nOzVwQnAIBQFQYXff81RUkQCOyDj1YOPnbXWPmeTRef+/3O/OyBjzh3CD95BfqICMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMK0CMO0TAAD//2Anhf4QtqobAAAAAElFTkSuQmCC'
const useAssetGenImageUrl = (ag: AssetGeneration) => useAssetGenImageUrlRaw(ag) || fallBackBase64Img

export {
  getAssetGenUrl,
  useAssetGenImageUrl,
  getMeshUrl,
  getImageUrl,
}
