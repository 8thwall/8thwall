import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'
import {useLocalSyncContext} from '../local-sync-context'

const useRuntimeMetadata = () => {
  const {appKey} = useCurrentApp()
  const local = useLocalSyncContext()
  return useSuspenseQuery({
    queryKey: ['runtimeMetadata', local.localBuildUrl],
    queryFn: () => getRuntimeMetadata(appKey),
  }).data
}

export {
  useRuntimeMetadata,
}
