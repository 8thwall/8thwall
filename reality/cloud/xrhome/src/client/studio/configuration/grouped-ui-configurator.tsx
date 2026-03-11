/* eslint-disable react/jsx-closing-tag-location */
import React from 'react'
import {useTranslation} from 'react-i18next'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import type {UiGraphSettings} from '@ecs/shared/scene-graph'

import {
  RowBooleanField, RowJointToggleButton, RowNumberField,
} from './row-fields'
import {useRootAttributeId} from '../hooks/use-root-attribute-id'
import {copyDirectProperties} from './copy-component'
import {useStudioStateContext} from '../studio-state-context'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {UI_COMPONENT} from '../hooks/available-components'
import {UI_COMPONENT as GRAPH_UI_COMPONENT} from './direct-property-components'
import {GroupedUIPropertySelector} from './ui-property-selector'
import SpaceBelow from '../../ui/layout/space-below'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {getNumericValue, SizeConfigurator} from './ui/size-configurator'
import {LayoutGroup} from './ui/layout-group'
import {TextGroup} from './ui/text-group'
import {BackgroundGroup} from './ui/background-group'
import {BorderGroup} from './ui/border-group'
import type {UiGroup} from './ui-configurator-types'
import {removeUiGroup, isUiGroupPresent, addUiGroup} from './ui/ui-group-definition'
import {PositionConfigurator} from './ui/position-configurator'

interface GroupedUiConfiguratorProps {
  objectId: string
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  resetToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

const GroupedUiConfigurator: React.FC<GroupedUiConfiguratorProps> = ({
  objectId, settings, onChange, resetToPrefab,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const stateCtx = useStudioStateContext()
  const rootId = useRootAttributeId(objectId, 'ui')
  const isRoot = rootId === objectId
  const studioClasses = useStudioMenuStyles()

  const onDelete = (p: string) => {
    // note(alancastillo): hide property in the configurator
    onChange((current) => {
      const n = Object.keys(current)
        .filter(key => key !== p)
        .reduce((obj, key) => {
          obj[key] = current[key]
          return obj
        }, {})
      return n
    })
  }

  const onDeleteGroup = (group: UiGroup) => {
    onChange(current => removeUiGroup(group, current))
  }

  return (
    <ComponentConfiguratorTray
      title={t('ui_configurator.title')}
      description={t('ui_configurator.description')}
      sectionId={UI_COMPONENT}
      onRemove={() => onChange(() => undefined)}
      onCopy={() => copyDirectProperties(stateCtx, {ui: settings})}
      onResetToPrefab={resetToPrefab ? () => resetToPrefab([GRAPH_UI_COMPONENT]) : undefined}
      componentData={[GRAPH_UI_COMPONENT]}
    >
      {isRoot && <RowJointToggleButton
        id='ui-type'
        label={t('ui_configurator.ui_type.label')}
        value={settings.type ?? UI_DEFAULTS.type}
        options={[
          {value: 'overlay', content: t('ui_configurator.ui_type.option.overlay')},
          {value: '3d', content: t('ui_configurator.ui_type.option.3d')},
        ]}
        onChange={value => (
          onChange(current => ({
            ...current,
            type: value,
            // if the user tries to switch back to 3d while having a percentage value,
            //  we reset it back to a numeric value (non-percentage) assuming this is a root element
            width: (value === '3d' && isRoot) ? getNumericValue(current.width) : current.width,
            height: (value === '3d' && isRoot) ? getNumericValue(current.height) : current.height,
          }))
        )}
      />
      }
      <RowNumberField
        id='opacity'
        label={t('ui_configurator.opacity.label')}
        value={settings.opacity ?? UI_DEFAULTS.opacity}
        onChange={value => onChange(current => ({...current, opacity: value}))}
        min={0}
        max={1}
        step={0.01}
        fixed={2}
        defaultValue={UI_DEFAULTS.opacity}
      />
      <RowBooleanField
        id='ignore-raycast'
        label={t('ui_configurator.ignore_raycast.label')}
        checked={settings.ignoreRaycast ?? UI_DEFAULTS.ignoreRaycast}
        onChange={e => onChange(current => ({...current, ignoreRaycast: e.target.checked}))}
      />
      {(!isRoot || settings.type !== '3d') &&
        <PositionConfigurator
          settings={settings}
          onChange={onChange}
          onDelete={onDelete}
        />
      }
      <RowNumberField
        id='ui-stacking-order'
        label={t('ui_configurator.ui_stacking_order.label')}
        placeholder={t('ui_configurator.ui_stacking_order.placeholder')}
        value={settings.stackingOrder ?? UI_DEFAULTS.stackingOrder}
        onChange={e => onChange(current => ({
          ...current,
          stackingOrder: e,
        }))}
        step={1}
        defaultValue={UI_DEFAULTS.stackingOrder}
        clearIfDefault
      />
      <SpaceBelow>
        <SizeConfigurator
          uiObjectId={objectId}
          settings={settings}
          isRoot={isRoot}
          onChange={onChange}
          onDelete={onDelete}
        />
        {isUiGroupPresent('layout', settings) &&
          <div>
            <div className={studioClasses.divider} />
            <LayoutGroup
              settings={settings}
              onChange={onChange}
              onDelete={onDelete}
              onDeleteGroup={() => onDeleteGroup('layout')}
            />
          </div>
        }
        {isUiGroupPresent('text', settings) &&
          <div>
            <div className={studioClasses.divider} />
            <TextGroup
              settings={settings}
              onChange={onChange}
              onDeleteGroup={() => onDeleteGroup('text')}
            />
          </div>
        }
        {isUiGroupPresent('background', settings) &&
          <div>
            <div className={studioClasses.divider} />
            <BackgroundGroup
              settings={settings}
              onChange={onChange}
              onDelete={onDelete}
              onDeleteGroup={() => onDeleteGroup('background')}
            />
          </div>
        }
        {isUiGroupPresent('border', settings) &&
          <div>
            <div className={studioClasses.divider} />
            <BorderGroup
              settings={settings}
              onChange={onChange}
              onDeleteGroup={() => onDeleteGroup('border')}
            />
          </div>
        }
      </SpaceBelow>
      <GroupedUIPropertySelector
        onSelect={group => onChange(current => addUiGroup(group, current))}
        settings={settings}
      />
    </ComponentConfiguratorTray>
  )
}

export {
  GroupedUiConfigurator,
}
