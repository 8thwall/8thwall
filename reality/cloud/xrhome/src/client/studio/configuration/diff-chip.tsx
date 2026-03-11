import React from 'react'

import {
  autoUpdate,
  FloatingPortal,
  hide,
  offset,
  size,
  useFloating,
} from '@floating-ui/react'

import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'
import type {DeepReadonly, Primitive} from 'ts-essentials'

import {bool, combine} from '../../common/styles'
import {darkBlue, mint, deletedRed, changedYellow, brandWhite} from '../../static/styles/settings'
import {DiffType, getDiffType} from '../get-diff-type'
import {SceneDiffContext, useSceneDiffContext} from '../scene-diff-context'
import type {
  DiffProps,
  DiffRenderInfo,
} from './diff-chip-types'

const useStyles = createUseStyles(({
  // TODO(Carson): Disable mouse-events?
  bannerDiv: {
    border: `1px solid ${changedYellow}`,
    backgroundColor: darkBlue,
    color: brandWhite,
    padding: '0.5em',
    borderRadius: '0.5em',
    zIndex: 10,
    display: 'flex',
    gap: '.25em',
    flexDirection: 'row',
    alignItems: 'center',
    fontSize: '12px',
  },
  chipPositioning: {
    position: 'absolute',
    left: '0',
    height: '100%',
  },
  diffChip: {
    width: '5px',
    alignSelf: 'stretch',
    zIndex: 13,
    borderRadius: '3px',
  },
  added: {
    backgroundColor: mint,
  },
  changed: {
    backgroundColor: changedYellow,
  },
  removed: {
    backgroundColor: deletedRed,
  },
  tripleRightHolder: {
    paddingInline: '0.5em',
    color: changedYellow,
  },
}))

const TripleRight: React.FC<{}> = () => {
  // TODO(Carson): Replace this placeholder for future triple chevron right icon
  const classes = useStyles()
  return (
    <span className={classes.tripleRightHolder}> {'>>>'} </span>
  )
}

const diffTypeLoc = {
  changed: 'diff_chip.changed',
  added: 'diff_chip.added',
  removed: 'diff_chip.removed',
  unchanged: 'diff_chip.unchanged',
}

const bannerFromRenderValue = <T, >(
  renderValue: (val: DeepReadonly<T>) => React.ReactNode,
  before: DeepReadonly<T>,
  after: DeepReadonly<T>) => (
    <>
      {renderValue(before)}
      <TripleRight />
      {renderValue(after)}
    </>
  )

const twoSidedBannerFromRenderValue = <T, >(
  renderValueBefore: (val: DeepReadonly<T>) => React.ReactNode,
  renderValueAfter: (val: DeepReadonly<T>) => React.ReactNode,
  before: DeepReadonly<T>,
  after: DeepReadonly<T>) => (
    <>
      {renderValueBefore(before)}
      <TripleRight />
      {renderValueAfter(after)}
    </>
  )

interface BannerlessChipProps {
  // subset of useStyles keys
  coloring: 'added' | 'removed' | 'changed'
  // Causes the chip to be positioned absolutely
  // matching the nearest positioned ancestor's
  // height and starting the chip at the left edge.
  positioned: boolean
}

const BannerlessChip = ({coloring, positioned}: BannerlessChipProps) => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')
  return (
    <div
      className={
          combine(classes.diffChip, bool(positioned, classes.chipPositioning), classes[coloring])
        }
      aria-label={t(diffTypeLoc[coloring])}
      title={t(diffTypeLoc[coloring])}
    />
  )
}

interface BannerChipProps {
  bannerContent: React.ReactNode
  // Causes the chip to be positioned absolutely
  // matching the nearest positioned ancestor's
  // height and starting the chip at the left edge.
  positioned: boolean
}

const BannerChip = ({bannerContent, positioned}: BannerChipProps) => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')

  const {refs, floatingStyles, middlewareData} = useFloating({
    placement: 'left-start',
    whileElementsMounted: autoUpdate,
    middleware: [
      offset(15),
      size({
        apply({rects, elements}) {
          Object.assign(elements.floating.style, {
            maxHeight: `${rects.reference.height}px`,
          })
        },
      }),
      // TODO(Carson): Followup add nicer hiding logic using clipping canvas box
      hide(),
    ],
  })

  return (
    <>
      <div
        ref={bannerContent ? refs.setReference : undefined}
        className={
            combine(classes.diffChip, bool(positioned, classes.chipPositioning), classes.changed)
          }
        aria-label={t(diffTypeLoc.changed)}
        title={t(diffTypeLoc.changed)}
      />
      <FloatingPortal>
        <div
          className={classes.bannerDiv}
          style={{
            ...floatingStyles,
            visibility: middlewareData.hide?.referenceHidden ? 'hidden' : 'visible',
          }}
          ref={refs.setFloating}
        >
          {bannerContent}
        </div>
      </FloatingPortal>
    </>
  )
}
const consolidateDiffs = <PATHLIST extends readonly string[][]>(
  // eslint-disable-next-line arrow-parens
  finalPaths: PATHLIST,
  defaults: { [K in keyof PATHLIST]: Primitive },
  sceneDiff: SceneDiffContext
): DiffType<unknown> => {
  const diffTypes = finalPaths.map((path, i) => getDiffType(
    sceneDiff.changeLog,
    sceneDiff.beforeScene,
    sceneDiff.afterScene,
    path,
    defaults[i]
  ))

  if (diffTypes.length === 0) {
    return null
  }

  if (diffTypes.length === 1) {
    return diffTypes[0]
  }

  const allSame = diffTypes.every(diffType => diffType.type === diffTypes[0].type)

  const type = allSame ? diffTypes[0].type : 'subfieldsChanged'

  const before = diffTypes.map(diffType => diffType.before)

  const after = diffTypes.map(diffType => diffType.after)

  return {
    type,
    before,
    after,
  }
}

