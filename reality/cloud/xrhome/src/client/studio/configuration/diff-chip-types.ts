import type React from 'react'

import type {DeepReadonly, Primitive} from 'ts-essentials'

import type {DiffType} from '../get-diff-type'

type DiffBaseProps<PATHLIST extends readonly string[][]> = {
  defaults?: { [K in keyof PATHLIST]: Primitive }
}

type DefaultRenderConfig<VALUETYPE> =
  | {
    defaultRenderValue?: never
    defaultRenderDiff?: never
  }
  | {
    defaultRenderValue: DiffProps<VALUETYPE, string[][]>['renderValue']
    defaultRenderDiff?: never
  }
  | {
    defaultRenderDiff: DiffProps<VALUETYPE, string[][]>['renderDiff']
    defaultRenderValue?: never
  }

type PathConfig<PATHLIST extends readonly string[][]> =
  | {finalPaths: PATHLIST}

type DiffRenderInfo = {
  type: DiffType<unknown>['type'] | null
  bannerContent: React.ReactNode
}

type RenderConfig<VALUETYPE> =
  | {
    renderDiff: (
      prev: DeepReadonly<VALUETYPE>,
      now: DeepReadonly<VALUETYPE>
    ) => DiffRenderInfo
    renderValue?: never
  }
  | {
    renderValue: (val: DeepReadonly<VALUETYPE>) => React.ReactNode
    renderDiff?: never
  }

type DefaultableRenderConfig<VALUETYPE> = RenderConfig<VALUETYPE>
  | {
    renderDiff?: never
    renderValue?: never
  }

type DiffProps<VALUETYPE, PATHLIST extends readonly string[][]> =
  PathConfig<PATHLIST> &
  RenderConfig<VALUETYPE> &
  DiffBaseProps<PATHLIST>

type DefaultableDiffProps<VALUETYPE, PATHLIST extends readonly string[][]> =
  PathConfig<PATHLIST> &
  DefaultableRenderConfig<VALUETYPE> &
  DiffBaseProps<PATHLIST>

type ConfigDiffInfo<VALUETYPE, PATHLIST extends readonly string[][]> =
  RenderConfig<VALUETYPE> &
  DiffBaseProps<PATHLIST>

type DefaultableConfigDiffInfo<VALUETYPE, PATHLIST extends readonly string[][]> =
  DefaultableRenderConfig<VALUETYPE> &
  DiffBaseProps<PATHLIST>

export type {
  DiffBaseProps,
  DiffProps,
  ConfigDiffInfo,
  DefaultableConfigDiffInfo,
  DiffRenderInfo,
  DefaultableDiffProps,
  DefaultRenderConfig,
}
