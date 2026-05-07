import {queryOptions, useQuery, useSuspenseQuery} from '@tanstack/react-query'

import {MILLISECONDS_PER_HOUR} from '../../shared/time-utils'
import {useEnclosedAppKey} from '../apps/enclosed-app-context'
import {checkConfigStatus} from '../studio/local-sync-api'

const getProjectConfigStatusQuery = (appKey: string) => queryOptions({
  queryKey: ['project-config', appKey],
  queryFn: () => checkConfigStatus(appKey),
  staleTime: MILLISECONDS_PER_HOUR,
})

const useProjectConfigStatus = () => {
  const appKey = useEnclosedAppKey()
  return useSuspenseQuery(getProjectConfigStatusQuery(appKey)).data
}

const useProjectConfigStatusOrLoading = () => {
  const appKey = useEnclosedAppKey()
  return useQuery(getProjectConfigStatusQuery(appKey))
}

export {
  useProjectConfigStatus,
  useProjectConfigStatusOrLoading,
  getProjectConfigStatusQuery,
}
