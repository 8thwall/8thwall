import type {DebugMessage, DebugCallback} from './shared/ecs/shared/debug-messaging'

const createStudioEventStreamManager = (
  getActiveWebsocket: () => WebSocket | null
) => {
  const messageListenerCallbacks = new Set<DebugCallback>()

  const handleMessage = (msg: DebugMessage) => {
    const callbacks = [...messageListenerCallbacks]
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
    handleMessage(msg)
  }

  const handlePostMessage = (msg: DebugMessage) => {
    // console.log('===== dev8:POST MESSAGE =====', msg.action)
    handleMessage(msg)
  }

  const sendViaSockets = ({action, ...data}: DebugMessage) => {
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
  const getPostMessageTarget = (): Window => {
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

  const sendViaPostMessage = (data: DebugMessage) => {
    const postMessageTarget = getPostMessageTarget()
    if (!postMessageTarget) {
      return false
    }
    postMessageTarget.postMessage(data, '*')
    return true
  }

  /**
   * Abstracts the transport for sending data to XRHome.
   */
  const send = (payload: DebugMessage) => {
    if (sendViaPostMessage(payload)) {
      return
    }
    sendViaSockets(payload)
  }

  const listen = (callback: DebugCallback) => {
    messageListenerCallbacks.add(callback)
  }

  const cancelListen = (callback: DebugCallback) => {
    messageListenerCallbacks.delete(callback)
  }

  return {
    handleSocketMessage,
    handlePostMessage,
    send,
    listen,
    cancelListen,
  }
}

export {
  createStudioEventStreamManager,
}
