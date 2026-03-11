import {INITIAL_RUNTIME_VERSION} from '@repo/reality/shared/studio/runtime-version-constants'

import {useSceneContext} from '../scene-context'
import {resolveSelectedVersion} from './version-resolution'
import {useRuntimeVersions} from './use-runtime-versions-query'

const useResolvedRuntimeVersion = () => {
  const ctx = useSceneContext()
  const versions = useRuntimeVersions()
  const originalTarget = ctx.scene.runtimeVersion || INITIAL_RUNTIME_VERSION
  return {...resolveSelectedVersion(originalTarget, versions), originalTarget}
}

export {
  useResolvedRuntimeVersion,
}
