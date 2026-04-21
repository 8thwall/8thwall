import {useSuspenseQuery} from '@tanstack/react-query'

import useCurrentApp from '../../common/use-current-app'
import {getRuntimeMetadata} from '../local-sync-api'
import {useMaybeLocalSyncContext} from '../local-sync-context'

const useRuntimeMetadata = () => {
  const {appKey} = useCurrentApp()
  const local = useMaybeLocalSyncContext()
  return useSuspenseQuery({
    queryKey: ['runtimeMetadata', local?.localBuildUrl],
    queryFn: () => getRuntimeMetadata(appKey),
  }).data
}

export {
  useRuntimeMetadata,
}