function computeTypeAndBanner<VALUETYPE, PATHLIST extends string[][]>(
  renderDiff: DiffProps<VALUETYPE, PATHLIST>['renderDiff'] | undefined,
  renderValue: DiffProps<VALUETYPE, PATHLIST>['renderValue'] | undefined,
  before: DeepReadonly<VALUETYPE>,
  after: DeepReadonly<VALUETYPE>,
  oldType: DiffType<VALUETYPE>['type']
): DiffRenderInfo {
  // Escape hatch if for some reason the default renderValue behavior
  // is not sufficient.
  if (renderDiff) {
    const render = renderDiff(before, after)
    if (render.type === null) {
      return {
        type: oldType,
        bannerContent: null,
      }
    }
    return render
  } else {
    return {
      type: oldType,
      bannerContent: bannerFromRenderValue(renderValue, before, after),
    }
  }
}

type IDiffChip<VALUETYPE> = {
  finalPaths: string[][]
  renderDiff?: DiffProps<VALUETYPE, string[][]>['renderDiff']
  renderValue?: DiffProps<VALUETYPE, string[][]>['renderValue']
  defaults?: Primitive[]
}

// eslint-disable-next-line arrow-parens
const DiffChip = <VALUETYPE, >({
  finalPaths,
  renderDiff,
  renderValue,
  defaults,
}: IDiffChip<VALUETYPE>) => {
  const sceneDiff = useSceneDiffContext()

  if (finalPaths.length === 0 || !sceneDiff) {
    return null
  }

  // Empty list also invalid, so coalescing here is OK
  const trueDefaults = defaults ??
    finalPaths.map(() => (undefined))

  const diffType = consolidateDiffs(
    finalPaths,
    trueDefaults,
    sceneDiff
  )

  const {type, bannerContent} = computeTypeAndBanner(
    renderDiff,
    renderValue,
    diffType.before,
    diffType.after,
    diffType.type
  )

  if (type === 'unchanged') {
    return null
  }
  if (type === 'removed' || type === 'added') {
    return (
      <BannerlessChip
        coloring={type}
        positioned
      />
    )
  }

  if (!bannerContent) {
    return (
      <BannerlessChip
        coloring='changed'
        positioned
      />
    )
  }
  return (
    <BannerChip
      bannerContent={bannerContent}
      positioned
    />
  )
}

const ObjectChip: React.FC<{
  objectId: string
}> = ({objectId}) => {
  const sceneDiff = useSceneDiffContext()

  if (!sceneDiff) {
    return null
  }

  if (sceneDiff.changeLog.objectAdditions[objectId]) {
    return (
      <BannerlessChip
        coloring='added'
        positioned
      />
    )
  } else if (sceneDiff.changeLog.objectDeletions[objectId]) {
    return (
      <BannerlessChip
        coloring='removed'
        positioned
      />
    )
  } else if (sceneDiff.changeLog.objectUpdates[objectId]) {
    return (
      <BannerlessChip
        coloring='changed'
        positioned
      />
    )
  }
  return null
}

const SpaceChip: React.FC<{
  spaceId: string
}> = ({spaceId}) => {
  const sceneDiff = useSceneDiffContext()

  if (!sceneDiff) {
    return null
  }

  const spaceChange = sceneDiff.changeLog.spaceChanges[spaceId]
  if (!spaceChange) {
    return null
  }

  if (spaceChange.type === 'added') {
    return (
      <BannerlessChip
        coloring='added'
        positioned
      />
    )
  } else if (spaceChange.type === 'removed') {
    return (
      <BannerlessChip
        coloring='removed'
        positioned
      />
    )
  } else if (spaceChange.type === 'changed') {
    return (
      <BannerlessChip
        coloring='changed'
        positioned
      />
    )
  }
  return null
}

export {
  bannerFromRenderValue,
  twoSidedBannerFromRenderValue,
  DiffChip,
  ObjectChip,
  SpaceChip,
}
