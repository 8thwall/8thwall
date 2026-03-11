import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'

const useRuntimeMetadata = () => {
  const {appKey} = useCurrentApp()
  return useSuspenseQuery({
    queryKey: ['runtimeMetadata'],
    queryFn: () => getRuntimeMetadata(appKey),
  }).data
}

export {
  useRuntimeMetadata,
}
