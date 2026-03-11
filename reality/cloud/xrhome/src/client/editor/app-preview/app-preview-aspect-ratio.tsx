import React from 'react'
import {useTranslation} from 'react-i18next'
import type {MeasuredComponentProps} from 'react-measure'

import {
  MIN_PREVIEW_WIDTH,
  aspectOptionData,
  AspectOptionData,
  DropdownOptionContent,
  makeAspectRatioValue,
  makeSection,
  SectionData,
  useAppPreviewStyles,
  VisibleContent,
} from './app-preview-utils'
import {DropdownOption, StandardDropdownField} from '../../ui/components/standard-dropdown-field'
import {Popup} from '../../ui/components/popup'
import {useSimulatorDimension} from './use-simulator-dimension'
import {useSimulator} from './use-simulator-state'
import {combine} from '../../common/styles'
import type {DropdownSection} from '../../ui/components/core-dropdown'
import {IconButton} from '../../ui/components/icon-button'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  aspectRatio: {
    color: theme.fgMuted,
  },
  orientationButton: {
    padding: '.25em .25em .25em .8em',
  },
}))

interface IAppPreviewAspectRatio {
  responsiveHeight?: number
  responsiveWidth?: number
  previewPaneMeasure?: MeasuredComponentProps
}

const AppPreviewAspectRatio: React.FC<IAppPreviewAspectRatio> = (
  {responsiveHeight, responsiveWidth, previewPaneMeasure}
) => {
  const classes = useStyles()
  const appPreviewStyles = useAppPreviewStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  const {simulatorState, updateSimulatorState} = useSimulator()
  const {isLandscape, responsive} = simulatorState

  const {
    iframeHeight, iframeWidth, disableOrientationToggle, optionData, aspectRatioValue,
  } = useSimulatorDimension()

  const makeOption = ({
    width, height, name, section, aspectOptionId,
  }: AspectOptionData): DropdownOption => {
    const getRightContent = (): string => {
      if (!width || !height) {
        return null
      }
      return (isLandscape && section !== 'DESKTOP') ? `${height}x${width}` : `${width}x${height}`
    }

    const isExplicitResponsiveMode = aspectOptionId === 'responsive'

    return ({
      value: makeAspectRatioValue(width, height, aspectOptionId),
      content: (
        <DropdownOptionContent
          textContent={name}
          rightContent={getRightContent()}
          stroke={isExplicitResponsiveMode ? 'expand' : null}
        />),
      section,
    })
  }

  const aspectOptions: DropdownOption[] = aspectOptionData.map(makeOption)

  const aspectSectionData: SectionData[] = [
    {
      key: 'RESPONSIVE',
      stroke: 'expand',
    },
    {
      key: 'MOBILE',
      textContent: t('editor_page.inline_app_preview.iframe.device_dropdown.section_name.mobile'),
      stroke: 'phone',
    },
    {
      key: 'TABLET',
      textContent: t('editor_page.inline_app_preview.iframe.device_dropdown.section_name.tablet'),
      stroke: 'tablet',
    },
    {
      key: 'DESKTOP',
      textContent: t('editor_page.inline_app_preview.iframe.device_dropdown.section_name.desktop'),
      stroke: 'laptop',
    },
  ]

  const aspectSections: DropdownSection[] = aspectSectionData.map(makeSection)

  const getVisibleAspectRatioContent = (option: DropdownOption): React.ReactNode => (
    <VisibleContent
      option={option}
      sectionData={aspectSectionData}
      textContent={optionData.aspectOptionId === 'responsive'
        ? `${responsiveHeight}x${responsiveWidth}`
        : optionData.name}
    />
  )

  const renderAspectRatioPopup = (containerWidth: number) => {
    if (containerWidth < MIN_PREVIEW_WIDTH * 1.1) {
      return null
    }

    return (
      <Popup
        content={t('editor_page.inline_app_preview.iframe.aspect_ratio.popup')}
        position='bottom'
        alignment='center'
        size='tiny'
        delay={750}
      >
        <div className={classes.aspectRatio}>
          {responsive
            ? `${responsiveHeight}x${responsiveWidth}`
            : `${iframeWidth}x${iframeHeight}`}
        </div>
      </Popup>
    )
  }

  const handleOrientationChange = () => {
    updateSimulatorState({isLandscape: !isLandscape})
  }

  const handleAspectRatioChange = (value: string) => {
    if (value === 'responsive') {
      updateSimulatorState({responsive: true, aspectOptionId: 'responsive'})
      return
    }
    const aspectParts = value.split('x')
    updateSimulatorState({
      width: parseFloat(aspectParts[0]),
      height: parseFloat(aspectParts[1]),
      responsive: false,
      aspectOptionId: undefined,
    })
  }

  const getAspectRatioSection = (): DropdownSection => ({
    key: 'ASPECT_RATIO',
    label:
        (
          <div className={appPreviewStyles.selectionContainer}>
            <div
              className={combine('style-reset', classes.orientationButton,
                appPreviewStyles.actionButton)}
              aria-disabled={disableOrientationToggle}
            >
              <IconButton
                stroke={isLandscape ? 'rotatePortrait' : 'rotateLandscape'}
                onClick={handleOrientationChange}
                text={t('editor_page.inline_app_preview.aspect_ratio.rotate_button')}
                disabled={disableOrientationToggle}
              />
            </div>
            {renderAspectRatioPopup(previewPaneMeasure.contentRect.bounds.width)}
          </div>
        ),
  })

  return (
    <div className={appPreviewStyles.dropdown}>
      <Popup
        content={t('editor_page.inline_app_preview.iframe.device_dropdown.popup')}
        position='bottom'
        alignment='center'
        size='tiny'
        delay={750}
      >
        <StandardDropdownField
          label=''
          id='aspectRatioDropdown'
          height='tiny'
          options={aspectOptions}
          value={aspectRatioValue}
          onChange={handleAspectRatioChange}
          sections={[getAspectRatioSection(), ...aspectSections]}
          width='maxContent'
          formatVisibleContent={getVisibleAspectRatioContent}
          maxHeight={responsiveHeight * 0.98}
          maxWidth={responsiveWidth * 0.9}
          shouldReposition={false}
        />
      </Popup>
    </div>
  )
}

export {
  AppPreviewAspectRatio,
}
