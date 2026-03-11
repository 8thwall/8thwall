import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {EntrySpaceDropdown} from './entry-space-dropdown'
import type {RIGHT_PANEL_PAGES} from './default-configurator'
import {StudioSceneGraphView} from '../studio-scenegraph-view'

const useStyles = createUseStyles({
  centerContainer: {
    display: 'flex',
    alignContent: 'center',
    justifyContent: 'center',
    gap: '1rem',
  },
})

interface ProjectSettingsConfiguratorProps {
  setPage: React.Dispatch<React.SetStateAction<RIGHT_PANEL_PAGES>>
}

const ProjectSettingsConfigurator: React.FC<ProjectSettingsConfiguratorProps> = ({setPage}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  return (
    <FloatingTraySection title={t('project_settings_configurator.title')}>
      <EntrySpaceDropdown />
      <div className={classes.centerContainer}>
        <FloatingPanelButton
          spacing='normal'
          onClick={() => setPage('inputManager')}
        >
          {t('input_manager_configurator.title')}
        </FloatingPanelButton>
        {BuildIf.SCENE_JSON_VISIBLE_20240616 && <StudioSceneGraphView />}
      </div>
    </FloatingTraySection>
  )
}

export {
  ProjectSettingsConfigurator,
}
