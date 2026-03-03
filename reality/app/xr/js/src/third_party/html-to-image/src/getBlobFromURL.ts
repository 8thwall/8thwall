import {fetch} from './fetch'
import * as util_1 from './util'
import {logError} from './log'
import {cacheGet, cacheSet} from './cache'

function getCacheKey(url) {
  let key = url.replace(/\?.*/, '')
  // font resourse
  if (/ttf|otf|eot|woff2?/i.test(key)) {
    key = key.replace(/.*\//, '')
  }
  return key
}
function getBlobFromURL(url, options) {
  const cacheKey = getCacheKey(url)
  const cached = cacheGet('blob', cacheKey, options)
  if (cached != null) {
    return cached
  }
  // cache bypass so we dont have CORS issues with cached images
  // ref: https://developer.mozilla.org/en/docs/Web/API/XMLHttpRequest/Using_XMLHttpRequest#Bypassing_the_cache
  if (options.cacheBust) {
    // eslint-disable-next-line no-param-reassign
    url += (/\?/.test(url) ? '&' : '?') + new Date().getTime()
  }
  const failed = (reason) => {
    let placeholder = ''
    let contentType = ''
    if (options.imagePlaceholder) {
      const parts = options.imagePlaceholder.split(/,/)
      if (parts && parts.length === 2) {
        ([contentType, placeholder] = parts)
      }
    }
    let msg = `Failed to fetch resource: ${url}`
    if (reason) {
      msg = typeof reason === 'string' ? reason : reason.message
    }
    if (msg) {
      logError(options, msg)
    }
    return {
      blob: placeholder,
      contentType,
    }
  }
  const deferred = fetch(url, options)
    .then(res => res.blob().then(blob => ({
      blob,
      contentType: res.headers.get('Content-Type') || '',
    })))

    .then(({blob, contentType}) => new Promise((resolve, reject) => {
      const reader = new FileReader()
      reader.onloadend = () => resolve({
        contentType,
        blob: reader.result,
      })
      reader.onerror = reject
      reader.readAsDataURL(blob)
    }))
    .then(({blob, contentType}) => ({
      contentType,
      blob: util_1.parseDataUrlContent(blob),
    }))
  // on failed
    .catch(failed)
    // cache result
  cacheSet('blob', cacheKey, deferred, options)
  return deferred
}

export {
  getBlobFromURL,
}
