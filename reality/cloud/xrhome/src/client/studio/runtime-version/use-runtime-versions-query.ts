import {useSuspenseQuery} from '@tanstack/react-query'
import * as Semver from 'semver'

import {MILLISECONDS_PER_HOUR, MILLISECONDS_PER_MINUTE} from '../../../shared/time-utils'

type NpmPackageInfo = {
  versions: Record<string, unknown>
  time: Record<string, string>
}

const HIDDEN_VERSIONS = new Set([
  // NOTE(christoph): These versions are not supported
  '3.0.0',
  '3.0.1',
])

const useRuntimeVersions = () => useSuspenseQuery({
  queryKey: ['runtimeVersions'],
  queryFn: async () => {
    const response = await fetch('https://registry.npmjs.org/@8thwall/ecs', {mode: 'cors'})

    if (!response.ok) {
      throw new Error(`Failed to fetch runtime versions: ${response.status} ${response.statusText}`)
    }

    const body: NpmPackageInfo = await response.json()

    return Object.keys(body.versions)
      .filter(e => !HIDDEN_VERSIONS.has(e))
      .sort(Semver.rcompare)
      .map(e => ({
        publishTime: new Date(body.time[e]).valueOf(),
        version: e,
      }))
  },
  staleTime: 15 * MILLISECONDS_PER_MINUTE,
  refetchInterval: MILLISECONDS_PER_HOUR,
}).data

const useLatestRuntimeVersion = () => (
  useRuntimeVersions()[0]
)

export {
  useRuntimeVersions,
  useLatestRuntimeVersion,
}
