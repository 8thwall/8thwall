import type SimplePeer from 'simple-peer'

import {createNewP2PReceiver} from './shared/webrtc/p2p-connect/receiver'
import {studioPostMessage} from './shared/utils/post-message'
import type {DebugMessage} from './shared/ecs/shared/debug-messaging'

type EventStreamManager = {
  handleSocketMessage: (msg: DebugMessage) => void
  handlePostMessage: (msg: DebugMessage) => void
  send: (message: DebugMessage, attemptPostMessage?: boolean, attemptWebrtc?: boolean) => void
  sendViaSockets: (message: DebugMessage) => void
  sendViaPostMessage: (message: DebugMessage) => void
  sendViaWebrtc: (message: DebugMessage) => void
  // Listen to an event message via some transport as desired
  // By default, callbacks will be registered to fire on postMessage and socket events
  listen: (
    callback: (message: DebugMessage) => void,
    config?: Partial<Pick<CallbackConfig, 'webrtc' | 'sockets' | 'postMessage'>>,
  ) => void
  cancelListen: (callback: (message: DebugMessage) => void) => void
}

type CallbackConfig = {
  callback: (msg: DebugMessage) => void
  postMessage: boolean
  webrtc: boolean
  sockets: boolean
}

type StudioWebRTCAnswerMessage = {
  action: 'CLOUD_STUDIO/WEBRTC_ANSWER'
  simulatorId: string
  webRTCData: unknown
}

const DEFAULT_CALLBACK_CONFIG = {
  // Since postMessage and sockets don't require any initial setup, by default
  // they will be eligible to receiving events.
  postMessage: true,
  sockets: true,

  // WebRTC requires some data passing between the 2 devices, which uses
  // either sockets or postMessage to do so. So default this is not enabled.
  webrtc: false,
}

const filterOnlySockets = c => c.sockets
const filterOnlyWebrtc = c => c.webrtc
const filterOnlyPostMessage = c => c.postMessage
const PAYLOAD_SIZE_LIMIT_BYTES = 250_000

