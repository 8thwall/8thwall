import React from 'react'

import type {DropdownOption, DropdownSection} from '../../ui/components/core-dropdown'
import {Icon, IconStroke} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'
import type {AspectOptionId} from '../editor-reducer'

const APP_PREVIEW_METADATA_SRC = 'https://cdn.8thwall.com/web/app-preview/sequence_metadata.json'
// NOTE(Dale): Dev version used while testing
// 'https://cdn-dev.8thwall.com/pr/756/5uofxfxg/web/app-preview/sequence_metadata.json'

const MIN_PREVIEW_WIDTH = 250
const MIN_PREVIEW_HEIGHT = 500
const DEFAULT_PREVIEW_WIDTH = '25rem'

const ACTION_BUTTON_TEXT_MIN_WIDTH = 400

const useStyles = createThemedStyles(theme => ({
  dropdownSectionLabel: {
    'position': 'relative',
    'overflow': 'hidden',
    'padding': '0.5rem',
    '&  > :first-child': {
      marginRight: '0.5rem',
    },
  },
  dropdownSectionInfo: {
    position: 'relative',
    overflow: 'hidden',
    padding: '0.5rem 1rem',
    fontStyle: 'italic',
    color: theme.fgMain,
  },
  dropdownVisibleContent: {
    'lineHeight': 'initial',
    'display': 'flex',
    'minWidth': '0',
    '&  > :first-child': {
      marginRight: '0.5rem',
    },
  },
  visibleContentText: {
    'flex': '1',
    'overflow': 'hidden',
    'text-overflow': 'ellipsis',
    'whiteSpace': 'nowrap',
  },
  dropdownOption: {
    display: 'flex',
    alignItems: 'center',
    flex: '1 0 0',
  },
  optionTextContent: {
    overflowWrap: 'anywhere',
    wordBreak: 'normal',
  },
  optionRightContent: {
    marginLeft: 'auto',
    marginRight: '2.5rem',
    paddingLeft: '1rem',
    color: theme.fgMuted,
  },
  previewActions: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    width: '100%',
    padding: '0.5em',
    background: theme.tabBarBg,
    gap: '0.5rem',
    zIndex: 1,
    pointerEvents: 'auto',
    height: 'var(--simulator-preview-actions-height, auto)',
  },
  appPreviewPlaybar: {
    extend: 'previewActions',
    padding: '0.5em 1em',
    gap: '1.5rem',
  },
  selectionContainer: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    flex: '1 0 0',
    minWidth: '0',
  },
  actionButton: {
    'color': theme.fgMuted,
    'userSelect': 'none',
    '&:hover:not([aria-disabled=true])': {
      color: theme.fgMain,
    },
  },
  withFeedback: {
    'transition': 'transform 0.1s ease-in-out',
    '&:has(button:active)': {
      transform: 'scale(0.8)',
    },
  },
  optionIcon: {
    color: theme.fgMuted,
    position: 'relative',
    left: '-0.5rem',
  },
  dropdown: {
    flex: '0 1 auto',
    minWidth: '0',
  },
}))

type ProgressMessageEvent = {
  data: {
    action: string
    data: {
      currentProgress: number
      simulatorId: string
    }
  }
}

interface IDropdownOptionContent {
  textContent: string
  rightContent?: string
  stroke?: IconStroke
}

type SectionData = {
  key: string
  textContent?: string
  stroke?: IconStroke
  info?: string
  loadMoreNode?: React.ReactNode
}

type AspectOptionData = {
  aspectOptionId?: AspectOptionId
  width?: number
  height?: number
  responsive?: boolean
  name: string
  section: string
}

type SequenceOptionData = {
  name: string
  section: string
  rightContent: string
}

type CoordinateOption = DropdownOption & {
  name: string
}

interface Recording {
  cameraUrl: string
  sequenceUrl?: string
  orientation?: 'portrait' | 'landscape'
}

type VariationConfig = Array<Recording> | Recording

interface Sequence {
  name: string
  section: string
  variations: Record<string, VariationConfig>
  tag: string
}

interface SequenceMetadata {
  defaultUserAgent: string
  sequences: Sequence[]
}

type SimulatorConfig = {
  // Version number is assigned in increasing order to indicate the most recent config.
  version: number
  simulatorId: String
  cameraUrl?: string
  sequenceUrl?: string
  isPaused?: boolean
  start?: number
  end?: number
  userAgent?: string
  poiId?: string
  gpsLatitude?: number
  gpsLongitude?: number
  locationName?: string
  manualVpsEvents?: boolean
  imageTargetName?: string
  mockLat?: number
  mockLng?: number
  mockCoordinateValue?: string
}

// @param stroke Draw dropdown icon with stroke
const DropdownOptionContent: React.FC<IDropdownOptionContent> = ({
  textContent, rightContent, stroke,
}) => {
  const classes = useStyles()
  return (
    <div className={classes.dropdownOption}>
      {stroke &&
        <div className={classes.optionIcon}>
          <Icon stroke={stroke} inline />
        </div>
      }
      <span className={classes.optionTextContent}>{textContent}</span>
      <div className={classes.optionRightContent}>
        {rightContent}
      </div>
    </div>
  )
}

interface ISectionContent {
  textContent: string
  stroke: IconStroke
  info?: string
}

