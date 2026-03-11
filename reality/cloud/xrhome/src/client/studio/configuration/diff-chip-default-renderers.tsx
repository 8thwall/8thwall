import React from 'react'

import type {DeepReadonly} from 'ts-essentials'

import type {Resource} from '@ecs/shared/scene-graph'

import {inferResourceObject} from '@ecs/shared/resource'

import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'

import {bannerFromRenderValue} from './diff-chip'
import {createThemedStyles} from '../../ui/theme'
import {SrOnly} from '../../ui/components/sr-only'
import {combine} from '../../common/styles'
import {useResourceUrl} from '../hooks/resource-url'
import {useVideoThumbnail} from '../hooks/use-video-thumbnail'
import {ASSET_EXT_TO_KIND} from '../common/studio-files'
import {fileExt} from '../../editor/editor-common'
import {derivePreciseEditValue} from './number-formatting'
import {csBlack} from '../../static/styles/settings'
import type {DefaultableDiffProps, DiffProps} from './diff-chip-types'

const useStyles = createThemedStyles(theme => ({
  groupDiffHolder: {
    maxHeight: '80%',
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '0.5em',
  },
  singleGroupValue: {
    height: '100%',
    maxHeight: '100%',
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '0.25em',
  },
  groupValueLabel: {
    color: theme.fgMuted,
    alignItems: 'center',
    display: 'flex',
  },
  image: {
    width: '100%',
    height: '100%',
    objectFit: 'cover',
  },
  insetImageContainer: {
    'height': '20px',
    'width': '20px',
    'aspectRatio': '1',
    'alignSelf': 'center',
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'cursor': 'pointer',
    'overflow': 'hidden',
    'borderRadius': '0.25em',
  },
  iconBacking: {
    background: csBlack,
    alignSelf: 'center',
    border: `1px solid ${theme.studioPanelBtnBg}`,
    display: 'flex',
  },
}))

const applyDefaultRenderValue = <VALUETYPE, PATHLIST extends readonly string[][]>(
  // eslint-disable-next-line arrow-parens
  passedProps: DefaultableDiffProps<VALUETYPE, PATHLIST> | undefined,
  defaultRenderValue: DiffProps<VALUETYPE, PATHLIST>['renderValue']
): DiffProps<VALUETYPE, PATHLIST> | undefined => {
  if (passedProps === undefined) {
    return undefined
  } else if (passedProps.renderDiff) {
    return passedProps as DiffProps<VALUETYPE, PATHLIST>
  } else if (!passedProps.renderValue) {
    return {
      ...passedProps,
      renderValue: defaultRenderValue,
    } as DiffProps<VALUETYPE, PATHLIST>
  }
  return passedProps as DiffProps<VALUETYPE, PATHLIST>
}

const applyDefaultRenderDiff = <VALUETYPE, PATHLIST extends readonly string[][]>(
  // eslint-disable-next-line arrow-parens
  passedProps: DefaultableDiffProps<VALUETYPE, PATHLIST> | undefined,
  defaultRenderDiff: DiffProps<VALUETYPE, PATHLIST>['renderDiff']
): DiffProps<VALUETYPE, PATHLIST> | undefined => {
  if (passedProps === undefined) {
    return undefined
  } else if (passedProps.renderValue) {
    return passedProps as DiffProps<VALUETYPE, PATHLIST>
  } else if (!passedProps.renderDiff) {
    return {
      ...passedProps,
      renderDiff: defaultRenderDiff,
    } as DiffProps<VALUETYPE, PATHLIST>
  }
  return passedProps as DiffProps<VALUETYPE, PATHLIST>
}

type NumberRenderConfig = {
  step: number
}

const makeRenderValueNumber = ({step}: NumberRenderConfig) => (value: DeepReadonly<number>) => (
  <span>{derivePreciseEditValue(value, step)}</span>
)

const renderValueDefault = (value: DeepReadonly<string>) => (
  <span>{value}</span>
)

type GroupFieldsDiffProps<VALUETYPE extends unknown[]> = {
  value: DeepReadonly<VALUETYPE>
  labels: { [K in keyof VALUETYPE]: string | React.ReactNode }
  renderMethods: { [K in keyof VALUETYPE]: (v: VALUETYPE[K]) => React.ReactNode }
}

// eslint-disable-next-line arrow-parens
const GroupFieldsDiff = <VALUETYPE extends unknown[]>({
  value, labels, renderMethods,
}: GroupFieldsDiffProps<VALUETYPE>): React.ReactElement => {
  const classes = useStyles()
  return (
    <div className={classes.groupDiffHolder}>
      {value.map((val, i) => (
        // The order of renders should never change, so we should be safe to use the index
        // as a key here without side effects. Without doing this, we'd need to pass the
        // corresponding paths all the way down, since this is the only stable identifier
        // we have for the values.
        // eslint-disable-next-line react/no-array-index-key
        <div key={i} className={classes.singleGroupValue}>
          <span className={classes.groupValueLabel}>{labels[i]}</span>
          {renderMethods[i](val)}
        </div>
      ))}
    </div>
  )
}

