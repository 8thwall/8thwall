import type {RuntimeVersionTarget} from '@repo/c8/ecs/src/shared/runtime-version'

import type {EcsManifest} from './ecs-manifest'
import {CDN_BASE_URL, INITIAL_RUNTIME_VERSION, S3_RELEASE_PREFIX} from './runtime-version-constants'

const generateVersionUrl = (
  versionTarget: RuntimeVersionTarget,
  baseSuffix: string
): string => {
  switch (versionTarget.level) {
    case 'major':
      return `${CDN_BASE_URL}/${S3_RELEASE_PREFIX}/major/${versionTarget.major}/${baseSuffix}`
    case 'minor':
      return (
        `${CDN_BASE_URL}/${S3_RELEASE_PREFIX}/minor/` +
        `${versionTarget.major}/${versionTarget.minor}/` +
        `${baseSuffix}`
      )
    case 'patch':
      return (
        `${CDN_BASE_URL}/${S3_RELEASE_PREFIX}/patch/` +
        `${versionTarget.major}/${versionTarget.minor}/` +
        `${versionTarget.patch}/${baseSuffix}`
      )
    default:
      return ''
  }
}

const generateRuntimeUrl = (
  manifest: EcsManifest | null, versionTarget: RuntimeVersionTarget | null
): string => {
  if (manifest?.config?.runtimeUrl) {
    return manifest.config.runtimeUrl
  }

  return generateVersionUrl(versionTarget || INITIAL_RUNTIME_VERSION, 'runtime.js')
}

const generateDev8Url = (
  manifest: EcsManifest | null, versionTarget: RuntimeVersionTarget | null
): string => {
  if (manifest?.config?.dev8Url) {
    return manifest.config.dev8Url
  }

  return generateVersionUrl(versionTarget || INITIAL_RUNTIME_VERSION, 'dev8/dev8.js')
}

const generateRuntimeArtifactUrl = (
  versionTarget: RuntimeVersionTarget | null, artifact: string
): string => (
  generateVersionUrl(versionTarget || INITIAL_RUNTIME_VERSION, artifact)
)

export {
  generateRuntimeUrl,
  generateDev8Url,
  generateRuntimeArtifactUrl,
}
