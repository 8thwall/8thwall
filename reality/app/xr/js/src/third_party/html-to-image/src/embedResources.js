import * as getBlobFromURL_1 from './getBlobFromURL'
import * as util_1 from './util'
import {cacheGet, cacheSet} from './cache'

const URL_REGEX = /url\((['"]?)([^'"]+?)\1\)/g
const URL_WITH_FORMAT_REGEX = /url\([^)]+\)\s*format\((["'])([^"']+)\1\)/g
const FONT_SRC_REGEX = /src:\s*(?:url\([^)]+\)\s*format\([^)]+\)[,;]\s*)+/g
function toRegex(url) {
  // eslint-disable-next-line no-useless-escape
  const escaped = url.replace(/([.*+?^${}()|\[\]\/\\])/g, '\\$1')
  return new RegExp(`(url\\(['"]?)(${escaped})(['"]?\\))`, 'g')
}
function parseURLs(cssText) {
  const result = []
  cssText.replace(URL_REGEX, (raw, quotation, url) => {
    result.push(url)
    return raw
  })
  return result.filter(url => !util_1.isDataUrl(url))
}
function embed(cssText, resourceURL, baseURL, options, get) {
  const resolvedURL = baseURL ? util_1.resolveUrl(resourceURL, baseURL) : resourceURL
  return Promise.resolve(resolvedURL)
    .then(url => (get ? get(url) : getBlobFromURL_1.getBlobFromURL(url, options)))
    .then((data) => {
      if (typeof data === 'string') {
        return util_1.makeDataUrl(data, util_1.getMimeType(resourceURL))
      }
      return util_1.makeDataUrl(data.blob, util_1.getMimeType(resourceURL) || data.contentType)
    })
    .then(dataURL => cssText.replace(toRegex(resourceURL), `$1${dataURL}$3`))
    .then(content => content, () => resolvedURL)
}

/**
 * Go through and find the first preferred format.
 */
function filterPreferredFontFormat(str, options) {
  const {preferredFontFormat} = options

  if (!preferredFontFormat) {
    return str
  }

  // If the the font is already embedded, punt. Should be enough to take a peek at the first bit.
  // This keeps us from regex'ing through nearly 100 KBs.
  // TODO(pawel) Verify that these embedded fonts only have one format in them.
  // Since we removed embedding at the embedWebFonts stage, we should be good.
  if (str.substring(0, 300).indexOf('data:font/') !== -1) {
    return str
  }

  let formatPriorities
  if (Array.isArray(preferredFontFormat)) {
    formatPriorities = preferredFontFormat
  } else {
    formatPriorities = [preferredFontFormat]
  }

  const cacheKey = 'filter-font-format/'.concat(formatPriorities.join(','))

  // Instead of running regex on the string let's see if we computed an answer already.
  const cache = cacheGet(cacheKey, str, options)
  if (cache) {
    return cache
  }

  const res = str.replace(FONT_SRC_REGEX, (match) => {
    let bestFormat = ''

    for (let i = 0; i < formatPriorities.length; i++) {
      const format = formatPriorities[i]
      if (match.indexOf(`format("${format}")`) !== -1 ||
        match.indexOf(`format('${format}')`) !== -1
      ) {
        bestFormat = format
        break
      }
    }

    let firstEncounteredSrc = ''

    // RegExp.exec() uses internal state on the regex object so create a copy.
    const urlWithFormatRegex = new RegExp(URL_WITH_FORMAT_REGEX)

    // eslint-disable-next-line no-constant-condition
    while (true) {
      const [src, , format] = urlWithFormatRegex.exec(match) || []
      firstEncounteredSrc = firstEncounteredSrc || src
      if (!format) {
        return `src: ${firstEncounteredSrc};`
      }
      if (format === bestFormat) {
        return `src: ${src};`
      }
    }
  })

  cacheSet(cacheKey, str, res, options)
  return res
}
function shouldEmbed(url) {
  return url.search(URL_REGEX) !== -1
}

function embedResources(cssText, baseUrl, options) {
  if (!shouldEmbed(cssText)) {
    return Promise.resolve(cssText)
  }
  const filteredCSSText = filterPreferredFontFormat(cssText, options)
  return Promise.resolve(filteredCSSText)
    .then(parseURLs)
    .then(urls => urls.reduce((deferred, url) => (
      deferred.then(css => embed(css, url, baseUrl, options))
    ), Promise.resolve(filteredCSSText)))
}

export {
  embed,
  embedResources,
  parseURLs,
  shouldEmbed,
  toRegex,
}
