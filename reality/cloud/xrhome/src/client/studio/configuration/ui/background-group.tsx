import React from 'react'
import {getBackgroundOpacity, UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {useTranslation} from 'react-i18next'
import type {BackgroundSize, Resource, UiGraphSettings} from '@ecs/shared/scene-graph'

import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {Icon} from '../../../ui/components/icon'
import {
  ColorPicker, DeleteGroupButton, NumberInput, NumberOrPercentInput, RowGroupFields, RowNumberField,
  RowSelectField,
} from '../row-fields'
import {CompactImagePicker} from '../../ui/compact-image-picker'
import {IconButton} from '../../../ui/components/icon-button'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {combine} from '../../../common/styles'
import type {UiPropertyName} from '../ui-configurator-types'

const UI_BORDER_RADIUS_PLACEHOLDER = '0'

const getIsNineSliceLocked = (settings: UiGraphSettings): boolean => (
  settings.nineSliceBorderTop === settings.nineSliceBorderBottom &&
    settings.nineSliceBorderLeft === settings.nineSliceBorderRight
)

interface IBackgroundGroup {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDelete: (key: UiPropertyName) => void
  onDeleteGroup: () => void
}

const BackgroundGroup: React.FC<IBackgroundGroup> = ({
  settings, onChange, onDelete, onDeleteGroup,
}) => {
  const classes = useUiConfiguratorStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const [isNineSliceLocked, SetIsNineSliceLocked] = React.useState(getIsNineSliceLocked(settings))

  const handleSrcChange = (resource: Resource, key: keyof UiGraphSettings) => {
    onChange(current => current && ({...current, [key]: resource ?? null}))
  }

  return (
    <>
      <div className={classes.groupHeaderRow}>
        {t('ui_configurator.ui_background_label.label')}
        <DeleteGroupButton
          onClick={onDeleteGroup}
        />
      </div>
      <RowNumberField
        id='background-opacity'
        label={t('ui_configurator.background_opacity.label')}
        value={
          getBackgroundOpacity(settings.backgroundOpacity, !!settings.image || !!settings.video)
        }
        onChange={value => onChange(current => ({...current, backgroundOpacity: value}))}
        min={0}
        max={1}
        step={0.01}
        fixed={2}
      />
      <RowGroupFields label={t('ui_configurator.ui_color.label')}>
        <ColorPicker
          id='background'
          ariaLabel={t('ui_configurator.background.label')}
          value={settings.background ?? UI_DEFAULTS.background}
          onChange={value => onChange(current => ({...current, background: value}))}
          defaultValue={UI_DEFAULTS.background}
        />
        <CompactImagePicker
          assetKind='image'
          ariaLabel={t('ui_configurator.image.label')}
          resource={settings?.image}
          onChange={(value) => {
            handleSrcChange(value, 'image')
            onChange(current => ({...current, backgroundSize: UI_DEFAULTS.backgroundSize}))
          }}
        />
      </RowGroupFields>
      <RowSelectField
        id='background-size'
        label={t('ui_configurator.background_size.label')}
        value={settings.backgroundSize ?? UI_DEFAULTS.backgroundSize}
        onChange={(value) => {
          onChange(current => ({
            ...current,
            backgroundSize: value as BackgroundSize,
          }))
        }
        }
        options={[
          {value: 'stretch', content: t('ui_configurator.background_size.option.stretch')},
          {value: 'contain', content: t('ui_configurator.background_size.option.contain')},
          {value: 'cover', content: t('ui_configurator.background_size.option.cover')},
          {
            value: 'nineslice',
            content: t('ui_configurator.background_size.option.nineslice'),
          },
        ].filter(Boolean)}
      />
      {settings.backgroundSize === 'nineslice' && (
        <div>
          {!isNineSliceLocked &&
            <RowGroupFields label={t('ui_configurator.background_size.option.nineslice.label')}>
              <div className={classes.columnContainer}>
                <NumberOrPercentInput
                  id='nineSliceBorderTop'
                  onChange={value => onChange(current => ({
                    ...current,
                    nineSliceBorderTop: value,
                  }))}
                  value={settings.nineSliceBorderTop ?? UI_DEFAULTS.nineSliceBorderTop}
                  icon={<Icon stroke='paddingVerticalTop' color='muted' />}
                  defaultValue={UI_DEFAULTS.nineSliceBorderTop}
                />
                <NumberOrPercentInput
                  id='nineSliceBorderBottom'
                  onChange={value => onChange(current => ({
                    ...current,
                    nineSliceBorderBottom: value,
                  }))}
                  value={settings.nineSliceBorderBottom ?? UI_DEFAULTS.nineSliceBorderBottom}
                  icon={<Icon stroke='paddingVerticalBottom' color='muted' />}
                  defaultValue={UI_DEFAULTS.nineSliceBorderBottom}
                />
              </div>
              <div className={classes.columnContainer}>
                <NumberOrPercentInput
                  id='nineSliceBorderLeft'
                  onChange={value => onChange(current => ({
                    ...current,
                    nineSliceBorderLeft: value,
                  }))}
                  value={settings.nineSliceBorderLeft ?? UI_DEFAULTS.nineSliceBorderLeft}
                  icon={<Icon stroke='paddingHorizontalLeft' color='muted' />}
                  defaultValue={UI_DEFAULTS.nineSliceBorderLeft}
                />
                <NumberOrPercentInput
                  id='nineSliceBorderRight'
                  onChange={value => onChange(current => ({
                    ...current,
                    nineSliceBorderRight: value,
                  }))}
                  value={settings.nineSliceBorderRight ?? UI_DEFAULTS.nineSliceBorderRight}
                  icon={<Icon stroke='paddingHorizontalRight' color='muted' />}
                  defaultValue={UI_DEFAULTS.nineSliceBorderRight}
                />
              </div>
              <div>
                <StandardFieldContainer>
                  <div className={classes.iconButton}>
                    <IconButton
                      stroke='chainBroken'
                      text={t('ui_configurator.background_size.option.nineslice.unlock')}
                      onClick={() => {
                        onChange(current => ({
                          ...current,
                          nineSliceBorderBottom: settings.nineSliceBorderTop,
                          nineSliceBorderRight: settings.nineSliceBorderLeft,
                        }))
                        SetIsNineSliceLocked(true)
                      }}
                    />
                  </div>
                </StandardFieldContainer>
              </div>
            </RowGroupFields>
          }
          {isNineSliceLocked &&
            <RowGroupFields label={t('ui_configurator.background_size.option.nineslice.label')}>
              <NumberOrPercentInput
                id='nineSliceBorderTop'
                onChange={value => onChange(current => ({
                  ...current,
                  nineSliceBorderTop: value,
                  nineSliceBorderBottom: value,
                }))}
                value={settings.nineSliceBorderTop ?? UI_DEFAULTS.nineSliceBorderTop}
                icon={<Icon stroke='paddingVertical' color='muted' />}
                defaultValue={UI_DEFAULTS.nineSliceBorderTop}
              />
              <NumberOrPercentInput
                id='nineSliceBorderLeft'
                onChange={value => onChange(current => ({
                  ...current,
                  nineSliceBorderLeft: value,
                  nineSliceBorderRight: value,
                }))}
                value={settings.nineSliceBorderLeft ?? UI_DEFAULTS.nineSliceBorderLeft}
                icon={<Icon stroke='paddingHorizontal' color='muted' />}
                defaultValue={UI_DEFAULTS.nineSliceBorderLeft}
              />
              <div>
                <StandardFieldContainer>
                  <div className={classes.iconButton}>
                    <IconButton
                      stroke='chain'
                      text={t('ui_configurator.background_size.option.nineslice.lock')}
                      onClick={() => {
                        onChange(current => ({
                          ...current,
                          nineSliceBorderBottom: current.nineSliceBorderTop,
                          nineSliceBorderRight: current.nineSliceBorderLeft,
                        }))
                        SetIsNineSliceLocked(false)
                      }}
                    />
                  </div>
                </StandardFieldContainer>
              </div>
            </RowGroupFields>
          }
          <RowNumberField
            id='nineSliceScaleFactor'
            onChange={value => onChange(current => ({
              ...current,
              nineSliceScaleFactor: value,
            }))}
            value={settings.nineSliceScaleFactor ?? UI_DEFAULTS.nineSliceScaleFactor}
            min={0}
            step={0.1}
            label={t('ui_configurator.background_size.option.nineslice.scaleFactor')}
          />
        </div>
      )}
      {settings.borderRadiusTopLeft === undefined &&
        <RowGroupFields label={t('ui_configurator.ui_corner_radius.label')}>
          <NumberInput
            id='ui-border-radius'
            value={settings.borderRadius ?? UI_DEFAULTS.borderRadius}
            onChange={value => onChange(current => ({...current, borderRadius: value}))}
            min={0}
            step={1}
            placeholder={UI_BORDER_RADIUS_PLACEHOLDER}
            icon={<Icon stroke='cornerRadiusAll' color='muted' />}
            ariaLabel={t('ui_configurator.ui_corner_radius.option.all')}
            defaultValue={UI_DEFAULTS.borderRadius}
          />
          <div>
            <StandardFieldContainer>
              <div className={classes.iconButton}>
                <IconButton
                  stroke='chain'
                  text={t('ui_configurator.ui_corner_radius.unlock')}
                  onClick={() => {
                    onChange(current => ({
                      ...current,
                      borderRadiusTopLeft: settings.borderRadius?.toString() ??
                        UI_DEFAULTS.borderRadiusTopLeft,
                      borderRadiusTopRight: settings.borderRadius?.toString() ??
                        UI_DEFAULTS.borderRadiusTopRight,
                      borderRadiusBottomLeft: settings.borderRadius?.toString() ??
                        UI_DEFAULTS.borderRadiusBottomLeft,
                      borderRadiusBottomRight: settings.borderRadius?.toString() ??
                        UI_DEFAULTS.borderRadiusBottomRight,
                    }))
                    onDelete('borderRadius')
                  }}
                />
              </div>
            </StandardFieldContainer>
          </div>
        </RowGroupFields>
      }
      {settings.borderRadiusTopLeft !== undefined &&
        <div className={combine('style-reset', classes.columnContainer)}>
          <RowGroupFields label={t('ui_configurator.ui_corner_radius.label')}>
            <div className={classes.columnContainer}>
              <NumberOrPercentInput
                id='ui-border-radius-top-left'
                onChange={value => onChange(current => ({
                  ...current,
                  borderRadiusTopLeft: value,
                }))}
                value={settings.borderRadiusTopLeft ??
                  String(settings.borderRadius ?? UI_DEFAULTS.borderRadius)}
                placeholder={UI_BORDER_RADIUS_PLACEHOLDER}
                icon={<Icon stroke='cornerRadiusTopLeft' color='muted' />}
                ariaLabel={t('ui_configurator.ui_corner_radius.option.top_left')}
                defaultValue={UI_DEFAULTS.borderRadiusTopLeft}
              />
              <NumberOrPercentInput
                id='ui-border-radius-bottom-left'
                onChange={value => onChange(current => ({
                  ...current,
                  borderRadiusBottomLeft: value,
                }))}
                value={settings.borderRadiusBottomLeft ??
                  String(settings.borderRadius ?? UI_DEFAULTS.borderRadius)}
                placeholder={UI_BORDER_RADIUS_PLACEHOLDER}
                icon={<Icon stroke='cornerRadiusBottomLeft' color='muted' />}
                ariaLabel={t('ui_configurator.ui_corner_radius.option.bottom_left')}
                defaultValue={UI_DEFAULTS.borderRadiusBottomLeft}
              />
            </div>
            <div className={classes.columnContainer}>
              <NumberOrPercentInput
                id='ui-border-radius-top-right'
                onChange={value => onChange(current => ({
                  ...current,
                  borderRadiusTopRight: value,
                }))}
                value={settings.borderRadiusTopRight ??
                  String(settings.borderRadius ?? UI_DEFAULTS.borderRadius)}
                placeholder={UI_BORDER_RADIUS_PLACEHOLDER}
                icon={<Icon stroke='cornerRadiusTopRight' color='muted' />}
                ariaLabel={t('ui_configurator.ui_corner_radius.option.top_right')}
                defaultValue={UI_DEFAULTS.borderRadiusTopRight}
              />
              <NumberOrPercentInput
                id='ui-border-radius-bottom-right'
                onChange={value => onChange(current => ({
                  ...current,
                  borderRadiusBottomRight: value,
                }))}
                value={settings.borderRadiusBottomRight ??
                  String(settings.borderRadius ?? UI_DEFAULTS.borderRadius)}
                placeholder={UI_BORDER_RADIUS_PLACEHOLDER}
                icon={<Icon stroke='cornerRadiusBottomRight' color='muted' />}
                ariaLabel={t('ui_configurator.ui_corner_radius.option.bottom_right')}
                defaultValue={UI_DEFAULTS.borderRadiusBottomRight}
              />
            </div>
            <div>
              <StandardFieldContainer>
                <div className={classes.iconButton}>
                  <IconButton
                    stroke='chainBroken'
                    text={t('ui_configurator.ui_corner_radius.lock')}
                    onClick={() => {
                      onChange(current => ({
                        ...current,
                        borderRadius: Number.isNaN(parseFloat(settings.borderRadiusTopLeft))
                          ? UI_DEFAULTS.borderRadius
                          : parseFloat(settings.borderRadiusTopLeft),
                      }))
                      onDelete('borderRadiusTopLeft')
                      onDelete('borderRadiusTopRight')
                      onDelete('borderRadiusBottomLeft')
                      onDelete('borderRadiusBottomRight')
                    }}
                  />
                </div>
              </StandardFieldContainer>
            </div>
          </RowGroupFields>
        </div>
      }
    </>
  )
}

export {
  BackgroundGroup,
}
