import type {
  RuntimeVersionInfo, RuntimeVersionTarget, RuntimeVersionTargetLevel,
} from '@ecs/shared/runtime-version'
import type {DeepReadonly} from 'ts-essentials'

import {isGreaterPatchVersion} from './compare-runtime-target'

const createNewVersionTarget = (
  version: RuntimeVersionTarget, level: string
): RuntimeVersionTarget => {
  switch (level) {
    case ('major'):
      return {...version, major: version.major + 1, minor: 0, patch: 0}
    case ('minor'):
      return {...version, major: version.major, minor: version.minor + 1, patch: 0}
    case ('patch'):
      return {...version, major: version.major, minor: version.minor, patch: version.patch + 1}
    default:
      return version
  }
}

const getNewestVersionInfo = (versions: DeepReadonly<RuntimeVersionInfo[]>): RuntimeVersionInfo => {
  if (!versions || versions.length === 0) {
    return null
  }
  return versions.reduce((prev, next) => ((
    isGreaterPatchVersion(next.patchTarget, prev.patchTarget))
    ? next
    : prev
  ))
}

const getVersionSpecifier = (version: RuntimeVersionTarget): string => {
  const numbers: number[] = [version.major, version.minor, version.patch]
  const specifier = numbers.map(n => (Number.isInteger(n) ? n : 'x'))
  return specifier.join('.')
}

const getVersionSpecifierAtLevel = (
  version: RuntimeVersionTarget, level: RuntimeVersionTargetLevel
): string => {
  if (version.level !== 'patch') {
    throw new Error('getVersionSpecifierAtLevel expected patch target')
  }
  switch (level) {
    case 'major':
      return `${version.major}.x.x`
    case 'minor':
      return `${version.major}.${version.minor}.x`
    case 'patch':
      return `${version.major}.${version.minor}.${version.patch}`
    default:
      throw new Error('Encountered unexpected level in isSameBaseVersionAtLevel')
  }
}

export {
  getVersionSpecifier,
  getNewestVersionInfo,
  createNewVersionTarget,
  getVersionSpecifierAtLevel,
}
