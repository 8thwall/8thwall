import type {LandingParameters} from './parameters'

const queryElementAttribute = (selector: string, attribute: string) => (
  document.querySelector(selector)?.getAttribute(attribute)
)

const getShortUrl = () => {
  const shortLink = queryElementAttribute('meta[name="8thwall:shortlink"]', 'content')
  if (!shortLink) return null

  if (shortLink.startsWith('http://') || shortLink.startsWith('https://')) {
    return shortLink
  }

  return `https://8th.io/${shortLink}`
}

const getCanonicalUrl = () => queryElementAttribute('link[rel="canonical"]', 'href')

const getOgUrl = () => queryElementAttribute('meta[property="og:url"]', 'content')

const getOgImage = () => queryElementAttribute('meta[property="og:image"]', 'content')

// Infer parameters from the <head> if missing.
// For URL, 8thwall:shortlink meta tag is preferred, followed by the canonical URL, followed by
//   og:url, followed by the current URL of the page.
// For image, use the og:image if unspecified.
const applyInferredParameters = (baseParams: LandingParameters): LandingParameters => ({
  ...baseParams,
  url: baseParams.url || getShortUrl() || getCanonicalUrl() || getOgUrl() || window.location.href,
  mediaSrc: baseParams.mediaSrc || getOgImage(),
})

export {
  applyInferredParameters,
}
