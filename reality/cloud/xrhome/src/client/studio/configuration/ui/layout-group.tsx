import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import React from 'react'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {useTranslation} from 'react-i18next'

import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {
  DeleteGroupButton, RowBooleanField, RowGroupFields,
} from '../row-fields'
import {Icon} from '../../../ui/components/icon'
import LayoutModeConfigurator from './layout-mode-configurator'
import {PaddingConfigurator} from './padding-configurator'
import {MarginConfigurator} from './margin-configurator'
import type {UiPropertyName} from '../ui-configurator-types'
import {JointToggleButton} from '../../../ui/components/joint-toggle-button'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {IconButton} from '../../../ui/components/icon-button'

interface ILayoutGroup {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDelete: (key: UiPropertyName) => void
  onDeleteGroup: () => void
}

const LayoutGroup: React.FC<ILayoutGroup> = ({
  settings, onChange, onDelete, onDeleteGroup,
}) => {
  const classes = useUiConfiguratorStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const isRow = ['row', 'row-reverse'].includes(settings.flexDirection)
  const isReversed = ['row-reverse', 'column-reverse'].includes(settings.flexDirection)

  return (
    <>
      <div className={classes.groupHeaderRow}>
        {t('ui_configurator.ui_layout_label.label')}
        <DeleteGroupButton
          onClick={onDeleteGroup}
        />
      </div>
      {isReversed
        ? (
          <RowGroupFields
            label={t('ui_configurator.ui_layout_direction.label')}
          >
            <JointToggleButton
              value={settings.flexDirection ?? UI_DEFAULTS.flexDirection}
              options={[
                {
                  value: 'row-reverse',
                  content: <Icon stroke='directionsRows' />,
                },
                {
                  value: 'column-reverse',
                  content: <Icon stroke='directionsColumns' />,
                },
              ]}
              onChange={(value) => {
                onChange(current => ({
                  ...current,
                  flexDirection: value,
                }))
              }}
            />
            <StandardFieldContainer>
              <div className={classes.iconButton}>
                <IconButton
                  stroke='reverseDirection'
                  text={t('ui_configurator.ui_flex_direction.reverse.label')}
                  onClick={() => {
                    const newDirection = isRow ? 'row' : 'column'
                    onChange(current => ({
                      ...current,
                      flexDirection: newDirection,
                    }))
                  }}
                  color='main'
                />
              </div>
            </StandardFieldContainer>
          </RowGroupFields>
        )
        : (
          <RowGroupFields
            label={t('ui_configurator.ui_layout_direction.label')}
          >
            <JointToggleButton
              value={settings.flexDirection ?? UI_DEFAULTS.flexDirection}
              options={[
                {
                  value: 'row',
                  content: <Icon stroke='directionsRows' />,
                },
                {
                  value: 'column',
                  content: <Icon stroke='directionsColumns' />,
                },
              ]}
              onChange={(value) => {
                onChange(current => ({
                  ...current,
                  flexDirection: value,
                }))
              }}
            />
            <div className={classes.iconButton}>
              <IconButton
                stroke='reverseDirection'
                text={t('ui_configurator.ui_flex_direction.reverse.label')}
                onClick={() => {
                  const newDirection = isRow ? 'row-reverse' : 'column-reverse'
                  onChange(current => ({
                    ...current,
                    flexDirection: newDirection,
                  }))
                }}
                color='muted'
              />
            </div>
          </RowGroupFields>
        )}
      <LayoutModeConfigurator
        settings={settings}
        onChange={onChange}
      />
      <PaddingConfigurator
        settings={settings}
        onChange={onChange}
      />
      <MarginConfigurator
        settings={settings}
        onChange={onChange}
        onDelete={onDelete}
      />
      <RowBooleanField
        id='ui-flex-wrap'
        label={t('ui_configurator.ui_flex_wrap.label')}
        checked={settings.flexWrap === 'wrap'}
        onChange={e => onChange(current => ({
          ...current, flexWrap: e.target.checked ? 'wrap' : UI_DEFAULTS.flexWrap,
        }))}
      />
    </>
  )
}

export {
  LayoutGroup,
}
