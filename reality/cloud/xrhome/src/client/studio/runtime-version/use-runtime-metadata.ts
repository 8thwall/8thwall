import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'

/* eslint-disable no-await-in-loop */

const untilAbort = (signal: AbortSignal) => new Promise<void>((resolve) => {
  if (signal.aborted) {
    resolve()
  } else {
    signal.addEventListener('abort', () => {
      resolve()
    })
  }
})

const getRuntimeMetadataQuery = (appKey: string) => ({
  queryKey: ['runtimeMetadata', appKey],
  queryFn: async ({signal}: {signal: AbortSignal}) => {
    // NOTE(christoph): During first time setup, this may mount before the config is fully
    // initialized. After the server starts, it will trigger a refetch regardless, but to avoid
    // error screens on start, absorb failures.
    let iterations = 0
    while (iterations++ < 10) {
      if (signal.aborted) {
        throw new Error('runtimeMetadata cancelled')
      }
      try {
        return await getRuntimeMetadata(appKey)
      } catch (err) {
        await new Promise(r => setTimeout(r, 1500))
      }
    }
    await untilAbort(signal)
    throw new Error('runtimeMetadata cancelled after timeout')
  },
})

const useRuntimeMetadata = () => {
  const {appKey} = useCurrentApp()
  return useSuspenseQuery(getRuntimeMetadataQuery(appKey)).data
}

export {
  useRuntimeMetadata,
  getRuntimeMetadataQuery,
}
