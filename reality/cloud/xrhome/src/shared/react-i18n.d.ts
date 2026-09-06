import 'react-i18next'
import type {i18n, TOptions} from 'i18next'

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
    ? K
    : T[K] extends object
      ? `${K}.${TranslationKeys<T[K]> & string}`
      : never
}[keyof T & string]

type TranslationNamespace = keyof TranslationResources
type TranslationNamespaceKeys<N extends TranslationNamespace> = TranslationKeys<TranslationResources[N]>
type TranslationNamespaceUnion<N> = N extends readonly (infer U)[]
  ? Extract<U, TranslationNamespace>
  : Extract<N, TranslationNamespace>
type TranslationFirstNamespace<N> = N extends readonly [infer F, ...unknown[]]
  ? Extract<F, TranslationNamespace>
  : TranslationNamespaceUnion<N>
type QualifiedTranslationKeys<N> = {
  [K in TranslationNamespaceUnion<N>]: `${K}:${TranslationNamespaceKeys<K>}`
}[TranslationNamespaceUnion<N>]
type TranslationOptionsWithoutNamespace = Omit<TOptions, 'ns'> & {ns?: never}
type TranslationOptions<N extends TranslationNamespace> = Omit<TOptions, 'ns'> & {ns: N}
type TranslationCall<N> =
  | [key: TranslationNamespaceKeys<TranslationFirstNamespace<N>>, options?: TranslationOptionsWithoutNamespace | string]
  | [key: QualifiedTranslationKeys<N>, options?: TOptions | string]
  | {
    [K in TranslationNamespaceUnion<N>]: [
      key: TranslationNamespaceKeys<K>,
      options: TranslationOptions<K>,
    ]
  }[TranslationNamespaceUnion<N>]
type TypedTranslationFunction<N> = {
  <K extends string>(
    ...args: string extends K ? [key: K, options?: TOptions | string] : TranslationCall<N>
  ): string
}
type TypedUseTranslationResponse<N> = [TypedTranslationFunction<N>, i18n, boolean] & {
  t: TypedTranslationFunction<N>
  i18n: i18n
  ready: boolean
}

declare module 'react-i18next' {
  interface CustomTypeOptions {
    defaultNS: 'common'
    resources: {
      'account-pages': TranslationResources['account-pages']
      'app-pages': TranslationResources['app-pages']
      'asset-lab': TranslationResources['asset-lab']
      'caught-error-page': TranslationResources['caught-error-page']
      'cloud-editor-pages': TranslationResources['cloud-editor-pages']
      'cloud-studio-pages': TranslationResources['cloud-studio-pages']
      'common': TranslationResources['common']
      'public-featured-pages': TranslationResources['public-featured-pages']
      'studio-desktop-pages': TranslationResources['studio-desktop-pages']
      'studio-tooltips': TranslationResources['studio-tooltips']
    }
  }

  function useTranslation<N extends readonly [TranslationNamespace, ...TranslationNamespace[]]>(
    ns: N,
    options?: import('react-i18next').UseTranslationOptions,
  ): TypedUseTranslationResponse<N>

  function useTranslation<N extends TranslationNamespace>(
    ns?: N,
    options?: import('react-i18next').UseTranslationOptions,
  ): TypedUseTranslationResponse<N>
}
