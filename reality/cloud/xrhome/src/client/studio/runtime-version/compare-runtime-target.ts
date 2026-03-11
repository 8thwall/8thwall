import type {
  RuntimeVersionInfo, RuntimeVersionTarget, RuntimeVersionTargetLevel,
} from '@ecs/shared/runtime-version'
import type {DeepReadonly} from 'ts-essentials'

const isEqualRuntimeTarget = (left: RuntimeVersionTarget, right: RuntimeVersionTarget) => {
  if (left.type !== 'version' || right.type !== 'version') {
    throw new Error('Encountered unexpected type in isEqualRuntimeTarget')
  }
  switch (left.level) {
    case 'major':
      return right.level === 'major' && left.major === right.major
    case 'minor':
      return right.level === 'minor' && left.major === right.major &&
                 left.minor === right.minor
    case 'patch':
      return right.level === 'patch' && left.major === right.major &&
                 left.minor === right.minor && left.patch === right.patch
    default:
      throw new Error('Encountered unexpected level in isEqualRuntimeTarget')
  }
}

const isSameBaseVersionAtLevel = (
  left: RuntimeVersionTarget, right: RuntimeVersionTarget, level: RuntimeVersionTargetLevel
) => {
  switch (level) {
    case 'major':
      return left.major === right.major
    case 'minor':
      return left.major === right.major && left.minor === right.minor
    case 'patch':
      return left.major === right.major && left.minor === right.minor && left.patch === right.patch
    default:
      throw new Error('Encountered unexpected level in isSameBaseVersionAtLevel')
  }
}

const isGreaterPatchVersion = (
  left: DeepReadonly<RuntimeVersionTarget>, right: DeepReadonly<RuntimeVersionTarget>
) => {
  if (left.type !== 'version' || right.type !== 'version') {
    throw new Error('Encountered unexpected type in isGreaterPatchVersion')
  }
  if (left.level !== 'patch' || right.level !== 'patch') {
    throw new Error('Encountered unexpected level in isGreaterPatchVersion')
  }

  return left.major > right.major ||
         (left.major === right.major && left.minor > right.minor) ||
         (left.major === right.major && left.minor === right.minor && left.patch > right.patch)
}

const comparePatchVersions = (
  left: DeepReadonly<RuntimeVersionTarget>, right: DeepReadonly<RuntimeVersionTarget>
) => {
  if (isEqualRuntimeTarget(left, right)) {
    return 0
  } else {
    return isGreaterPatchVersion(left, right) ? -1 : 1
  }
}

const compareVersionInfo = (
  left: DeepReadonly<RuntimeVersionInfo>, right: DeepReadonly<RuntimeVersionInfo>
) => comparePatchVersions(left.patchTarget, right.patchTarget)

export {
  isSameBaseVersionAtLevel,
  isGreaterPatchVersion,
  compareVersionInfo,
}
