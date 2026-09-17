const XR8Promise = new Promise((resolve) => {
  if (window.XR8) {
    resolve(window.XR8)
  } else {
    window.addEventListener('xrloaded', () => resolve(window.XR8), {once: true})
  }
})

module.exports = {
  XR8Promise,
}
