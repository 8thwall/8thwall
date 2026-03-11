import React, {useMemo} from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {Light} from '@ecs/shared/scene-graph'
import {
  LIGHT_DEFAULTS, DEFAULT_COLOR,
  SHADOW_LOW_QUALITY_SIZE, SHADOW_MEDIUM_QUALITY_SIZE, SHADOW_HIGH_QUALITY_SIZE,
} from '@ecs/shared/light-constants'
import {type TFunction, useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'
import {degreesToRadians, radiansToDegrees} from '@ecs/shared/angle-conversion'

import {FloatingTrayCollapsible} from '../../ui/components/floating-tray-collapsible'
import {
  RowNumberField, RowSelectField, RowBooleanField, RowColorField, useStyles as useRowStyles,
  RowGroupFields, ColorPicker,
} from './row-fields'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {useStudioStateContext} from '../studio-state-context'
import {copyDirectProperties} from './copy-component'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {LIGHT_COMPONENT} from '../hooks/available-components'
import {LIGHT_COMPONENT as GRAPH_LIGHT_COMPONENT} from './direct-property-components'
import {CompactImagePicker} from '../ui/compact-image-picker'
import {RowContent} from './row-content'

const LIGHT_TYPES = [
  {value: 'directional', content: 'light_configurator.light_type.option.directional'},
  {value: 'ambient', content: 'light_configurator.light_type.option.ambient'},
  {value: 'point', content: 'light_configurator.light_type.option.point'},
  {value: 'spot', content: 'light_configurator.light_type.option.spot'},
  {value: 'area', content: 'light_configurator.light_type.option.area'},
] as const

const SHADOW_QUALITIES = [
  {value: 'low', content: 'light_configurator.shadow_quality.option.low'},
  {value: 'medium', content: 'light_configurator.shadow_quality.option.medium'},
  {value: 'high', content: 'light_configurator.shadow_quality.option.high'},
]

type LightConfig = DeepReadonly<Light> | undefined

const makeDefaultLight = (type: Light['type'], t: TFunction<'cloud-studio-pages'[]>): Light => {
  if (!LIGHT_TYPES.map(({value}) => value).includes(type)) {
    throw new Error(t('light_configurator.error.invalid_light_type'))
  }
  return {
    type,
  }
}

const useStyles = createUseStyles({
  userSelectNone: {
    userSelect: 'none',
  },
})

interface ILightConfigurator {
  light: LightConfig
  onChange: (updater: (currentLight: LightConfig) => LightConfig) => void
  resetToPrefab?: (componentIds: string[], nonDirect?: any) => void
}

const LightConfigurator: React.FC<ILightConfigurator> = ({light, onChange, resetToPrefab}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const rowClasses = useRowStyles()
  const stateCtx = useStudioStateContext()

  const shadowQuality = useMemo(() => {
    switch (light?.shadowMapSize?.[0]) {
      case SHADOW_LOW_QUALITY_SIZE:
        return 'low'
      case SHADOW_MEDIUM_QUALITY_SIZE:
        return 'medium'
      case SHADOW_HIGH_QUALITY_SIZE:
        return 'high'
      default:
        return 'medium'
    }
  }, [light])

  if (!light) {
    return (
      <FloatingTrayCollapsible
        title={t('light_configurator.title')}
      >
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeDefaultLight('directional', t))
          }}
        >{t('light_configurator.button.add_light')}
        </FloatingPanelButton>
      </FloatingTrayCollapsible>
    )
  }

  return (
    <ComponentConfiguratorTray
      title={t('light_configurator.title')}
      description={t('light_configurator.description')}
      sectionId={LIGHT_COMPONENT}
      onRemove={() => onChange(() => undefined)}
      onCopy={() => copyDirectProperties(stateCtx, {light})}
      onResetToPrefab={resetToPrefab ? () => resetToPrefab([GRAPH_LIGHT_COMPONENT]) : undefined}
      componentData={[GRAPH_LIGHT_COMPONENT]}
    >
      <RowSelectField
        id='lightType'
        label={t('light_configurator.light_type.label')}
        value={light.type}
        options={LIGHT_TYPES.map(type => ({value: type.value, content: t(type.content)}))}
        onChange={(value) => {
          onChange(currentLight => ({
            ...currentLight,
            type: value,
          }))
        }}
      />
      {light.type === 'directional' && (
        <div>
          {!(light.followCamera ?? LIGHT_DEFAULTS.followCamera) && (
            <RowContent>
              <p className={classes.userSelectNone}>
                {t('light_configurator.target_position.label')}
              </p>
              <RowNumberField
                id='target-x'
                label='X '
                step={0.1}
                value={light.target?.[0] ?? LIGHT_DEFAULTS.targetX}
                onChange={(newValue) => {
                  onChange(currentLight => ({
                    ...currentLight,
                    target: [newValue,
                      currentLight.target?.[1] ?? LIGHT_DEFAULTS.targetY,
                      currentLight.target?.[2] ?? LIGHT_DEFAULTS.targetZ],
                  }))
                }}
              />
              <RowNumberField
                id='target-y'
                label='Y '
                step={0.1}
                value={light.target?.[1] ?? LIGHT_DEFAULTS.targetY}
                onChange={(newValue) => {
                  onChange(currentLight => ({
                    ...currentLight,
                    target: [currentLight.target?.[0] ?? LIGHT_DEFAULTS.targetX,
                      newValue,
                      currentLight.target?.[2] ?? LIGHT_DEFAULTS.targetZ],
                  }))
                }}
              />
              <RowNumberField
                id='target-z'
                label='Z '
                step={0.1}
                value={light.target?.[2] ?? LIGHT_DEFAULTS.targetZ}
                onChange={(newValue) => {
                  onChange(currentLight => ({
                    ...currentLight,
                    target: [
                      currentLight.target?.[0] ?? LIGHT_DEFAULTS.targetX,
                      currentLight.target?.[1] ?? LIGHT_DEFAULTS.targetY,
                      newValue],
                  }))
                }}
              />
            </RowContent>
          )}
          <RowBooleanField
            label={t('light_configurator.follow_camera.label')}
            id='follow-camera'
            checked={light.followCamera ?? LIGHT_DEFAULTS.followCamera}
            onChange={(e) => {
              const {checked} = e.target
              onChange(currentLight => ({
                ...currentLight,
                followCamera: checked,
              }))
            }}
          />
        </div>)}
      {light.type === 'spot'
        ? (
          <RowGroupFields label={t('light_configurator.color.label')}>
            <ColorPicker
              id='light-color'
              ariaLabel={t('light_configurator.color.label')}
              value={light.color ?? DEFAULT_COLOR}
              onChange={(color) => {
                onChange(currentLight => ({
                  ...currentLight,
                  color,
                }))
              }}
            />
            <CompactImagePicker
              assetKind='image'
              ariaLabel={t('light_configurator.color_map.label')}
              resource={light.colorMap}
              onChange={(src) => {
                onChange(old => ({...old, colorMap: src}))
              }}
            />
          </RowGroupFields>
        )
        : (
          <RowColorField
            id='light-color'
            label={t('light_configurator.color.label')}
            value={light.color ?? DEFAULT_COLOR}
            onChange={(color) => {
              onChange(currentLight => ({
                ...currentLight,
                color,
              }))
            }}
          />
        )}
      <RowNumberField
        id='intensity'
        label={t('light_configurator.intensity.label')}
        min={0}
        step={0.1}
        value={light.intensity ?? LIGHT_DEFAULTS.intensity}
        onChange={(newValue) => {
          onChange(currentLight => ({
            ...currentLight,
            intensity: newValue,
          }))
        }}
      />
      {(light.type === 'directional' || light.type === 'point' || light.type === 'spot') &&
        <>
          <RowBooleanField
            label={t('light_configurator.cast_shadow.label')}
            id='castShadow'
            checked={light?.castShadow ?? LIGHT_DEFAULTS.castShadow}
            onChange={(e) => {
              const {checked} = e.target
              onChange(currentLight => ({
                ...currentLight,
                castShadow: checked,
              }))
            }}
          />
          {(light.castShadow ?? LIGHT_DEFAULTS.castShadow) && (
            <div className={rowClasses.indent}>
              <RowSelectField
                id='shadowQuality'
                label={t('light_configurator.shadow_quality.label')}
                value={shadowQuality}
                options={SHADOW_QUALITIES.map(type => ({
                  value: type.value,
                  content: t(type.content),
                }))}
                onChange={(quality) => {
                  switch (quality) {
                    case 'low':
                      onChange(currentLight => ({
                        ...currentLight,
                        shadowMapSize: [SHADOW_LOW_QUALITY_SIZE, SHADOW_LOW_QUALITY_SIZE],
                      }))
                      break
                    case 'medium':
                      onChange(currentLight => ({
                        ...currentLight,
                        shadowMapSize: [SHADOW_MEDIUM_QUALITY_SIZE, SHADOW_MEDIUM_QUALITY_SIZE],
                      }))
                      break
                    case 'high':
                      onChange(currentLight => ({
                        ...currentLight,
                        shadowMapSize: [SHADOW_HIGH_QUALITY_SIZE, SHADOW_HIGH_QUALITY_SIZE],
                      }))
                      break
                    default:
                      throw new Error('Unexpected shadow quality')
                  }
                }}
              />
              {(light.type !== 'directional' || !light.followCamera) && (
                <RowNumberField
                  label={t('light_configurator.shadow_distance.label')}
                  id='shadowDistance'
                  step={1}
                  value={light.shadowCamera?.[5] ?? LIGHT_DEFAULTS.shadowCameraFar}
                  onChange={(shadowDistance) => {
                    onChange(currentLight => ({
                      ...currentLight,
                      shadowCamera: [
                        currentLight.shadowCamera?.[0] ?? LIGHT_DEFAULTS.shadowCameraLeft,
                        currentLight.shadowCamera?.[1] ?? LIGHT_DEFAULTS.shadowCameraRight,
                        currentLight.shadowCamera?.[2] ?? LIGHT_DEFAULTS.shadowCameraTop,
                        currentLight.shadowCamera?.[3] ?? LIGHT_DEFAULTS.shadowCameraBottom,
                        currentLight.shadowCamera?.[4] ?? LIGHT_DEFAULTS.shadowCameraNear,
                        shadowDistance,
                      ],
                    }))
                  }}
                />
              )}
              {light.type === 'directional' && !light.followCamera && (
                <RowNumberField
                  label={t('light_configurator.shadow_area.label')}
                  id='shadowArea'
                  step={1}
                  min={1}
                  value={Math.abs(light.shadowCamera?.[0] ?? LIGHT_DEFAULTS.shadowCameraLeft)}
                  onChange={(shadowArea) => {
                    // the shadow camera viewing frustum should be symmetrical
                    onChange(currentLight => ({
                      ...currentLight,
                      shadowCamera: [
                        -shadowArea,
                        shadowArea,
                        shadowArea,
                        -shadowArea,
                        currentLight.shadowCamera?.[4] ?? LIGHT_DEFAULTS.shadowCameraNear,
                        currentLight.shadowCamera?.[5] ?? LIGHT_DEFAULTS.shadowCameraFar,
                      ],
                    }))
                  }}
                />
              )}
              <RowNumberField
                label={t('light_configurator.shadow_blur_amount.label')}
                id='shadowBlurAmount'
                step={1}
                value={light.shadowRadius ?? LIGHT_DEFAULTS.shadowRadius}
                onChange={(shadowRadius) => {
                  onChange(currentLight => ({
                    ...currentLight,
                    shadowRadius,
                  }))
                }}
              />
            </div>
          )}
          <RowNumberField
            label={t('light_configurator.shadow_bias.label')}
            id='shadowBias'
            step={0.0001}
            value={light?.shadowBias ?? LIGHT_DEFAULTS.shadowBias}
            onChange={(e) => {
              onChange(currentLight => ({
                ...currentLight,
                shadowBias: e,
              }))
            }}
          />
          <RowNumberField
            label={t('light_configurator.shadow_normal_bias.label')}
            id='normalShadowBias'
            step={0.0001}
            value={light?.shadowNormalBias ?? LIGHT_DEFAULTS.shadowNormalBias}
            onChange={(e) => {
              onChange(currentLight => ({
                ...currentLight,
                shadowNormalBias: e,
              }))
            }}
          />
        </>
      }
      {(light.type === 'point' || light.type === 'spot') &&
        <>
          <RowNumberField
            id='distance'
            label={t('light_configurator.distance.label')}
            step={0.1}
            value={light?.distance ?? LIGHT_DEFAULTS.distance}
            onChange={(newValue) => {
              onChange(currentLight => ({
                ...currentLight,
                distance: newValue,
              }))
            }}
          />
          <RowNumberField
            id='decay'
            label={t('light_configurator.decay.label')}
            step={0.1}
            value={light?.decay ?? LIGHT_DEFAULTS.decay}
            onChange={(newValue) => {
              onChange(currentLight => ({
                ...currentLight,
                decay: newValue,
              }))
            }}
          />
        </>
      }
      {light.type === 'spot' && (
        <>
          <RowNumberField
            id='angle'
            label={t('light_configurator.angle.label')}
            step={1}
            value={radiansToDegrees(light?.angle ?? LIGHT_DEFAULTS.angle)}
            min={0}
            max={90}
            onChange={(degree) => {
              // Convert the angle back to radians
              const angle = degreesToRadians(degree)
              onChange(currentLight => ({
                ...currentLight,
                angle,
              }))
            }}
          />
          <RowNumberField
            id='penumbra'
            label={t('light_configurator.penumbra.label')}
            step={0.1}
            value={light?.penumbra ?? LIGHT_DEFAULTS.penumbra}
            min={0}
            max={1}
            onChange={(penumbra) => {
              onChange(currentLight => ({
                ...currentLight,
                penumbra,
              }))
            }}
          />
        </>
      )}
      {light.type === 'area' && (
        <>
          <RowNumberField
            id='width'
            label={t('light_configurator.width.label')}
            step={0.1}
            value={light?.width ?? LIGHT_DEFAULTS.width}
            onChange={(newValue) => {
              onChange(currentLight => ({
                ...currentLight,
                width: newValue,
              }))
            }}
          />
          <RowNumberField
            id='height'
            label={t('light_configurator.height.label')}
            step={0.1}
            value={light?.height ?? LIGHT_DEFAULTS.height}
            onChange={(newValue) => {
              onChange(currentLight => ({
                ...currentLight,
                height: newValue,
              }))
            }}
          />
        </>
      )}
    </ComponentConfiguratorTray>
  )
}

export {
  LightConfigurator,
}
