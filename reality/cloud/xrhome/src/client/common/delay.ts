const delayResolve = (timeMs: number): Promise<void> => new Promise((resolve) => {
  setTimeout(() => {
    resolve()
  }, timeMs)
})

export {
  delayResolve,
}
