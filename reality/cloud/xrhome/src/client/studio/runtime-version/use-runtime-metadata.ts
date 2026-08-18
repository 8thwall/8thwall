import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'

/* eslint-disable no-await-in-loop */

const RUNTIME_METADATA_RETRY_INTERVAL_MS = 1500
const RUNTIME_METADATA_RETRY_ATTEMPTS = 10

const getRuntimeMetadataQuery = (appKey: string) => ({
  queryKey: ['runtimeMetadata', appKey],
  queryFn: async () => {
    // NOTE(christoph): During first time setup, this may mount before the config is fully
    // initialized. After the server starts, it will trigger a refetch regardless, but to avoid
    // error screens on start, absorb failures.
    let iterations = 0

    while (iterations++ < RUNTIME_METADATA_RETRY_ATTEMPTS) {
      try {
        return await getRuntimeMetadata(appKey)
      } catch (err) {
        await new Promise(r => setTimeout(r, RUNTIME_METADATA_RETRY_INTERVAL_MS))
      }
    }

    throw new Error('Could not load metadata')
  },
  retry: false,
})

const useRuntimeMetadata = () => {
  const {appKey} = useCurrentApp()
  return useSuspenseQuery(getRuntimeMetadataQuery(appKey)).data
}

export {
  useRuntimeMetadata,
  getRuntimeMetadataQuery,
}
