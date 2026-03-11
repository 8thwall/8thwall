import React from 'react'

import type {Sky} from '@ecs/shared/scene-graph'

import {useTranslation} from 'react-i18next'

import {getSkyFromSceneGraph} from '@ecs/shared/get-sky-from-scene-graph'

import {MutateCallback, useSceneContext} from '../scene-context'
import {CornerDropdownMenu} from '../ui/corner-dropdown-menu'
import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {useUiTheme} from '../../ui/theme'
import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {useActiveSpace} from '../hooks/active-space'
import {SkyConfigurator} from './sky-configurator'
import {IncludedSpacesMenu} from './included-spaces-menu'
import {ActiveCameraMenu} from './active-camera-menu'
import {SceneReflectionsMenu} from './scene-reflections-menu'
import {FogConfigurator} from './fog-configurator'

const SpaceConfigurator: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])

  const ctx = useSceneContext()
  const theme = useUiTheme()
  const activeSpace = useActiveSpace()
  const sky = getSkyFromSceneGraph(ctx.scene, activeSpace?.id)
  const spaceName = activeSpace?.name

  const spaces = ctx.scene.spaces ? Object.values(ctx.scene.spaces) : []

  const updateSceneSky = (cb: MutateCallback<Sky>) => {
    ctx.updateScene((scene) => {
      const updatedSky = cb(sky)
      if (activeSpace) {
        return {
          ...scene,
          spaces: {
            ...scene.spaces,
            [activeSpace.id]: {
              ...scene.spaces[activeSpace.id],
              sky: updatedSky,
            },
          },
        }
      }
      return {
        ...scene,
        sky: updatedSky,
      }
    })
  }

  const topContent = (
    <CornerDropdownMenu>
      {collapse => (
        <FloatingMenuButton
          onClick={() => {
            updateSceneSky(() => ({
              type: 'gradient',
              colors: [theme.studioSkyboxStart, theme.studioSkyboxEnd],
            }))
            collapse()
          }}
        >
          {t('space_configurator.button.reset_to_default')}
        </FloatingMenuButton>
      )}
    </CornerDropdownMenu>
  )

  return (
    <FloatingTraySection
      title={spaceName
        ? t('space_configurator.title.with_name', {spaceName})
        : t('space_configurator.title.no_name')}
      topContent={topContent}
    >
      <ActiveCameraMenu />
      <SkyConfigurator updateSceneSky={updateSceneSky} />
      {spaces.length > 1 && <IncludedSpacesMenu />}
      <SceneReflectionsMenu />
      <FogConfigurator />
    </FloatingTraySection>
  )
}

export {
  SpaceConfigurator,
}
