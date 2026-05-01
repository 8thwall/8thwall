const postMessage = (action, data) => {
  if (!window.parent || window.parent === window) {
    return
  }
  window.parent.postMessage({
    action,
    data,
  }, '*')
}

// Just a different data format
const studioPostMessage = (targetWindow, action, data) => {
  targetWindow.postMessage({
    action,
    ...data,
  }, '*')
}

export {
  postMessage,
  studioPostMessage,
}
