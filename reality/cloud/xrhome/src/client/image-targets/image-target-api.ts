import {
  LIST_PATH, ListTargetsResponse,
} from '@repo/reality/shared/desktop/image-target-api'

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

export {
  listImageTargets,
}
