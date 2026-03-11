import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import React from 'react'
import {useTranslation} from 'react-i18next'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'

import {SrOnly} from '../../../ui/components/sr-only'
import {
  NumberOrPercentInput, RowGroupFields, RowSelectField, useStyles as useRowStyles,
} from '../row-fields'
import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {StandardFieldLabel} from '../../../ui/components/standard-field-label'
import {LayoutMode, VisualAligner} from '../../../ui/components/visual-aligner'
import {
  getAlignItemsFromDirection, getAlignments, getJustifyContentFromDirection,
} from './visual-aligner-definition'

const UI_GAP_PLACEHOLDER = '0'

interface IAlignItemsDropdown {
  settings: UiGraphSettings
  icon: React.ReactNode
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
}

const AlignItemsDropdown: React.FC<IAlignItemsDropdown> = ({settings, icon, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  return (
    <RowSelectField
      id='ui-align-items'
      label={(<SrOnly> {t('ui_configurator.ui_align_items.label')}</SrOnly>)}
      value={settings.alignItems ?? UI_DEFAULTS.alignItems}
      options={[
        {
          value: 'flex-start',
          content: t(`ui_configurator.ui_text_align.option.${icon === 'X' ? 'left' : 'top'}`),
        },
        {
          value: 'center',
          content: t('ui_configurator.ui_text_align.option.center'),
        },
        {
          value: 'flex-end',
          content: t(`ui_configurator.ui_text_align.option.${icon === 'X' ? 'right' : 'bottom'}`),
        },
      ]}
      onChange={value => onChange(current => ({
        ...current, alignItems: value,
      }))}
      icon={icon}
      formatLabel={false}
    />
  )
}
interface IJustifyContentDropdown {
  settings: UiGraphSettings
  icon: React.ReactNode
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  isReversed?: boolean
}

const JustifyContentDropdown: React.FC<IJustifyContentDropdown> = (
  {settings, icon, onChange, isReversed = false}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  return (
    <RowSelectField
      id='ui-justify-content'
      label={(<SrOnly> {t('ui_configurator.ui_justify_content.label')}</SrOnly>)}
      value={settings.justifyContent ?? UI_DEFAULTS.justifyContent}
      options={[
        {
          value: isReversed ? 'flex-end' : 'flex-start',
          content: t(`ui_configurator.ui_text_align.option.${icon === 'X' ? 'left' : 'top'}`),
        },
        {
          value: 'center',
          content: t('ui_configurator.ui_text_align.option.center'),
        },
        {
          value: isReversed ? 'flex-start' : 'flex-end',
          content: t(`ui_configurator.ui_text_align.option.${icon === 'X' ? 'right' : 'bottom'}`),
        },
        {
          value: 'space-between',
          content: t('ui_configurator.ui_justify_content.option.space_between'),
        },
        // if we want to make these conditional we can do value === 'space-around' &&
        // {value 'space-around', ...},with a filter(Boolean) to make the option only
        // show up if it was already selected.
        {
          value: 'space-around',
          content: t('ui_configurator.ui_justify_content.option.space_around'),
        },
        {
          value: 'space-evenly',
          content: t('ui_configurator.ui_justify_content.option.space_evenly'),
        },
      ]}
      onChange={value => onChange(current => ({
        ...current, justifyContent: value,
      }))}
      icon={icon}
      formatLabel={false}
    />
  )
}

interface ILayoutModeConfiguratorProps {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
}
const LayoutModeConfigurator: React.FC<ILayoutModeConfiguratorProps> = ({
  settings,
  onChange,
}) => {
  const classes = useUiConfiguratorStyles()
  const rowClasses = useRowStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const isRow = ['row', 'row-reverse'].includes(settings.flexDirection)
  const isReversed = ['row-reverse', 'column-reverse'].includes(settings.flexDirection)

  return (
    <RowGroupFields
      label={<SrOnly>{t('ui_configurator.ui_layout_mode.label')}</SrOnly>}
      formatLabel={false}
    >
      <div className={classes.columnContainer}>
        <div className={rowClasses.flexItem}>
          <StandardFieldLabel
            label={t('ui_configurator.ui_align.label')}
            mutedColor
          />
          {isRow
            ? (
              <div className={rowClasses.noPadding}>
                <JustifyContentDropdown
                  settings={settings}
                  icon='X'
                  onChange={onChange}
                  isReversed={isReversed}
                />
                <AlignItemsDropdown
                  settings={settings}
                  icon='Y'
                  onChange={onChange}
                />
              </div>
            )
            : (
              <div className={rowClasses.noPadding}>
                <AlignItemsDropdown
                  settings={settings}
                  icon='X'
                  onChange={onChange}
                />
                <JustifyContentDropdown
                  settings={settings}
                  icon='Y'
                  onChange={onChange}
                  isReversed={isReversed}
                />
              </div>
            )
            }
          <NumberOrPercentInput
            id='ui-gap'
            value={settings.gap ?? UI_DEFAULTS.gap}
            onChange={value => onChange(current => ({
              ...current,
              gap: value,
            }))}
            min={0}
            step={1}
            placeholder={UI_GAP_PLACEHOLDER}
            icon={t('ui_configurator.ui_gap.label')}
            ariaLabel={t('ui_configurator.ui_gap.label')}
            defaultValue={UI_DEFAULTS.gap}
          />
        </div>
      </div>
      <VisualAligner
        layoutMode={LayoutMode.STACK}
        direction={isRow ? 'row' : 'column'}
        value={getAlignments(
          settings.flexDirection ?? UI_DEFAULTS.flexDirection,
          settings.justifyContent ?? UI_DEFAULTS.justifyContent,
          settings.alignItems ?? UI_DEFAULTS.alignItems
        )}
        onChange={([newHorizontalAlign, newVerticalAlign]) => {
          onChange(current => ({
            ...current,
            alignItems: getAlignItemsFromDirection(isRow
              ? newVerticalAlign
              : newHorizontalAlign),
            justifyContent: getJustifyContentFromDirection(isRow
              ? newHorizontalAlign
              : newVerticalAlign, isReversed),
          }))
        }}
      />
    </RowGroupFields>
  )
}

export default LayoutModeConfigurator
