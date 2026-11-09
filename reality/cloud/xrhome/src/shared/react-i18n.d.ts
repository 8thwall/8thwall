import 'react-i18next'

type TranslationResources = {
  'account-pages': typeof import('../client/i18n/en-US/account-pages.json')
  'app-pages': typeof import('../client/i18n/en-US/app-pages.json')
  'asset-lab': typeof import('../client/i18n/en-US/asset-lab.json')
  'caught-error-page': typeof import('../client/i18n/en-US/caught-error-page.json')
  'cloud-editor-pages': typeof import('../client/i18n/en-US/cloud-editor-pages.json')
  'cloud-studio-pages': typeof import('../client/i18n/en-US/cloud-studio-pages.json')
  'common': typeof import('../client/i18n/en-US/common.json')
  'public-featured-pages': typeof import('../client/i18n/en-US/public-featured-pages.json')
  'studio-desktop-pages': typeof import('../client/i18n/en-US/studio-desktop-pages.json')
  'studio-tooltips': typeof import('../client/i18n/en-US/studio-tooltips.json')
}

type TranslationKeys<T> = {
  [K in keyof T & string]: T[K] extends string
    ? K | (K extends `${infer Base}_${'one' | 'other'}` ? Base : never)
    : never
}[keyof T & string]

type TranslationNamespace = keyof TranslationResources

declare module 'react-i18next' {
  // Type translation-bearing data without changing TFunction or useTranslation yet.
  type TranslationKey<N extends TranslationNamespace> = TranslationKeys<TranslationResources[N]>
}
