import React, {useEffect} from 'react'
import {
  DEFAULT_LAT,
  DEFAULT_LNG,
  DEFAULT_RADIUS,
  LAT_LNG_SLIDER_FACTOR,
  MAX_LAT, MAX_LNG,
  MIN_LAT, MIN_LNG,
} from '@ecs/shared/map-constants'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import {Popup} from '../../ui/components/popup'
import {useSimulator} from './use-simulator-state'
import {
  CoordinateOption,
  DropdownOptionContent,
  makeSection,
  SectionData,
  VisibleContent,
} from './app-preview-utils'
import {DropdownOption, StandardDropdownField} from '../../ui/components/standard-dropdown-field'
import {Icon} from '../../ui/components/icon'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import type {DropdownSection} from '../../ui/components/core-dropdown'

const useStyles = createThemedStyles(theme => ({
  coordinateInput: {
    display: 'flex',
    gap: '0.2em',
    padding: '.1em',
  },
  coordinateInputIcon: {
    color: theme.fgMuted,
    position: 'relative',
    padding: '.2em .0em',
    left: '-0.5rem',
  },
  coordinateInputFields: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em 0em',
  },
}))

interface IAppPreviewMockLocation {
  responsiveHeight?: number
  responsiveWidth?: number
  iframeRef?: React.RefObject<HTMLIFrameElement>
}

const AppPreviewMockLocation: React.FC<IAppPreviewMockLocation> = (
  {responsiveHeight, responsiveWidth, iframeRef}
) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  const [canLocate, setCanLocate] = React.useState<boolean>(true)

  const {simulatorState, updateSimulatorState} = useSimulator()
  const {
    mockLat, mockLng,
  } = simulatorState

  const mockCoordinateValue = simulatorState.mockCoordinateValue ?? 'none'

  const updateDeviceLocation = (coords?: {latitude: number, longitude: number}) => {
    setCanLocate(!!coords)
    updateSimulatorState({
      mockCoordinateValue: 'device',
      ...(coords ? {mockLat: coords?.latitude, mockLng: coords?.longitude} : {}),
    })
  }

  const handleCoordinateChange = (value) => {
    updateSimulatorState({
      mockCoordinateValue: value,
    })
  }

  const handleLatChange = (value) => {
    updateSimulatorState({
      mockLat: value,
      mockCoordinateValue: 'input',
    })
  }

  const handleLngChange = (value) => {
    updateSimulatorState({
      mockLng: value,
      mockCoordinateValue: 'input',
    })
  }

  const inputOption: CoordinateOption = {
    value: 'input',
    name: t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.input'),
    section: 'INPUT',
    shouldNotCollapse: true,
    content: (
      <div className={classes.coordinateInput}>
        <div className={classes.coordinateInputIcon}>
          <Icon stroke='edit' inline />
        </div>
        <div className={classes.coordinateInputFields}>
          <SliderInputAxisField
            id='lat'
            label={t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.lat')}
            step={LAT_LNG_SLIDER_FACTOR * DEFAULT_RADIUS}
            value={mockLat ?? DEFAULT_LAT}
            onChange={handleLatChange}
            fixed={6}
            fullWidth
            min={MIN_LAT}
            max={MAX_LAT}
          />
          <SliderInputAxisField
            id='lng'
            label={t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.lng')}
            step={LAT_LNG_SLIDER_FACTOR * DEFAULT_RADIUS}
            value={mockLng ?? DEFAULT_LNG}
            onChange={handleLngChange}
            fixed={6}
            fullWidth
            min={MIN_LNG}
            max={MAX_LNG}
          />
        </div>
      </div>
    ),
  }

  const noneOption: CoordinateOption = {
    value: 'none',
    name: t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.none'),
    section: 'NONE',
    content: <DropdownOptionContent
      textContent={t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.none')}
      stroke='banCircle'
    />,
  }

  const deviceLocationOption = {
    value: 'device',
    // eslint-disable-next-line max-len
    name: t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.current_location'),
    section: 'DEVICE',
    content: <DropdownOptionContent
      // eslint-disable-next-line max-len
      textContent={t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.current_location')}
      stroke='gpsArrow'
    />,
  }

  const coordinateOptions: CoordinateOption[] = [
    inputOption,
    noneOption,
    deviceLocationOption,
  ]

  const coordinateSectionData: SectionData[] = [
    {
      key: 'DEVICE',
      stroke: 'gpsArrow',
    },
    {
      key: 'INPUT',
      stroke: 'edit',
    },
    {
      key: 'LOCATIONS',
      // eslint-disable-next-line max-len
      textContent: t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.locations'),
      stroke: 'vpsLocation',
    },
    {
      key: 'MAP-POINTS',
      // eslint-disable-next-line max-len
      textContent: t('editor_page.inline_app_preview.iframe.coordinate_dropdown.section_name.map_points'),
      stroke: 'mapPointLocation',
    },
    {
      key: 'NONE',
      stroke: 'banCircle',
    },
  ]

  const coordinateSections: DropdownSection[] = coordinateSectionData.map(makeSection)

  const getVisibleCoordinateContent = (option: DropdownOption): React.ReactNode => (
    <VisibleContent
      option={option}
      sectionData={coordinateSectionData}
      textContent={coordinateOptions.find(e => e.value === option.value).name}
    />
  )

  useEffect(() => {
    const emitGpsData = () => {
      const detail = {
        latitude: mockLat ?? DEFAULT_LAT,
        longitude: mockLng ?? DEFAULT_LNG,
        accuracy: 45,
      }
      if (mockCoordinateValue === 'none' || (mockCoordinateValue === 'device' && !canLocate)) {
        iframeRef.current?.contentWindow.postMessage({
          action: 'SIMULATOR_MOCKLOCATIONLOST8W',
        }, '*')
      } else {
        iframeRef.current?.contentWindow.postMessage({
          action: 'SIMULATOR_GPS8W',
          gps8w: {...detail},
        }, '*')
      }
    }
    emitGpsData()
    const id = window.setInterval(emitGpsData, 100)
    return () => {
      window.clearInterval(id)
    }
  }, [mockLat, mockLng, mockCoordinateValue, canLocate])

  useEffect(() => {
    if (mockCoordinateValue === 'device') {
      const watchId = navigator.geolocation.watchPosition(
        p => updateDeviceLocation(p.coords),
        () => updateDeviceLocation(),
        {enableHighAccuracy: true}
      )
      return () => {
        navigator.geolocation.clearWatch(watchId)
      }
    }
    return () => {}
  }, [mockCoordinateValue])

  return (
    <Popup
      content={t('editor_page.inline_app_preview.iframe.coordinate_dropdown.popup')}
      position='bottom'
      alignment='center'
      size='tiny'
      delay={750}
    >
      <StandardDropdownField
        label=''
        id='coordinateDropdown'
        height='tiny'
        options={coordinateOptions}
        value={mockCoordinateValue}
        onChange={handleCoordinateChange}
        sections={coordinateSections}
        width='maxContent'
        formatVisibleContent={getVisibleCoordinateContent}
        maxHeight={responsiveHeight * 0.98}
        maxWidth={responsiveWidth * 0.65}
        shouldReposition={false}
      />
    </Popup>
  )
}
export {
  AppPreviewMockLocation,
}
