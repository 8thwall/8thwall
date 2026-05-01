import {createMouseToTouchTranslator} from './mouse-to-touch-translator'
import type {
  SimulatorSessionManager,
  SimulatorConfig,
  SimulatorRendererConfig,
  SequenceControls,
  LocationEventControls,
} from './simulator-types'
import {SessionManagerSimulatorSequence} from './session-manager-simulatorsequence'
import {SessionManagerSimulatorRenderer} from './session-manager-simulatorrenderer'
import {
  broadcastConfigUpdateConfirmation, broadcastInitializeConfirmation,
} from './broadcast-messages'

declare const XR8: any

const SIM_MODULE_NAME = 'xrsimulatormodule'

const waitForXr8 = () => new Promise<typeof XR8>((resolve) => {
  if (typeof XR8 !== 'undefined') {
    resolve(XR8)
  } else {
    window.addEventListener('xrloaded', () => resolve(XR8), {once: true})
  }
})

const XrSimulatorManager = () => {
  const xr8Promise = waitForXr8()
  let addedPipelineModule_ = false
  let enabled_ = false
  let simulatorSessionManager_: SimulatorSessionManager
  let sequenceControls_: SequenceControls
  let locationEventControls_: LocationEventControls
  const touchTranslator_ = createMouseToTouchTranslator()

  const addSimulatorPipelineModule = async (
    simulatorConfig: SimulatorConfig,
    broadcastSequenceProgress?: (simulatorId: string, progress: number) => void
  ) => {
    await xr8Promise

    if (!enabled_) {
      return
    }

    const simulatorModule = {
      name: SIM_MODULE_NAME,
      onBeforeSessionInitialize: ({sessionAttributes}) => {
        if (!sessionAttributes.isSimulator) {
          throw new Error('Must be simulator')
        }
      },
      onRemove: () => {
        if (enabled_) {
          setTimeout(() => {
            XR8.addCameraPipelineModule(simulatorModule)
          }, 0)
        }
      },
      sessionManager: () => {
        if (simulatorConfig.poiId || simulatorConfig.imageTargetName) {
          const {
            sessionManager, locationEventControls,
          } = SessionManagerSimulatorRenderer(simulatorConfig)
          simulatorSessionManager_ = sessionManager
          locationEventControls_ = locationEventControls
        } else {
          const {sessionManager, sequenceControls} = SessionManagerSimulatorSequence(
            simulatorConfig,
            broadcastSequenceProgress
          )
          simulatorSessionManager_ = sessionManager
          sequenceControls_ = sequenceControls
        }
        broadcastInitializeConfirmation(simulatorConfig.simulatorId)
        return simulatorSessionManager_
      },
    }

    XR8.addCameraPipelineModule(simulatorModule)
    addedPipelineModule_ = true
  }

  const enable = (
    simulatorConfig: SimulatorConfig,
    simulatorRendererConfig: SimulatorRendererConfig,
    broadcastSequenceProgress: (simulatorId: string, progress: number) => void
  ) => {
    if (enabled_) {
      return
    }

    const {userAgent} = simulatorConfig
    if (userAgent) {
      // Note(Dale): Cannot do navigator.userAgent = ... because it is a read-only property.
      Object.defineProperty(
        navigator, 'userAgent', {value: userAgent, writable: false, configurable: true}
      )
    }
    enabled_ = true

    if (simulatorConfig.poiId) {
      addSimulatorPipelineModule({
        ...simulatorConfig,
        ...simulatorRendererConfig,
        version: simulatorRendererConfig.version ?? simulatorConfig.version,
      }, broadcastSequenceProgress)
    } else {
      addSimulatorPipelineModule(simulatorConfig, broadcastSequenceProgress)
    }

    touchTranslator_.attach()
  }

  const disable = () => {
    enabled_ = false
    if (addedPipelineModule_) {
      XR8.removeCameraPipelineModule(SIM_MODULE_NAME)
      simulatorSessionManager_?.clear()
      addedPipelineModule_ = false
      touchTranslator_.detach()
    }
  }

  const updateSimulator = async (
    simulatorConfig: SimulatorConfig, simulatorRendererConfig: SimulatorRendererConfig
  ) => {
    // Don't apply the updates if simulatorSessionManager_ is not yet ready.
    if (!addedPipelineModule_ || !simulatorSessionManager_) {
      return
    }

    if (simulatorConfig.poiId || simulatorConfig.imageTargetName) {
      simulatorSessionManager_.updateConfig({
        ...simulatorConfig,
        ...simulatorRendererConfig,
        version: simulatorRendererConfig.version ?? simulatorConfig.version,
      })
    } else {
      simulatorSessionManager_.updateConfig(simulatorConfig)
    }

    broadcastConfigUpdateConfirmation(simulatorConfig.simulatorId)
  }

  const scrub = (progress: number) => {
    if (addedPipelineModule_) {
      sequenceControls_?.scrub?.(progress)
    }
  }

  const stopScrub = () => {
    if (addedPipelineModule_) {
      sequenceControls_?.stopScrub?.()
    }
  }

  const dispatchLocationScanning = () => {
    locationEventControls_.dispatchLocationScanning()
  }

  const dispatchLocationFound = () => {
    locationEventControls_?.dispatchLocationFound()
  }

  const dispatchLocationLost = () => {
    locationEventControls_?.dispatchLocationLost()
  }

  const dispatchMeshFound = () => {
    locationEventControls_?.dispatchMeshFound()
  }

  const dispatchMeshLost = () => {
    locationEventControls_?.dispatchMeshLost()
  }

  return {
    enable,
    disable,
    updateSimulator,
    scrub,
    stopScrub,
    dispatchLocationScanning,
    dispatchLocationFound,
    dispatchLocationLost,
    dispatchMeshFound,
    dispatchMeshLost,
  }
}

export {
  XrSimulatorManager,
}
