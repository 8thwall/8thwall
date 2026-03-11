import {useWindowMessageHandler} from '../../hooks/use-window-message-handler'

type IPostMessageBridge = {
  isActive: boolean
  parentActions: string[]
  childActions: string[]
  iframeEl: HTMLIFrameElement | null
}

const useStandalonePostMessageBridge = (
  {isActive, iframeEl, parentActions, childActions}: IPostMessageBridge
) => {
  useWindowMessageHandler((msg) => {
    const {action} = msg.data
    const isFromParent = msg.source === window.opener
    const isFromChild = msg.source === iframeEl?.contentWindow

    // From the parent, forward the event to the iframe.
    // From the child, forward the event to the opener
    if (isFromParent) {
      if (parentActions.includes(action)) {
        if (iframeEl) {
          iframeEl.contentWindow?.postMessage(msg.data, '*')
        }
      }
    } else if (isFromChild) {
      if (childActions.includes(action)) {
        if (window.opener) {
          window.opener.postMessage(msg.data, '*')
        }
      }
    }
  }, isActive)

  return null
}

export {
  useStandalonePostMessageBridge,
}
