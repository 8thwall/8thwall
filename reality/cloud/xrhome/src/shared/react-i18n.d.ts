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

type TranslationPluralSuffix = 'zero' | 'one' | 'two' | 'few' | 'many' | 'other'
type ResourceKeys<T> = {
  [K in keyof T & string]: T[K] extends string
    ? K | (K extends `${infer Base}_${TranslationPluralSuffix}` ? Base : never)
    : never
}[keyof T & string]

type TranslationNamespace = keyof TranslationResources
type TranslationNamespaceTuple = [TranslationNamespace, ...TranslationNamespace[]]
type TranslationSelection = TranslationNamespace | TranslationNamespaceTuple
type ResourceKeysByNamespace = {
  [N in TranslationNamespace]: ResourceKeys<TranslationResources[N]>
}
type TranslationNamespaceKeys<N extends TranslationNamespace> = ResourceKeysByNamespace[N]
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
interface StrictTranslationFunction<N> {
  <TResult extends TFunctionResult | ReactNode = string>(
    key: DefaultNamespaceOptionKeys<N> | DefaultNamespaceOptionKeys<N>[],
    options?: TranslationOptionsWithoutNamespace | string,
  ): TResult
  <TResult extends TFunctionResult | ReactNode = string>(
    key: DefaultNamespaceOptionKeys<N>,
    defaultValue?: string,
    options?: TranslationOptionsWithoutNamespace | string,
  ): TResult
  <NS extends SelectedNamespaces<N>>(
    key: TranslationNamespaceKeys<NS>,
    options: TranslationOptionsWithNamespace<NS>,
  ): string
  <NS extends SelectedNamespaces<N>>(
    key: TranslationNamespaceKeys<NS>,
    defaultValue: string,
    options: TranslationOptionsWithNamespace<NS>,
  ): string
}
type CheckedTranslationSelection<N> = N extends TranslationSelection ? N : never
type ResolvedTranslationSelection<N> = N extends TranslationSelection
  ? N
  : TranslationNamespace[]
type TypedUseTranslationResponse<N> = [StrictTranslationFunction<N>, i18n, boolean] & {
  t: StrictTranslationFunction<N>
  i18n: i18n
  ready: boolean
}

declare module 'react-i18next' {
  interface CustomTypeOptions {
    defaultNS: 'common'
    resources: TranslationResources
  }

  interface TFunction<
    N extends import('react-i18next').Namespace,
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    _TKPrefix,
  > extends StrictTranslationFunction<N> {}

  function useTranslation<N extends string | TranslationNamespaceTuple>(
    ns: CheckedTranslationSelection<N> | Readonly<CheckedTranslationSelection<N>>,
    options?: import('react-i18next').UseTranslationOptions
  ): TypedUseTranslationResponse<ResolvedTranslationSelection<N>>
}
