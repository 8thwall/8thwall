const createRunLoop = (callback: (timestamp: number) => void) => {
  let queued: ReturnType<typeof requestAnimationFrame>

  const loop = (timestamp: number) => {
    callback(timestamp)
    // eslint-disable-next-line @typescript-eslint/no-use-before-define
    queue()
  }

  const queue = () => {
    queued = requestAnimationFrame(loop)
  }

  const start = () => {
    cancelAnimationFrame(queued)
    queue()
  }

  const stop = () => {
    cancelAnimationFrame(queued)
  }

  return {
    start,
    stop,
  }
}

export {
  createRunLoop,
}
