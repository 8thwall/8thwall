import type {Resource} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'
import {inferResourceObject} from '@ecs/shared/resource'

import {useScopedGitFile} from '../../git/hooks/use-current-git'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {FileSyncStatus, useLocalSyncContext} from '../local-sync-context'
import {makeLocalAssetUrl} from '../local-sync-api'

const READY_STATUSES: DeepReadonly<Array<FileSyncStatus>> = ['initialized', 'active', 'listening']

const useResourceUrl = (rawResource: DeepReadonly<string | Resource>) => {
  const resource = inferResourceObject(rawResource)

  const repoId = useCurrentRepoId()
  const fileContent = useScopedGitFile(repoId, resource?.type === 'asset' && resource.asset)
  const localSyncContext = useLocalSyncContext()

  if (!resource) {
    return undefined
  }
  switch (resource.type) {
    case 'url':
      return resource.url
    case 'asset': {
      if (!fileContent) {
        return undefined
      }
      const localReady = READY_STATUSES.includes(localSyncContext.fileSyncStatus)
      if (!localReady) {
        return undefined
      }
      const {appKey, assetVersions} = localSyncContext
      const assetPath = resource.asset
      return makeLocalAssetUrl(appKey, assetPath, assetVersions[resource.asset])
    }
    default:
      throw new Error('Unsupported resource type')
  }
}

export {
  useResourceUrl,
}
