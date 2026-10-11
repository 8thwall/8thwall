import type {StartFlowOptions, StartFlowStringOverrides} from './types'

const queryAttribute = (selector: string, attribute: string) => (
  document.querySelector(selector)?.getAttribute(attribute) ?? undefined
)

const queryContent = (selector: string) => (
  queryAttribute(selector, 'content')?.trim() || undefined
)

const getOpenGraphContent = (property: string) => (
  queryContent(`meta[property="${property}"]`) ??
  queryContent(`meta[name="${property}"]`)
)

const resolveDefaultStringOverrides = (): StartFlowStringOverrides => {
  const documentTitle = document.title.trim() || undefined
  const startTitle = getOpenGraphContent('og:title') ?? documentTitle
  const startDescription = getOpenGraphContent('og:description') ??
    queryContent('meta[name="description"]')
  return {
    ...(startTitle === undefined ? {} : {startTitle}),
    ...(startDescription === undefined ? {} : {startDescription}),
  }
}

const resolveDefaultOptions = (): StartFlowOptions => {
  const ogImage = getOpenGraphContent('og:image')
  return {
    previewSrc: ogImage ?? null,
    showStartScreen: false,
  }
}

export {resolveDefaultOptions, resolveDefaultStringOverrides}
