import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import React from 'react'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {useTranslation} from 'react-i18next'

import {NumberOrPercentInput, RowBooleanField, RowGroupFields} from '../row-fields'
import {Icon} from '../../../ui/components/icon'
import {SrOnly} from '../../../ui/components/sr-only'

const UI_POSITION_PLACEHOLDER = '—'

interface IPositionConfiguratorProps {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDelete: (property: string) => void
}

const PositionConfigurator: React.FC<IPositionConfiguratorProps> = ({
  settings, onChange, onDelete,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <>
      <RowBooleanField
        id='ui-position-absolute'
        label={t('ui_configurator.ui_position_absolute.label')}
        checked={settings.position === 'absolute'}
        onChange={(e) => {
          if (e.target.checked) {
            onChange(current => ({
              ...current,
              position: 'absolute',
              top: settings.top ?? UI_DEFAULTS.top,
              left: settings.left ?? UI_DEFAULTS.left,
              bottom: settings.bottom ?? UI_DEFAULTS.bottom,
              right: settings.right ?? UI_DEFAULTS.right,
            }))
          } else {
            onDelete('position')
            onDelete('top')
            onDelete('left')
            onDelete('bottom')
            onDelete('right')
          }
        }}
      />
      {(settings.position ?? UI_DEFAULTS.position) !== 'static' && (
        <>
          <RowGroupFields
            label={<SrOnly>{t('ui_configurator.ui_position.label')}</SrOnly>}
            formatLabel={false}
          >
            <NumberOrPercentInput
              id='ui-top-location'
              value={settings.top ?? ''}
              onChange={e => onChange(current => ({
                ...current,
                top: e,
              }))}
              icon={(
                <Icon stroke='paddingVerticalTop' color='muted' />
              )}
              placeholder={UI_POSITION_PLACEHOLDER}
              defaultValue={UI_DEFAULTS.top}
              ariaLabel={t('ui_configurator.ui_position_top.label')}
            />
            <NumberOrPercentInput
              id='ui-left-location'
              value={settings.left ?? ''}
              onChange={e => onChange(current => ({
                ...current,
                left: e,
              }))}
              icon={(
                <Icon stroke='paddingHorizontalLeft' color='muted' />
              )}
              placeholder={UI_POSITION_PLACEHOLDER}
              defaultValue={UI_DEFAULTS.left}
              ariaLabel={t('ui_configurator.ui_position_left.label')}
            />
          </RowGroupFields>
          <RowGroupFields
            label={<SrOnly>{t('ui_configurator.ui_position.label')}</SrOnly>}
            formatLabel={false}
          >
            <NumberOrPercentInput
              id='ui-bottom-location'
              value={settings.bottom ?? ''}
              onChange={e => onChange(current => ({
                ...current,
                bottom: e,
              }))}
              icon={(
                <Icon stroke='paddingVerticalBottom' color='muted' />
              )}
              placeholder={UI_POSITION_PLACEHOLDER}
              defaultValue={UI_DEFAULTS.bottom}
              ariaLabel={t('ui_configurator.ui_position_bottom.label')}
            />
            <NumberOrPercentInput
              id='ui-right-location'
              value={settings.right ?? ''}
              onChange={e => onChange(current => ({
                ...current,
                right: e,
              }))}
              icon={(
                <Icon stroke='paddingHorizontalRight' color='muted' />
              )}
              placeholder={UI_POSITION_PLACEHOLDER}
              defaultValue={UI_DEFAULTS.right}
              ariaLabel={t('ui_configurator.ui_position_right.label')}
            />
          </RowGroupFields>
        </>
      )}

    </>
  )
}

export {
  PositionConfigurator,
}
