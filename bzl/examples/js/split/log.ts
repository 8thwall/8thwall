// @attr(target_compatible_with = BROWSER_ONLY)

const log = (message: string) => {
  document.body.appendChild(Object.assign(document.createElement('p'), {
    textContent: `${performance.now().toFixed(1)} ${message}`,
  }))
}

export {
  log,
}
