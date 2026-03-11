import type {EcsManifest} from './ecs-manifest'
import type {Script} from '@repo/reality/shared/studio/ecs-config'

const isValidManifest = (manifest: EcsManifest | any): manifest is EcsManifest => {
  if (!manifest || typeof manifest !== 'object') {
    return false
  }

  if (manifest.config?.runtimeUrl && typeof manifest.config.runtimeUrl !== 'string') {
    return false
  }

  if (manifest.config?.dev8Url && typeof manifest.config.dev8Url !== 'string') {
    return false
  }

  if (manifest.config?.app8Url && typeof manifest.config.app8Url !== 'string') {
    return false
  }

  const preloadChunks = manifest.config?.preloadChunks
  const preloadChunkIsValid = !preloadChunks || (
    Array.isArray(preloadChunks) && preloadChunks.every(chunk => typeof chunk === 'string')
  )

  if (!preloadChunkIsValid) {
    return false
  }

  if (manifest.config?.deferXr8 && typeof manifest.config.deferXr8 !== 'boolean') {
    return false
  }

  if (manifest.config?.scripts) {
    const isValidScript = (script: Script) => (
      script && typeof script === 'object' && typeof script.src === 'string'
    )
    if (!Array.isArray(manifest.config.scripts)) {
      return false
    }
    if (!manifest.config.scripts.every((script: Script) => isValidScript(script))) {
      return false
    }
  }

  if (manifest.config?.runtimeTypeCheck && typeof manifest.config.runtimeTypeCheck !== 'boolean') {
    return false
  }

  if (typeof manifest.config?.backgroundColor !== 'undefined') {
    if (typeof manifest.config.backgroundColor !== 'string') {
      return false
    }
    if (!/^#[0-9a-f]{6}$/i.test(manifest.config.backgroundColor)) {
      return false
    }
  }

  return manifest.version === 1
}

export {
  isValidManifest,
}
