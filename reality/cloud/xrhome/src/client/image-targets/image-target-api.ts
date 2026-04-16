import {
  ImageTargetData, LIST_PATH, ListTargetsResponse, UPLOAD_PATH, UploadTargetParams,
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
}
