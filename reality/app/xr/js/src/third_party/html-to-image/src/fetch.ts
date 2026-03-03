const fetch = (url, options) => {
  if (options.fetchOptions) {
    return window.fetch(url, options.fetchOptions)
  } else {
    return window.fetch(url)
  }
}

export {
  fetch,
}
