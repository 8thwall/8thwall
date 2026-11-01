import {defaultStrings} from './strings'
import type {StartFlowOptions, StartFlowStrings} from './types'

const queryAttribute = (selector: string, attribute: string) => (
  document.querySelector(selector)?.getAttribute(attribute)
)

const queryContent = (selector: string) => (
  queryAttribute(selector, 'content')?.trim()
)

const getOpenGraphContent = (property: string) => (
  queryContent(`meta[property="${property}"]`) ||
  queryContent(`meta[name="${property}"]`)
)

const getShortUrl = () => {
  const shortLink = queryContent('meta[name="8thwall:shortlink"]')
  if (!shortLink || /^https?:\/\//.test(shortLink)) {
    return shortLink
  }
  return `https://8th.io/${shortLink}`
}

const resolveDefaultStrings = (): StartFlowStrings => ({
  ...defaultStrings,
  startTitle: (
    getOpenGraphContent('og:title') ||
    document.title.trim() ||
    defaultStrings.startTitle
  ),
  startDescription: (
    getOpenGraphContent('og:description') ||
    queryContent('meta[name="description"]') ||
    defaultStrings.startDescription
  ),
})

const resolveDefaultOptions = (): StartFlowOptions => {
  const ogImage = getOpenGraphContent('og:image')
  return {
    previewSrc: ogImage ?? null,
    showStartScreen: false,
    waitForCameraTexture: true,
    url: getShortUrl() ||
      queryAttribute('link[rel="canonical"]', 'href') ||
      getOpenGraphContent('og:url') ||
      window.location.href,
    mediaSrc: ogImage ?? null,
    logoSrc: null,
    mediaAutoplay: true,
    mediaAnimation: '',
    mediaControls: 'minimal',
    sceneEnvMap: 'field',
    sceneOrbitIdle: 'spin',
    sceneOrbitInteraction: 'drag',
    sceneLightingIntensity: 1,
  }
}

export {resolveDefaultOptions, resolveDefaultStrings}
