import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'

/* eslint-disable no-await-in-loop */

const getRuntimeMetadataQuery = (appKey: string) => ({
  queryKey: ['runtimeMetadata', appKey],
  queryFn: async () => {
    // await new Promise(r => setTimeout(r, 2500))
    let iterations = 0
    while (iterations++ < 10) {
      try {
        return await getRuntimeMetadata(appKey)
      } catch (err) {
        await new Promise(r => setTimeout(r, 1500))
      }
    }
    throw new Error('Could not load metadata')
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
