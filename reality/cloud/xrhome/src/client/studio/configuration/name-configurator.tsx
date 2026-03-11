import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {isMissingPrefab, isPrefab} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from '../scene-context'
import {createThemedStyles} from '../../ui/theme'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {useSelectedObjects} from '../hooks/selected-objects'
import {FloatingTrayCheckboxField} from '../../ui/components/floating-tray-checkbox-field'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {handleBlurInputField} from './row-fields'
import {IconButton} from '../../ui/components/icon-button'
import {Popup} from '../../ui/components/popup'
import {useActiveSpace} from '../hooks/active-space'
import {StaticBanner} from '../../ui/components/banner'
import {prefabNameExists} from '../common/studio-files'
import {useDerivedScene} from '../derived-scene-context'

const useStyles = createThemedStyles(theme => ({
  nameConfiguratorContainer: {
    fontSize: '12px',
    padding: '1em 1em',
    borderBottom: theme.studioSectionBorder,
    display: 'grid',
    gridTemplateColumns: '1fr auto',
    gridGap: '0.5em',
  },
  nameConfiguratorInputRow: {
    display: 'flex',
    gap: '0.5em',
    alignItems: 'center',
  },
  fullWidthRow: {
    gridColumn: '1 / -1',
  },
  nameConfiguratorInput: {
    width: '100%',
  },
}))

const getSharedName = (objects: DeepReadonly<GraphObject[]>) => {
  const names = objects.map(object => object.name).filter(Boolean)
  if (new Set(names).size === 1) {
    return names[0]
  } else {
    return ''
  }
}

const getSharedBooleanStatus = (
  objects: DeepReadonly<GraphObject[]>, property: keyof GraphObject
) => objects.every(object => object[property])

interface INameConfigurator {
  showActiveCameraBtn?: boolean
  isActive?: boolean
  onSetActiveCamera?: () => void
}

const NameConfigurator: React.FC<INameConfigurator> = (
  {showActiveCameraBtn, isActive, onSetActiveCamera}
) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const objects = useSelectedObjects()
  const activeSpace = useActiveSpace()

  const missingPrefab = objects.some(object => isMissingPrefab(ctx.scene, object.id))
  const duplicatePrefabName = objects.some(object => (
    isPrefab(object) && prefabNameExists(derivedScene, object.name, object.id)
  ))

  const isRoot = objects.every(
    object => object.parentId === derivedScene.resolveSpaceForObject(object.id)?.id
  )

  const handleNameChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    objects.forEach((object) => {
      ctx.updateObject(object.id, o => ({...o, name: e.target.value}))
    })
  }

  const handleToggleChange = (property: keyof GraphObject) => (
    e: React.ChangeEvent<HTMLInputElement> | React.MouseEvent<HTMLButtonElement>
  ) => {
    e.stopPropagation()
    const newStatus = !getSharedBooleanStatus(objects, property) || undefined
    objects.forEach((object) => {
      ctx.updateObject(object.id, o => ({...o, [property]: newStatus}))
    })
  }

  const isHidden = getSharedBooleanStatus(objects, 'hidden')
  const isPersistent = getSharedBooleanStatus(objects, 'persistent')

  const disableHidden = objects.some(obj => obj.mapPoint)

  return (
    <div className={classes.nameConfiguratorContainer}>
      <div className={classes.nameConfiguratorInputRow}>
        <FloatingTrayCheckboxField
          label={t('name_configurator.enable_object.label')}
          id='object-disabled-toggle'
          checked={!getSharedBooleanStatus(objects, 'disabled')}
          onChange={handleToggleChange('disabled')}
          sr
        />
        <div className={classes.nameConfiguratorInput}>
          <StandardTextField
            label={null}
            aria-label={t('name_configurator.object_name.label')}
            id='object-name'
            value={getSharedName(objects)}
            height='small'
            onChange={handleNameChange}
            onFocus={e => e.target.select()}
            onKeyDown={handleBlurInputField}
          />
        </div>

        <Popup
          content={
          isHidden
            ? t('name_configurator.tooltip.hidden')
            : t('name_configurator.tooltip.visible')
        }
          position='bottom'
          alignment='right'
          size='tiny'
        >
          <IconButton
            text={isHidden
              ? t('name_configurator.button.show_element')
              : t('name_configurator.button.hide_element')}
            onClick={handleToggleChange('hidden')}
            stroke={isHidden ? 'eyeClosed' : 'eyeOpen'}
            disabled={disableHidden}
          />
        </Popup>
        {isRoot && activeSpace &&
          <Popup
            content={
          isPersistent
            ? t('name_configurator.tooltip.persistent')
            : t('name_configurator.tooltip.not_persistent')
        }
            position='bottom'
            alignment='right'
            size='tiny'
          >
            <IconButton
              text={isPersistent
                ? t('name_configurator.button.remove_persistent')
                : t('name_configurator.button.set_persistent')}
              onClick={handleToggleChange('persistent')}
              stroke={isPersistent ? 'infinityLoop' : 'pill'}
            />
          </Popup>}
      </div>
      {showActiveCameraBtn && onSetActiveCamera &&
        <div className={classes.fullWidthRow}>
          <FloatingPanelButton
            spacing='full'
            onClick={onSetActiveCamera}
            disabled={isActive}
            transparentOnDisable
          >
            {!isActive
              ? t('name_configurator.button.set_active_camera')
              : t('name_configurator.button.current_active_camera')}
          </FloatingPanelButton>
        </div>
      }
      {missingPrefab &&
        <div className={classes.fullWidthRow}>
          <StaticBanner
            type='danger'
            message={t('name_configurator.banner.missing_prefab')}
          />
        </div>
      }
      {duplicatePrefabName &&
        <div className={classes.fullWidthRow}>
          <StaticBanner
            type='danger'
            message={t('name_configurator.banner.duplicate_prefab_name')}
          />
        </div>
      }
    </div>
  )
}

export {
  NameConfigurator,
}
