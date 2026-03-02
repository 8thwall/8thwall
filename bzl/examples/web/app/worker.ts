const ctx: Worker = self as any  // eslint-disable-line no-restricted-globals

onmessage = (e) => {
  const {action} = e.data
  if (action === 'getTime') {
    ctx.postMessage(new Date().valueOf())
  }
}