// Dev can choose to add handler to run on
// only rtc, only socket, or both
// remove, removes from all
const createStudioEventStreamManager = (
  simulatorId: string,
  getActiveWebsocket: () => WebSocket | null
): EventStreamManager => {
  let _peerConnected = false
  let _peerReceiver: SimplePeer | null = null

  const messageListenerCallbacks = new Map<() => void, CallbackConfig>()

  const handleMessage = (msg: DebugMessage, filterApplied) => {
    const callbackConfigs = Array.from(messageListenerCallbacks.values())
    const callbacks = callbackConfigs.filter(filterApplied).map(c => c.callback)

    callbacks.forEach((callback) => {
      try {
        callback(msg)
      } catch (err) {
        // eslint-disable-next-line no-console
        console.error(err)
      }
    })
  }

  // Comment in to see which pipes events are flowing through in the EventStream
  const handleSocketMessage = (msg: DebugMessage) => {
    // console.log('===== dev8:SOCKET =====', msg.action)
    handleMessage(msg, filterOnlySockets)
  }

  const handleWebrtcMessage = (msg: DebugMessage) => {
    // console.log('===== dev8:WEBRTC =====', msg.action)
    handleMessage(msg, filterOnlyWebrtc)
  }

  const handlePostMessage = (msg: DebugMessage) => {
    // console.log('===== dev8:POST MESSAGE =====', msg.action)
    handleMessage(msg, filterOnlyPostMessage)
  }

  const sendViaSockets = ({action, ...data}: DebugMessage | StudioWebRTCAnswerMessage) => {
    const ws = getActiveWebsocket()

    ws?.send(JSON.stringify({
      action: 'BROADCAST',
      broadcast_data: {
        action,
        data,
        // Do not broadcast console to connections with deviceId
        FilterExpression: 'attribute_not_exists(deviceId)',
      },
    }))
  }

  /**
   * Discerns the correct window to postMessage to based on where dev8 is loading.
   */
  const getPostMessageTarget = () => {
    const isInIframe = window.parent && window.parent !== window
    const hasOpener = window.parent && window.parent.opener

    // dev8 is running in an inline simulator iframe
    const isInlineSimulator = isInIframe && !hasOpener
    // dev8 is running in an iframe in a different window opened by xrhome
    const isStandaloneIframedSimulator = isInIframe && hasOpener
    // dev8 is running in a new tab, not iframed
    const isNotIframedSimulator = !isInIframe

    if (isNotIframedSimulator) {
      // When running as a demo mode, we want to postMessage to the opener (XRHome root)
      return window.opener
    } else if (isInlineSimulator) {
      // When running as in inline simulator, we want to postMessage to the parent which should
      // be XRHome
      return window.parent
    } else if (isStandaloneIframedSimulator) {
      // When running in a new window or tab that XRHome opens, but we are iframed in the
      // app preview, we want to postMessage to the parent, which will forward the event to XRHome
      // root
      return window.parent
    }

    return null
  }

  // @mchen - Currently there is a bug where the peer never connects.
  // This happens when messages are either dropped or missed when completing the OFFER/ANSWER
  // process of the P2P connection. From some testing, I think its rooted in that the socket
  // being used to complete the handshake is the same socket used for all things
  // in dev8, like ECS updates. As a result, sometimes not all or none of the offer requests are
  // received or the answer responses are also not sent.
  const createNextReceiver = () => createNewP2PReceiver({
    onData: (data) => {
      const parsed = JSON.parse(data)
      handleWebrtcMessage(parsed)
    },
    onSignal: (data) => {
      const payload: StudioWebRTCAnswerMessage = {
        action: 'CLOUD_STUDIO/WEBRTC_ANSWER',
        webRTCData: data,
        simulatorId,
      }

      sendViaSockets(payload)
    },
    // Once connection is complete, we can remove the initial connection handler.
    onConnectionSuccess: () => {
      _peerConnected = true
    },
    // If something goes wrong during setup, remove the handler as well.
    onConnectionClosed: () => {
      _peerConnected = false
      _peerReceiver = createNextReceiver()
    },
    onConnectionError: () => {
      _peerConnected = false
      _peerReceiver = createNextReceiver()
    },
  })

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const handleWebRtcOfferEvents = (msg) => {
    try {
      switch (msg.action) {
        case 'CLOUD_STUDIO/WEBRTC_OFFER': {
          const forSimulatorId = msg.simulatorId

          // Only consume offers that are targeted for this simulator
          if (simulatorId !== forSimulatorId || !_peerReceiver) {
            return
          }

          const peerNeedsReInit = _peerReceiver.destroyed || _peerReceiver.destroying

          // In the event the peer has been destroyed, create a new one
          if (peerNeedsReInit) {
            _peerReceiver = createNextReceiver()
          }

          // Consume the offer, and respond with an answer.
          _peerReceiver.signal(msg.webRTCData)
          break
        }
        default:
      }
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error(error)
    }
  }

  const sendViaPostMessage = ({action, ...data}: DebugMessage) => {
    const postMessageTarget = getPostMessageTarget()

    studioPostMessage(postMessageTarget, action, data)
  }

  const sendViaWebrtc = ({action, ...data}: DebugMessage) => {
    if (!_peerReceiver) {
      return
    }

    const isPeerDestroyed = _peerReceiver.destroyed || _peerReceiver.destroying
    const peerIsConnectedAndAlive = _peerConnected && !isPeerDestroyed

    if (!peerIsConnectedAndAlive) {
      return
    }

    const payload = {
      action,
      ...data,
    }
    const payloadAsString = JSON.stringify(payload)
    const payloadSize = payloadAsString.length

    // If the payload is greater than 250 KB, don't send it to avoid the browser from
    // terminating the P2P connection.
    if (payloadSize > PAYLOAD_SIZE_LIMIT_BYTES) {
      // eslint-disable-next-line no-console,max-len
      console.error(`Attempted to send payload of size ${payloadSize} bytes over WebRTC! Message not sent`)
      return
    }

    // Hack check since SimplePeer's channel isn't ready immediately on creation.
    // So look at the private channel it has before sending
    try {
      _peerReceiver.send(payloadAsString)
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error(error)
    }
  }

  /**
   * Abstracts the transport for sending data to XRHome.
   */
  const send = (
    payload: DebugMessage,
    attemptPostMessage = true,
    attemptWebrtc = false
  ) => {
    const postMessageTarget = getPostMessageTarget()
    const canPostMessage = Boolean(postMessageTarget)
    const canSendViaWebrtc = _peerReceiver && _peerConnected

    if (attemptPostMessage && canPostMessage) {
      sendViaPostMessage(payload)
    } else if (attemptWebrtc && canSendViaWebrtc) {
      sendViaWebrtc(payload)
    } else {
      sendViaSockets(payload)
    }
  }

  const listen = (
    callback,
    config: Partial<Pick<CallbackConfig, 'webrtc' | 'sockets' | 'postMessage'>> = {}
  ) => {
    messageListenerCallbacks.set(callback, {
      callback,
      ...DEFAULT_CALLBACK_CONFIG,
      ...config,
    })
  }

  const cancelListen = (callback) => {
    messageListenerCallbacks.delete(callback)
  }

  // Commented out for now, no webRTC activated yet
  // Listen for webrtc connection events over postMessage or sockets
  // listen(handleWebRtcOfferEvents)
  // _peerReceiver = createNextReceiver()

  return {
    handleSocketMessage,
    handlePostMessage,
    send,
    sendViaSockets,
    sendViaPostMessage,
    sendViaWebrtc,
    listen,
    cancelListen,
  }
}

export {
  EventStreamManager,
  createStudioEventStreamManager,
}
