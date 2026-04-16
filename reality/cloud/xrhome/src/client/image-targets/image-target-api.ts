import {
  ImageTargetData, LIST_PATH, ListTargetsResponse, UPLOAD_PATH, UploadTargetParams,
  DeleteTargetParams, TARGET_PATH, UpdateTargetParams, UpdateTargetRequest,
} from '@repo/reality/shared/desktop/image-target-api'
import type {CropResult} from '@repo/reality/shared/desktop/image-target-api'

type FetchOptions = {}

const fetchJson = async <T>(url: string, options?: FetchOptions): Promise<T> => {
  const response = await fetch(url, options)
  if (!response.ok) {
    throw Object.assign(
      new Error(`fetch error status code: ${response.status}, ${response.statusText}`),
      {res: response}
    )
  }
  return response.json()
}

const listImageTargets = (appKey: string) => (
  fetchJson<ListTargetsResponse>(
    `image-targets://${LIST_PATH}?${new URLSearchParams({appKey})}`
  )
)

const deleteImageTarget = (appKey: string, name: string) => {
  const params: DeleteTargetParams = {
    appKey, name,
  }
  return fetchJson<{}>(
    `image-targets://${TARGET_PATH}?${new URLSearchParams(params)}`,
    {method: 'DELETE'}
  )
}

const updateImageTarget = (appKey: string, name: string, update: UpdateTargetRequest) => {
  const params: UpdateTargetParams = {
    appKey, name,
  }
  return fetchJson<void>(
    `image-targets://${TARGET_PATH}?${new URLSearchParams(params)}`,
    {body: JSON.stringify(update), method: 'PATCH'}
  )
}

const uploadImageTarget = (
  appKey: string,
  image: Blob,
  name: string,
  crop: CropResult
) => {
  const params: UploadTargetParams = {
    appKey,
    name,
    crop: JSON.stringify(crop),
  }

  return fetchJson<ImageTargetData>(
    `image-targets://${UPLOAD_PATH}?${new URLSearchParams(params)}`,
    {method: 'POST', body: image}
  )
}

export {
  listImageTargets,
  uploadImageTarget,
  deleteImageTarget,
  updateImageTarget,
}
