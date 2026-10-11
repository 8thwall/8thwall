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

const createStartFlowPipelineModule = (renderer: StartFlowRenderer) => {
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
    const requested = requestedPermissions()
    const uniqueFailed = [...new Set(failed)]
    renderer.show({
      type: 'permissions-error',
      environment: currentPermissionsErrorEnvironment(),
      failed: uniqueFailed,
      requested: [...new Set([...requested, ...uniqueFailed])],
    })
  }

  const pipelineModule = (): StartFlowPipelineModule => ({
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
    onUpdate: () => {
      if (cameraReady && resourcesReady) {
        renderer.hide()
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
        showPermissionsError(permission ? [permission] : requestedPermissions())
        return
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
  })

  return pipelineModule
}

export {createStartFlowPipelineModule}
