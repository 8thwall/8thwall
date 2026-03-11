const getQueryParams = (url: URL) => Object.fromEntries(url.searchParams)

export {
  getQueryParams,
}
