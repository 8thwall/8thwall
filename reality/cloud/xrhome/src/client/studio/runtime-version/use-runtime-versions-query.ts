import {useSuspenseQuery} from '@tanstack/react-query'

import type {RuntimeVersionList} from '@ecs/shared/runtime-version'

import {
  CDN_BASE_URL, S3_VERSION_HISTORY_KEY,
} from '@repo/reality/shared/studio/runtime-version-constants'

import {compareVersionInfo} from './compare-runtime-target'

const useRuntimeVersions = () => useSuspenseQuery({
  queryKey: ['runtimeVersions'],
  queryFn: async (): Promise<RuntimeVersionList> => {
    const url = `${CDN_BASE_URL}/${S3_VERSION_HISTORY_KEY}`
    const response = await fetch(url)

    if (!response.ok) {
      throw new Error(`Failed to fetch runtime versions: ${response.status} ${response.statusText}`)
    }

    const versions = (await response.json()).versions as RuntimeVersionList ?? []
    return versions.sort(compareVersionInfo)
  },
}).data

const useLatestRuntimeVersion = () => (
  useRuntimeVersions()[0]
)

export {
  useRuntimeVersions,
  useLatestRuntimeVersion,
}
