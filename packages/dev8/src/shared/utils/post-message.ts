const postMessage = (action, data) => {
  if (!window.parent || window.parent === window) {
    return
  }
  window.parent.postMessage({
    action,
    data,
  }, '*')
}

export {
  postMessage,
}