// REQUIRES: Ensure that the labels and renderMethods stay in the same order,
// without deletions or additions.
const makeRenderValueGroup = <GROUPFIELDS extends GroupFieldsDiffProps<unknown[]>>(
  // eslint-disable-next-line arrow-parens
  labels: GROUPFIELDS['labels'],
  renderMethods: GROUPFIELDS['renderMethods']
) => (value: GROUPFIELDS['value']) => (
  <GroupFieldsDiff
    value={value}
    labels={labels}
    renderMethods={renderMethods}
  />
  )

const renderValueVector3 = (step: number, value: DeepReadonly<[number, number, number]>) => (
  <GroupFieldsDiff
    value={value}
    labels={['X', 'Y', 'Z']}
    renderMethods={[
      makeRenderValueNumber({step}),
      makeRenderValueNumber({step}),
      makeRenderValueNumber({step}),
    ]}
  />
)

const makeRenderValueVector3 = ({step}: {step: number}) => (
  (value: DeepReadonly<[number, number, number]>) => (
    renderValueVector3(step, value)
  )
)

const renderDiffVector3 = (
  step: number,
  before: DeepReadonly<[number, number, number]>,
  after: DeepReadonly<[number, number, number]>
): ReturnType<DiffProps<DeepReadonly<[number, number, number]>, string[][]>['renderDiff']> => {
  // The transform either doesn't exist or is being deleted.
  if (before === undefined || after === undefined) {
    return {
      type: null,
      bannerContent: null,
    }
  }

  // Manual significant figures comparisons
  const beforePrecise: readonly string[] = before.map(v => derivePreciseEditValue(v, step))
  const afterPrecise: readonly string[] = after.map(v => derivePreciseEditValue(v, step))

  const change = beforePrecise.some((v, i) => v !== afterPrecise[i])

  const precisionRenderer = makeRenderValueVector3({step})
  return {
    type: change ? 'subfieldsChanged' : 'unchanged',
    bannerContent: bannerFromRenderValue(
      precisionRenderer,
      before,
      after
    ),
  }
}

const makeRenderDiffVector3 = ({step}: NumberRenderConfig):
DiffProps<DeepReadonly<[number, number, number]>, string[][]>['renderDiff'] => (
  (
    before: DeepReadonly<[number, number, number]>,
    after: DeepReadonly<[number, number, number]>
  ) => renderDiffVector3(
    step,
    before,
    after
  )
)

const BooleanDiffValue: React.FC<{value: DeepReadonly<boolean>}> =
({value}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()

  return (
    <>
      <span className={classes.iconBacking}>
        {value ? <Icon stroke='checkmark' /> : <Icon stroke='blankStroke' />}
      </span>
      <SrOnly>{value ? t('scene_diff.true') : t('scene_diff.false')}</SrOnly>
    </>
  )
}

const renderValueBoolean = (value: DeepReadonly<boolean>) => (
  <BooleanDiffValue value={value} />
)

const renderValueColor = (color: string) => (
  <span>{color}</span>
)

// Corresponding non-interactive version of CompactImagePicker
// Used for diffs etc.
const CompactImage: React.FC<{
  resource: Resource | string
  altText?: string
}> = ({resource, altText}) => {
  const classes = useStyles()
  const currentResource: Resource | null = inferResourceObject(resource)
  const url = useResourceUrl(currentResource)

  const thumbnail = useVideoThumbnail(url)
  const isVideo = ASSET_EXT_TO_KIND[fileExt(url)] === 'video'
  const isImageReady = isVideo ? !!thumbnail : true

  return (
    (url && isImageReady)
      ? (
        <div className={combine('style-reset', classes.insetImageContainer)}>
          <img
            className={classes.image}
            src={thumbnail?.image?.toDataURL() ?? url}
            alt={altText}
            loading='lazy'
          />
        </div>
      )
      : (
        <Icon stroke='image' />
      )
  )
}

const renderValueVisualResource = (value: DeepReadonly<Resource>) => (
  <CompactImage resource={value} />
)

type ResourceRenderConfig = {
  altText?: string
}

const makeRenderValueVisualResource = ({altText}: ResourceRenderConfig) => (
  (value: DeepReadonly<Resource>) => (
    <CompactImage resource={value} altText={altText} />
  )
)

const NonVisualResource: React.FC<{
  value: DeepReadonly<Resource>
}> = ({value}) => {
  const url = useResourceUrl(value)

  if (value.type === 'asset') {
    return (
      <span>
        {value.asset}
      </span>
    )
  } else {
    return (
      <span>
        {url || value.url}
      </span>
    )
  }
}

const renderValueNonVisualResource = (value: DeepReadonly<Resource>) => (
  <NonVisualResource value={value} />
)

// Renders only the diff chip, without any banner
const renderDiffNoBanner: DiffProps<unknown, string[][]>['renderDiff'] = () => ({
  type: null,
  bannerContent: null,
})

export {
  makeRenderValueNumber,
  makeRenderDiffVector3,
  renderValueBoolean,
  renderValueColor,
  makeRenderValueVisualResource,
  renderValueVisualResource,
  renderValueNonVisualResource,
  renderValueDefault,
  makeRenderValueGroup,
  applyDefaultRenderValue,
  applyDefaultRenderDiff,
  renderDiffNoBanner,
  GroupFieldsDiff,
}
