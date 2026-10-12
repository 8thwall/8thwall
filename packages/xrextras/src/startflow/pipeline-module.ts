import type {StartFlowRenderer} from './renderer'
import {
  compatibilityScreen,
  currentPermissionsErrorEnvironment,
  permissionFromEngine,
  permissionFromFailureReason,
  requestedPermissions,
} from './runtime-detection'
import type {Permission, StartFlowPipelineModule} from './types'

declare const XR8: any

interface BeforeRunArgs {
  config: unknown
}

interface CameraStatusArgs {
  reason?: string
  status: string
}

interface EngineErrorDetails {
  message?: string
  permission?: string
  type?: string
}

interface ProcessCpuResult {
  facecontroller?: {cameraFeedTexture?: unknown}
  handcontroller?: {cameraFeedTexture?: unknown}
  layerscontroller?: {cameraFeedTexture?: unknown}
  reality?: {realityTexture?: unknown}
}

const stopCameraLoop = () => {
  XR8.pause()
  XR8.stop()
}

const errorMessage = (error: unknown, details?: EngineErrorDetails) => {
  if (error instanceof Error) {
    return error.message
  }
  if (typeof error === 'string') {
    return error
  }
  return details?.message ?? String(error)
}

const hasRealityTexture = (value: unknown) => {
  if (!value || typeof value !== 'object') {
    return false
  }
  const {reality, facecontroller, handcontroller, layerscontroller} = value as ProcessCpuResult
  return Boolean(
    reality?.realityTexture ||
    facecontroller?.cameraFeedTexture ||
    handcontroller?.cameraFeedTexture ||
    layerscontroller?.cameraFeedTexture
  )
}

const createStartFlowPipelineModule = (renderer: StartFlowRenderer): StartFlowPipelineModule => {
  let runConfig: unknown
  let cameraReady = false
  let resourcesReady = false

  const showCompatibilityScreen = () => {
    const compatibility = compatibilityScreen(runConfig)
    if (!compatibility) {
      return false
    }
    renderer.show(compatibility)
    return true
  }

  const showPermissionsError = (failed: Permission[]) => {
    renderer.show({
      type: 'permissions-error',
      environment: currentPermissionsErrorEnvironment(),
      failed,
      requested: requestedPermissions(),
    })
  }

  return {
    name: 'startflow',
    onBeforeRun: ({config}: BeforeRunArgs) => {
      runConfig = config
      cameraReady = false
      resourcesReady = false
      if (showCompatibilityScreen()) {
        return undefined
      }
      if (!renderer.getOptions().showStartScreen) {
        renderer.show({type: 'loading', requestingPermissions: false})
        return undefined
      }
      return new Promise<void>((resolve) => {
        renderer.showStart(() => {
          renderer.configure({showStartScreen: false})
          renderer.show({type: 'loading', requestingPermissions: false})
          resolve()
        })
      })
    },
    onCameraStatusChange: ({status, reason}: CameraStatusArgs) => {
      if (showCompatibilityScreen()) {
        stopCameraLoop()
        return
      }

      switch (status) {
        case 'requesting':
          renderer.show({type: 'loading', requestingPermissions: true})
          break
        case 'hasStream':
          renderer.show({type: 'loading', requestingPermissions: false})
          break
        case 'hasVideo':
        case 'hasDesktop3D':
        case 'hasSessionWebXr':
          cameraReady = true
          renderer.show({type: 'loading', requestingPermissions: false})
          break
        case 'failed':
          showPermissionsError([permissionFromFailureReason(reason) || 'camera'])
          break
        default:
          break
      }
    },
    onAppResourcesLoaded: () => {
      resourcesReady = true
    },
    onUpdate: ({processCpuResult}) => {
      if (renderer.getOptions().waitForRealityTexture &&
        !hasRealityTexture(processCpuResult)) {
        return
      }
      if (cameraReady && resourcesReady) {
        renderer.fadeOut()
      }
    },
    onException: (error: unknown) => {
      if (showCompatibilityScreen()) {
        return
      }

      const details = typeof error === 'object' && error
        ? error as EngineErrorDetails
        : undefined
      if (details?.type === 'permission') {
        const permission = permissionFromEngine(details.permission)
        if (permission) {
          showPermissionsError([permission])
          return
        }
      }

      renderer.show({type: 'unexpected-error', error: errorMessage(error, details)})
      stopCameraLoop()
    },
    onRemove: () => {
      renderer.hide()
      cameraReady = false
      resourcesReady = false
      runConfig = undefined
    },
  }
}

export {createStartFlowPipelineModule}
