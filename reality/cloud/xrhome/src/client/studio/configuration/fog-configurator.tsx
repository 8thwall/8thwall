import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import type {Fog} from '@ecs/shared/scene-graph'
import {DEFAULT_COLOR, DEFAULT_DENSITY, DEFAULT_FAR, DEFAULT_NEAR} from '@ecs/shared/fog-constants'
import {getFogFromSceneGraph} from '@ecs/shared/get-fog-from-scene-graph'

import {RowColorField, RowNumberField, useStyles as useRowStyles} from './row-fields'
import {RowSelectField} from './row-fields'
import {MutateCallback, useSceneContext} from '../scene-context'
import {useActiveSpace} from '../hooks/active-space'
import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  flexRow: {
    'width': '100%',
    '&:not(:last-child)': {
      marginBottom: '0.5em',
    },
  },
  lastChildZeroMargin: {
    '&:last-child': {
      marginBottom: 0,
    },
  },
})

const FogConfigurator: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const rowClasses = useRowStyles()

  const ctx = useSceneContext()
  const activeSpace = useActiveSpace()

  if (!activeSpace?.id) {
    return null
  }

  const fog = getFogFromSceneGraph(ctx.scene, activeSpace.id)

  const updateFog = (cb: MutateCallback<Fog>) => {
    const updatedFog = cb(fog)
    ctx.updateScene(scene => ({
      ...scene,
      spaces: {
        ...scene.spaces,
        [activeSpace.id]: {
          ...scene.spaces[activeSpace.id],
          fog: updatedFog,
        },
      },
    }))
  }

  return (
    <div className={combine(classes.flexRow, classes.lastChildZeroMargin)}>
      <div className={rowClasses.flexItem}>
        <RowSelectField
          id='skybox-type'
          label={t('space_configurator.fog_type.label')}
          value={fog?.type || 'none'}
          options={[
            {value: 'linear', content: t('space_configurator.fog_type.option.linear')},
            {value: 'exponential', content: t('space_configurator.fog_type.option.exponential')},
            {value: 'none', content: t('space_configurator.skybox_type.option.none')},
          ]}
          onChange={(value) => {
            if (value === 'none') {
              updateFog(() => null)
            } else if (value === 'linear') {
              updateFog(() => ({
                type: 'linear',
                near: DEFAULT_NEAR,
                far: DEFAULT_FAR,
                color: DEFAULT_COLOR,
              }))
            } else if (value === 'exponential') {
              updateFog(() => ({
                type: 'exponential',
                density: DEFAULT_DENSITY,
                color: DEFAULT_COLOR,
              }))
            }
          }}
        />
        <div className={rowClasses.indent}>
          {fog && fog.type !== 'none' &&
            <RowColorField
              id='fog-color'
              label={t('space_configurator.fog_color.label')}
              onChange={(value) => {
                updateFog(oldFog => ({
                  ...oldFog,
                  color: value,
                }))
              }}
              value={fog.color ?? DEFAULT_COLOR}
            />
          }
          {fog?.type === 'linear' &&
            <>
              <RowNumberField
                id='fog-near'
                label={t('space_configurator.fog_near.label')}
                onChange={(value) => {
                  updateFog(oldFog => ({
                    ...oldFog,
                    near: value,
                  }))
                }}
                value={fog.near ?? DEFAULT_NEAR}
                min={0}
                step={0.01}
              />
              <RowNumberField
                id='fog-far'
                label={t('space_configurator.fog_far.label')}
                onChange={(value) => {
                  updateFog(oldFog => ({
                    ...oldFog,
                    far: value,
                  }))
                }}
                value={fog.far ?? DEFAULT_FAR}
                step={0.01}
              />
            </>
          }
          {fog?.type === 'exponential' &&
            <RowNumberField
              id='fog-density'
              label={t('space_configurator.fog_density.label')}
              onChange={(value) => {
                updateFog(oldFog => ({
                  ...oldFog,
                  density: value,
                }))
              }}
              value={fog.density ?? DEFAULT_DENSITY}
              min={0}
              step={0.00001}
            />
          }
        </div>
      </div>
    </div>
  )
}

export {
  FogConfigurator,
}
