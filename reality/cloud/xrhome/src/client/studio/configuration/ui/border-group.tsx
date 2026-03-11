import React from 'react'
import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'

import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {DeleteGroupButton, RowColorField, RowNumberField} from '../row-fields'

interface IBorderGroup {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDeleteGroup: () => void
}

const BorderGroup: React.FC<IBorderGroup> = ({settings, onChange, onDeleteGroup}) => {
  const classes = useUiConfiguratorStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <>
      <div className={classes.groupHeaderRow}>
        {t('ui_property_strings.category.border')}
        <DeleteGroupButton
          onClick={onDeleteGroup}
        />
      </div>
      <RowColorField
        id='ui-border-color'
        label={t('ui_configurator.ui_border_color.label')}
        value={settings.borderColor ?? UI_DEFAULTS.borderColor}
        onChange={value => onChange(current => ({...current, borderColor: value}))}
      />
      <RowNumberField
        id='ui-border-width'
        label={t('ui_configurator.ui_border_width.label')}
        value={settings.borderWidth ?? UI_DEFAULTS.borderWidth}
        onChange={value => onChange(current => ({...current, borderWidth: value}))}
        min={0}
        step={1}
      />
    </>
  )
}

export {BorderGroup}
