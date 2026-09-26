import 'react-i18next'
import type {ReactNode} from 'react'
import type {i18n, TFunctionResult, TOptions} from 'i18next'

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
    : T[K] extends object
      ? `${K}.${TranslationKeys<T[K]> & string}`
      : never
}[keyof T & string]

type TranslationNamespace = keyof TranslationResources
type TranslationNamespaceKeys<N extends TranslationNamespace> = TranslationKeys<
  TranslationResources[N]
>
type AllTranslationKeys = {
  [K in TranslationNamespace]: TranslationNamespaceKeys<K>
}[TranslationNamespace]
type TranslationOptionsWithoutNamespace = Omit<TOptions, 'ns'> & {ns?: never}
type TranslationOptionsWithNamespace<N extends TranslationNamespace> = Omit<TOptions, 'ns'> & {
  ns: N
}
type NamespaceOf<N> = N extends readonly (infer Namespace)[] ? Namespace : N
type SelectedNamespaces<N> = Extract<NamespaceOf<N>, TranslationNamespace>
type DefaultNamespace<N> = N extends readonly [infer Namespace, ...unknown[]]
  ? Namespace
  : NamespaceOf<N>
type DefaultNamespaceOptionKeys<N> = TranslationNamespaceKeys<
  Extract<DefaultNamespace<N>, TranslationNamespace>
>
type CheckedTranslationKey<K extends string, ValidKey extends string> = K extends ValidKey
  ? K
  : string extends K
    ? K
    : never
type StrictTranslationFunction<N> = {
  <K extends string, TResult extends TFunctionResult | ReactNode = string>(
    key: CheckedTranslationKey<K, DefaultNamespaceOptionKeys<N>> |
      CheckedTranslationKey<K, DefaultNamespaceOptionKeys<N>>[],
    options?: TranslationOptionsWithoutNamespace | string,
  ): TResult
  <K extends string, TResult extends TFunctionResult | ReactNode = string>(
    key: CheckedTranslationKey<K, DefaultNamespaceOptionKeys<N>>,
    defaultValue?: string,
    options?: TranslationOptionsWithoutNamespace | string,
  ): TResult
  <K extends string, NS extends SelectedNamespaces<N>>(
    key: CheckedTranslationKey<K, TranslationNamespaceKeys<NS>>,
    options: TranslationOptionsWithNamespace<NS>,
  ): string
  <K extends string, NS extends SelectedNamespaces<N>>(
    key: CheckedTranslationKey<K, TranslationNamespaceKeys<NS>>,
    defaultValue: string,
    options: TranslationOptionsWithNamespace<NS>,
  ): string
  <K extends string, NS extends TranslationNamespace>(
    key: CheckedTranslationKey<K, NS extends SelectedNamespaces<N> ? never : AllTranslationKeys>,
    options: TranslationOptionsWithNamespace<NS & SelectedNamespaces<N>>,
  ): string
}
type TypedTranslationFunction<N> = StrictTranslationFunction<N>
type TypedUseTranslationResponse<N> = [TypedTranslationFunction<N>, i18n, boolean] & {
  t: TypedTranslationFunction<N>
  i18n: i18n
  ready: boolean
}

declare module 'react-i18next' {
  interface CustomTypeOptions {
    defaultNS: 'common'
    resources: TranslationResources
  }

  export type CustomTFunction<N = TranslationNamespace[]> = StrictTranslationFunction<N>

  type TFunctionKey<N> = N extends readonly TranslationNamespace[]
    ? DefaultNamespaceOptionKeys<N>
    : AllTranslationKeys

  interface TFunction<
    N extends import('react-i18next').Namespace = 'common',
    TKPrefix = undefined,
  > {
    <K extends string>(
      key: CheckedTranslationKey<K, TFunctionKey<N> | (TKPrefix extends never ? never : never)>,
      options?: TranslationOptionsWithoutNamespace | string,
    ): string
    <K extends string>(
      key: CheckedTranslationKey<K, TFunctionKey<N> | (TKPrefix extends never ? never : never)>,
      defaultValue?: string,
      options?: TranslationOptionsWithoutNamespace | string,
    ): string
    <K extends string, NS extends SelectedNamespaces<N>>(
      key: CheckedTranslationKey<K, TranslationNamespaceKeys<NS> | TFunctionKey<N>>,
      options: TranslationOptionsWithNamespace<NS>,
    ): string
    <K extends string, NS extends SelectedNamespaces<N>>(
      key: CheckedTranslationKey<K, TranslationNamespaceKeys<NS> | TFunctionKey<N>>,
      defaultValue: string,
      options: TranslationOptionsWithNamespace<NS>,
    ): string
    <K extends string, NS extends TranslationNamespace>(
      key: CheckedTranslationKey<K, NS extends SelectedNamespaces<N> ? never : AllTranslationKeys>,
      options: TranslationOptionsWithNamespace<NS & SelectedNamespaces<N>>,
    ): string
  }

  function useTranslation(): never

  function useTranslation<N extends readonly [TranslationNamespace, ...TranslationNamespace[]]>(
    ns: N,
    options?: import('react-i18next').UseTranslationOptions
  ): TypedUseTranslationResponse<N>

  function useTranslation<N extends readonly TranslationNamespace[]>(
    ns: N,
    options?: import('react-i18next').UseTranslationOptions
  ): TypedUseTranslationResponse<N>

  function useTranslation<N extends TranslationNamespace>(
    ns: N,
    options?: import('react-i18next').UseTranslationOptions
  ): TypedUseTranslationResponse<N>

  function useTranslation<N extends string>(
    ns: N & (string extends N ? string : N extends TranslationNamespace ? N : never),
    options?: import('react-i18next').UseTranslationOptions
  ): TypedUseTranslationResponse<TranslationNamespace[]>
}
