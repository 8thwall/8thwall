const resolveXr8 = () => {
  if ((window as any).loadXr8) {
    (window as any).loadXr8()
  }
}

const ensureXr8Loaded = new Promise<void>((resolve) => {
  if ((window as any).XR8) {
    resolve()
    return
  }

  window.addEventListener('xrloaded', () => {
    resolve()
  })
})

export {resolveXr8, ensureXr8Loaded}