const SectionContent: React.FC<ISectionContent> = ({textContent, stroke, info}) => {
  const classes = useStyles()
  return (
    <>
      <div className={classes.dropdownSectionLabel}>
        <Icon stroke={stroke} inline />
        {textContent}
      </div>
      {info && <div className={classes.dropdownSectionInfo}>{info}</div>}
    </>
  )
}

const makeSection = ({
  key, textContent, stroke, info, loadMoreNode,
}: SectionData): DropdownSection => ({
  key,
  label: (textContent && stroke)
    ? (
      <SectionContent
        textContent={textContent}
        stroke={stroke}
        info={info}
      />
    )
    : null,
  loadMoreNode,
})

const makeSequenceOption = (
  {name, section, rightContent}: SequenceOptionData
): DropdownOption => ({
  value: name,
  content: (
    <DropdownOptionContent
      textContent={name}
      rightContent={rightContent}
    />
  ),
  section,
})

const getIconStrokeForOption = (
  option: DropdownOption, sectionData: SectionData[]
): IconStroke => (
  sectionData.find(({key}) => key === option.section).stroke
)

interface IUseVisibleContent {
  option: DropdownOption
  sectionData: SectionData[]
  textContent: string
}

const VisibleContent: React.FC<IUseVisibleContent> = (
  {option, sectionData, textContent}
) => {
  const iconStroke = getIconStrokeForOption(option, sectionData)
  const classes = useStyles()
  return (
    <div className={classes.dropdownVisibleContent}>
      <Icon stroke={iconStroke} color='gray4' />
      <span className={classes.visibleContentText}>{textContent}</span>
    </div>
  )
}

/* eslint-disable local-rules/hardcoded-copy */

const RESPONSIVE_OPTION_DATA: AspectOptionData = {
  aspectOptionId: 'responsive',
  responsive: true,
  name: 'Responsive',
  section: 'RESPONSIVE',
}
const aspectOptionData: AspectOptionData[] = [
  RESPONSIVE_OPTION_DATA,
  {width: 393, height: 659, name: 'iPhone 15 Pro', section: 'MOBILE'},
  {width: 430, height: 739, name: 'iPhone 15 Pro Max', section: 'MOBILE'},
  {width: 375, height: 629, name: 'iPhone 13 Mini', section: 'MOBILE'},
  {width: 375, height: 547, name: 'iPhone SE (2022)', section: 'MOBILE'},
  {width: 412, height: 784, name: 'Google Pixel 8', section: 'MOBILE'},
  {width: 384, height: 693, name: 'Samsung S23 Ultra', section: 'MOBILE'},
  {width: 345, height: 746, name: 'Samsung Z Fold5 (cover)', section: 'MOBILE'},
  {width: 691, height: 655, name: 'Samsung Z Fold5 (main)', section: 'MOBILE'},
  {width: 834, height: 1120, name: 'iPad Pro 11"', section: 'TABLET'},
  {width: 1024, height: 1292, name: 'iPad Pro 12.9"', section: 'TABLET'},
  {width: 820, height: 1106, name: 'iPad (10th Gen)', section: 'TABLET'},
  {width: 744, height: 1059, name: 'iPad mini (6th Gen)', section: 'TABLET'},
  {width: 980, height: 1351, name: 'Samsung Tab S9', section: 'TABLET'},
  {width: 924, height: 1312, name: 'Samsung Tab S9 Ultra', section: 'TABLET'},
  {width: 1512, height: 892, name: 'MacBook Pro 14"', section: 'DESKTOP'},
  {width: 1728, height: 962, name: 'MacBook Pro 16', section: ' DESKTOP'},
  {width: 1920, height: 1080, name: 'Full HD', section: 'DESKTOP'},
  {width: 3840, height: 2160, name: '4k TV"', section: 'DESKTOP'},
]

/* eslint-enable local-rules/hardcoded-copy */

const makeAspectRatioValue = (
  width: number,
  height: number,
  aspectOptionId?: AspectOptionId
): string => {
  if (!aspectOptionId) {
    return `${width}x${height}`
  }

  switch (aspectOptionId) {
    case 'responsive':
      return aspectOptionId
    default:
      return 'unknown'
  }
}

const getCurrentRecording = (
  variationConfig: VariationConfig, wantsLandscapeRecording: boolean
): Recording => {
  if (Array.isArray(variationConfig)) {
    const desiredOrientation = wantsLandscapeRecording ? 'landscape' : 'portrait'
    const landscapeRecording =
      variationConfig?.find(item => item.orientation === desiredOrientation)
    return landscapeRecording || variationConfig[0]
  }
  return variationConfig
}

export {
  ACTION_BUTTON_TEXT_MIN_WIDTH,
  APP_PREVIEW_METADATA_SRC,
  DEFAULT_PREVIEW_WIDTH,
  MIN_PREVIEW_HEIGHT,
  MIN_PREVIEW_WIDTH,
  RESPONSIVE_OPTION_DATA,
  DropdownOptionContent,
  VisibleContent,
  aspectOptionData,
  getCurrentRecording,
  makeAspectRatioValue,
  makeSection,
  makeSequenceOption,
  useStyles as useAppPreviewStyles,
}

export type {
  AspectOptionData,
  CoordinateOption,
  ProgressMessageEvent,
  SectionData,
  Sequence,
  SequenceMetadata,
  SequenceOptionData,
  SimulatorConfig,
  VariationConfig,
  Recording,
}
