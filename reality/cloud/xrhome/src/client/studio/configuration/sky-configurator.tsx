import React from 'react'

import type {Sky} from '@ecs/shared/scene-graph'
import {
  DEFAULT_BOTTOM_COLOR, MIN_GRADIENT_COLORS, MAX_GRADIENT_COLORS,
} from '@ecs/shared/sky-constants'

import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {getSkyFromSceneGraph} from '@ecs/shared/get-sky-from-scene-graph'

import {RowColorField, useStyles as useRowStyles} from './row-fields'

import {RowSelectField} from './row-fields'
import {MutateCallback, useSceneContext} from '../scene-context'
import {AssetSelector} from '../ui/asset-selector'
import {FloatingPanelIconButton} from '../../ui/components/floating-panel-icon-button'
import {IconButton} from '../../ui/components/icon-button'
import {UiTheme, useUiTheme} from '../../ui/theme'
import {useChangeEffect} from '../../hooks/use-change-effect'
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

interface ISkyConfigurator {
  updateSceneSky: (cb: MutateCallback<Sky>) => void
}

const SkyConfigurator: React.FC<ISkyConfigurator> = ({updateSceneSky}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const rowClasses = useRowStyles()

  const ctx = useSceneContext()
  const theme = useUiTheme()
  const activeSpace = useActiveSpace()
  const sky = getSkyFromSceneGraph(ctx.scene, activeSpace?.id)

  const isDefaultGradient = (oldTheme: UiTheme) => (
    sky.type === 'gradient' && sky.colors?.length === 2 &&
    sky.colors[0] === oldTheme.studioSkyboxStart && sky.colors[1] === oldTheme.studioSkyboxEnd
  )

  // Note(Dale): If the theme changes, and the skybox is the default gradient, update the colors
  useChangeEffect(([oldTheme]) => {
    if (!oldTheme) {
      return
    }
    if (isDefaultGradient(oldTheme)) {
      updateSceneSky(() => ({
        type: 'gradient',
        colors: [theme.studioSkyboxStart, theme.studioSkyboxEnd],
      }))
    }
  }, [theme])

  return (
    <div className={combine(classes.flexRow, classes.lastChildZeroMargin)}>
      <div className={rowClasses.flexItem}>
        <RowSelectField
          id='skybox-type'
          label={t('space_configurator.skybox_type.label')}
          value={sky?.type || 'gradient'}
          options={[
            {value: 'color', content: t('space_configurator.skybox_type.option.color')},
            {value: 'gradient', content: t('space_configurator.skybox_type.option.gradient')},
            {value: 'image', content: t('space_configurator.skybox_type.option.image')},
            {value: 'none', content: t('space_configurator.skybox_type.option.none')},
          ]}
          onChange={(value) => {
            updateSceneSky(() => {
              if (value === 'gradient' && sky.type === 'gradient') {
                return sky
              }
              return {
                type: value,
                ...(value === 'gradient'
                  ? {style: 'linear', colors: [theme.studioSkyboxStart, theme.studioSkyboxEnd]}
                  : {}),
              }
            })
          }}
        />
        <div className={rowClasses.indent}>
          {sky?.type === 'color' &&
            <RowColorField
              id='skybox-color'
              label={t('space_configurator.skybox_color.label')}
              onChange={(value) => {
                updateSceneSky(() => ({
                  type: 'color',
                  color: value,
                }))
              }}
              value={sky.color || DEFAULT_BOTTOM_COLOR}
            />
          }
          {sky.type === 'gradient' &&
            <>
              <div className={classes.flexRow}>
                <RowSelectField
                  id='skybox-gradient-style'
                  label={t('space_configurator.skybox_gradient_style.label')}
                  value={sky.style || 'linear'}
                  noMargin
                  options={[
                    {
                      value: 'linear',
                      content: t('space_configurator.skybox_gradient_style.option.linear'),
                    },
                    {
                      value: 'radial',
                      content: t('space_configurator.skybox_gradient_style.option.radial'),
                    },
                  ]}
                  onChange={(value) => {
                    updateSceneSky(() => ({
                      ...sky,
                      style: value,
                    }))
                  }}
                  rightContent={(
                    <FloatingPanelIconButton
                      text={t('space_configurator.button.add_color')}
                      stroke='plus'
                      onClick={() => {
                        updateSceneSky(() => ({
                          ...sky,
                          colors: [...sky.colors, '#000000'],
                        }))
                      }}
                      buttonSize='tiny'
                      disabled={sky.colors.length >= MAX_GRADIENT_COLORS}
                    />
                  )}
                />
              </div>
              {sky.colors?.map((color, index) => (
                // eslint-disable-next-line react/no-array-index-key
                <div className={classes.flexRow} key={index}>
                  <RowColorField
                    id={`skybox-gradient-color-${index}`}
                    label={
                      t('space_configurator.skybox_gradient_color.label', {index: index + 1})}
                    onChange={(value) => {
                      updateSceneSky(() => ({
                        ...sky,
                        colors: sky.colors.map(
                          (c, colorIndex) => (colorIndex === index ? value : c)
                        ),
                      }))
                    }}
                    value={color}
                    noMargin
                    rightContent={(
                      <IconButton
                        text={t('button.delete', {ns: 'common'})}
                        stroke='delete12'
                        color='muted'
                        disabled={sky.colors.length <= MIN_GRADIENT_COLORS}
                        onClick={() => {
                          updateSceneSky(() => ({
                            ...sky,
                            colors: sky.colors.filter((_, colorIndex) => colorIndex !== index),
                          }))
                        }}
                      />
                    )}
                  />
                </div>
              ))}
            </>
          }
          {sky.type === 'image' &&
            <AssetSelector
              label={t('space_configurator.skybox_image.label')}
              assetKind='image'
              resource={sky.src}
              onChange={(src) => {
                updateSceneSky(() => ({
                  type: 'image',
                  src,
                }))
              }}
            />
          }
        </div>
      </div>
    </div>
  )
}

export {
  SkyConfigurator,
}
