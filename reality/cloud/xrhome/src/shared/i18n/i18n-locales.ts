import type {DeepReadonly} from 'ts-essentials'

const FALLBACK_LOCALE = 'en-US' as SupportedLocale8w

// Locales supported by 8th Wall builds.
const PROD_SUPPORTED_LOCALES_8W = [
  'en-US',
  'ja-JP',
  'fr-FR',
  'de-DE',
  'es-MX',
] as const
// Locales supported by 8th Wall dev builds.
const DEV_SUPPORTED_LOCALES_8W = [
  ...PROD_SUPPORTED_LOCALES_8W,
] as const
type SupportedLocale8w = typeof DEV_SUPPORTED_LOCALES_8W[number]
const getSupportedLocales8w = (): DeepReadonly<SupportedLocale8w[]> => (
  BuildIf.EXPERIMENTAL
    ? DEV_SUPPORTED_LOCALES_8W
    : PROD_SUPPORTED_LOCALES_8W
)

type LocaleOption8w = {
  value: SupportedLocale8w
  content: string
}

// eslint-disable-next-line arrow-parens
const filterUnsupported8w = <T>(localeMap: Partial<{[key in SupportedLocale8w]: T}>) => {
  const supportedSet = new Set(getSupportedLocales8w())
  Object.keys(localeMap).forEach((locale: SupportedLocale8w) => {
    if (!supportedSet.has(locale)) {
      delete localeMap[locale]
    }
  })
  return localeMap
}

const getSupportedLocale8wOptionsMap = () => filterUnsupported8w({
  'en-US': 'English',
  'ja-JP': '日本語',
  'fr-FR': 'Français',
  'de-DE': 'Deutsch',
  'es-MX': 'Español',
})
const getSupportedLocale8wOptions = (): LocaleOption8w[] => getSupportedLocales8w().map(locale => (
  {value: locale, content: getSupportedLocale8wOptionsMap()[locale]}
))

const validLocaleOrNull = (locale: string): SupportedLocale8w | null => (
  getSupportedLocales8w().find(supportedLocale => supportedLocale === locale) || null
)

const EXPORTS_FOR_TESTS = {
  PROD_SUPPORTED_LOCALES_8W,
  DEV_SUPPORTED_LOCALES_8W,
}

export {
  FALLBACK_LOCALE,
  getSupportedLocales8w,
  getSupportedLocale8wOptions,
  validLocaleOrNull,

  // NOTE: The values below are exported for unit tests only. They should not
  // be imported in production code.
  EXPORTS_FOR_TESTS,
}

export type {
  SupportedLocale8w,
}
