const LOCALE_URL_PARAM_NAME = 'lang'
const LOCALE_COGNITO_ATTR_NAME = 'locale'
const LOCALE_COOKIE_NAME = '_locale'
const SSR_NAMESPACES = ['public-featured-pages', 'navigation', 'common']

const SUPPORTED_LOCALES_LIGHTSHIP = ['en-US', 'ja-JP'] as const
type SupportedLocaleLightship = typeof SUPPORTED_LOCALES_LIGHTSHIP[number]

export {
  LOCALE_URL_PARAM_NAME,
  LOCALE_COGNITO_ATTR_NAME,
  LOCALE_COOKIE_NAME,
  SUPPORTED_LOCALES_LIGHTSHIP,
  SSR_NAMESPACES,
}

export type {
  SupportedLocaleLightship,
}
