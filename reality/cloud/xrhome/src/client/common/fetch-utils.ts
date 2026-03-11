const getHeaders = (Authorization, {json = true, headers = {}}) => Object.assign(
  json ? {'Content-Type': 'application/json'} : {},
  Authorization ? {Authorization} : {},
  headers
)

const responseToJsonOrText = async (res: Response) => {
  const contentType = res.headers.get('Content-Type')
  const isJsonResponse = contentType && contentType.indexOf('json') > 0
  if (!res.ok) {
    const message: string = isJsonResponse
      ? await res.json().then(body => body.message)
      : await res.clone().text()

    // Preserve status code for whoever is interested.
    throw Object.assign(new Error(message), {status: res.status})
  }

  if (isJsonResponse) {
    return res.json()
  }
  return res.text()
}

const fetchSize = async (url) => {
  try {
    const res = await fetch(url, {method: 'HEAD'})
    return Number.parseInt(res.headers.get('content-length'), 10) || undefined
  } catch (err) {
    return undefined
  }
}

export {
  getHeaders,
  responseToJsonOrText,
  fetchSize,
}
