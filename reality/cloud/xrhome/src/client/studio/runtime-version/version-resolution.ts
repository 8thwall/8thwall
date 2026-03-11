import type {
  RuntimeVersionInfo, RuntimeVersionList, RuntimeVersionTarget, RuntimeVersionTargetLevel,
} from '@ecs/shared/runtime-version'

import type {DeepReadonly} from 'ts-essentials'

import {isSameBaseVersionAtLevel} from './compare-runtime-target'

const resolveSelectedVersion = (
  target: RuntimeVersionTarget, versions?: RuntimeVersionList
) => {
  if (target.type !== 'version') {
    return null
  }

  if (!versions) {
    return null
  }

  return versions.find(vi => isSameBaseVersionAtLevel(target, vi.patchTarget, target.level))
}

const filterResolvableVersions = (
  level: RuntimeVersionTargetLevel, versions: RuntimeVersionList
) => {
  if (!versions) {
    return null
  }

  if (level === 'patch') {
    return versions
  }

  const res: DeepReadonly<RuntimeVersionInfo>[] = []

  let lastSeen: RuntimeVersionTarget

  versions.forEach((version) => {
    if (lastSeen && isSameBaseVersionAtLevel(version.patchTarget, lastSeen, level)) {
      return
    }
    lastSeen = version.patchTarget
    res.push(version)
  })

  return res
}

export {
  resolveSelectedVersion,
  filterResolvableVersions,
}
