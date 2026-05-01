import type SimplePeer from 'simple-peer'

import {createNewP2PReceiver} from '../shared/webrtc/p2p-connect/receiver'
import {handleWebRtcOffer} from '../shared/webrtc/p2p-connect/handleWebRTCOffer'
import {RemoteAction, SimulatorHMDLinkActionType} from '../shared/webrtc/constants/remoteActions'
import XRRemote from './xrremote-core'
import {
  sendReadyForNewConnectionEventToXrHome,
  sendWebRtcAnswerToXrHome,
  sendWebRtcConnectionClosedEventToXrHome,
  sendWebRtcConnectionErrorEventToXrHome,
  sendWebRtcConnectionSuccessEventToXrHome,
  sendUnloadEventToXrHome,
} from './broadcast-messages'

let _handleNextOffer: (msg: MessageEvent) => void
let _peerReceiver: SimplePeer | null = null

const destroyReceiver = () => {
  if (_peerReceiver) {
    _peerReceiver.destroy()
  }
}

const prepareForNewConnection = () => {
  let controllerType = ''

  const cleanup = () => {
    window.removeEventListener('message', _handleNextOffer)
  }

  // Remove any existing handlers for a new connection and attach a new handler.
  cleanup()

  /**
   * The primary handler when data is received over WebRTC.
   */
  const onData = (data) => {
    const parsed = JSON.parse(data)
    const isControllerUpdate = parsed.actionName === RemoteAction.CONTROLLER_POSITION
    const isControllerAction = parsed.actionName === RemoteAction.CONTROLLER_ACTION

    if (isControllerUpdate) {
      const controllerPositionEvent = new CustomEvent('controller-position-update', {
        detail: parsed.actionData,
      })
      window.XrRemote8.controllerPosition.dispatchEvent(controllerPositionEvent)
    } else if (isControllerAction) {
      const controllerActionEvent = new CustomEvent(parsed.actionData.remoteEventName, {
        detail: parsed.actionData,
      })
      window.XrRemote8.controller.dispatchEvent(controllerActionEvent)
    }
  }

  destroyReceiver()

  // Create a new receiver instance to handle the incoming data
  const peerReceiver = createNewP2PReceiver({
    onData,
    onSignal: (data) => {
      sendWebRtcAnswerToXrHome(data)
    },
    // Once connection is complete, we can remove the initial connection handler.
    onConnectionSuccess: () => {
      sendWebRtcConnectionSuccessEventToXrHome()
      window.XrRemote8.setConnected(true)
      window.XrRemote8.updateControllerType(controllerType)
      cleanup()
    },
    // If something goes wrong during setup, remove the handler as well.
    onConnectionClosed: () => {
      sendWebRtcConnectionClosedEventToXrHome()
      window.XrRemote8.cleanup()
      cleanup()
    },
    onConnectionError: () => {
      sendWebRtcConnectionErrorEventToXrHome()
      window.XrRemote8.cleanup()
      cleanup()
    },
  })

  _peerReceiver = peerReceiver
  _handleNextOffer = (msg: MessageEvent) => {
    controllerType = msg?.data?.data?.controllerType ?? ''
    handleWebRtcOffer(msg, peerReceiver)
  }

  window.addEventListener('message', _handleNextOffer)
}

const handleNewConnectionViaPostMessage = (msg: MessageEvent) => {
  const isNewConnection = msg?.data?.action === SimulatorHMDLinkActionType.CONNECT
  const isDisconnectRequest = msg?.data?.action === SimulatorHMDLinkActionType.DISCONNECT
  const isRecenterRequest = msg?.data?.action === SimulatorHMDLinkActionType.RECENTER
  const isControllerTypeChangeRequest =
    msg?.data?.action === SimulatorHMDLinkActionType.CHANGE_CONTROLLER_TYPE

  if (isNewConnection) {
    prepareForNewConnection()
  } else if (isDisconnectRequest) {
    destroyReceiver()
    window.XrRemote8.cleanup()
  } else if (isRecenterRequest) {
    window.XrRemote8.setNeedsOriginSync()
  } else if (isControllerTypeChangeRequest) {
    window.XrRemote8.updateControllerType(msg?.data?.data?.controllerType)
  }
}

const initXrRemote = () => {
  // When the simulator unloads, tell XRHome to cleanup the P2P session
  window.addEventListener('beforeunload', sendUnloadEventToXrHome)

  // By default, the simulator just waits patiently for a desire by the developer to connect
  // and HMD. Whenever XRHome signals its time to connect, this fires.
  window.addEventListener('message', handleNewConnectionViaPostMessage)

  // Create the XrRemote instance for interacting with remote events
  const xrRemoteInstance = new XRRemote()

  // Put XrRemote instance on the window for use by developers (mainly if they don't use aframe)
  window.XrRemote8 = xrRemoteInstance

  sendReadyForNewConnectionEventToXrHome()

  // Register aframe integration if possible
  import('./aframe/xr-remote-hand-controls').then(({register}) => {
    register(xrRemoteInstance)
  })
}

const removeXrRemote = () => {
  window.removeEventListener('beforeunload', sendUnloadEventToXrHome)
  window.removeEventListener('message', handleNewConnectionViaPostMessage)

  if (window.XrRemote8) {
    window.XrRemote8.cleanup()
  }
}

export {
  initXrRemote,
  removeXrRemote,
}
