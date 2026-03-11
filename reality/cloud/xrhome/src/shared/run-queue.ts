// @attr(visibility = ["//visibility:public"])

const makeRunQueue = (maxParallelRuns: number = 1) => {
  let currentRuns = 0
  const queued = [] as (() => void)[]

  const next: <T>(work: () => Promise<T>) => Promise<T> = async (work) => {
    if (currentRuns < maxParallelRuns) {
      currentRuns++
    } else {
      await new Promise<void>(resolve => queued.push(resolve))
    }

    try {
      return await work()
    } finally {
      if (queued.length) {
        queued.shift()!()
      } else {
        currentRuns--
      }
    }
  }

  return {
    next,
  }
}

export {
  makeRunQueue,
}
