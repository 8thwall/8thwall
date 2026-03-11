import React from 'react'
import {useTranslation} from 'react-i18next'

import {InputManagerConfigurator} from './input-manager-configurator'
import {SpaceConfigurator} from './space-configurator'
import {SubMenuHeading} from '../ui/submenu-heading'
import {ProjectSettingsConfigurator} from './project-settings-configurator'
import {RuntimeVersionConfigurator} from './runtime-version-configurator'
import {ProjectManifestConfigurator} from './project-manifest-configurator'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {StudioHotkeysModal} from '../ui/studio-hotkeys-modal'
import {SpaceBetween} from '../../ui/layout/space-between'

interface IDefaultConfigurator {
}

type RIGHT_PANEL_PAGES = 'home' | 'inputManager'

const DefaultConfigurator: React.FC<IDefaultConfigurator> = (
) => {
  const [page, setPage] = React.useState<RIGHT_PANEL_PAGES>('home')
  const {t} = useTranslation(['cloud-studio-pages', 'app-pages'])
  const hasRepo = !!useCurrentRepoId()

  return (
    <div>
      {page === 'home' &&
        <>
          <SpaceConfigurator />
          <ProjectSettingsConfigurator
            setPage={setPage}
          />
          {BuildIf.STUDIO_RUNTIME_CONFIG_20260209 && hasRepo &&
            <RuntimeVersionConfigurator />
          }
          {hasRepo && BuildIf.MANIFEST_EDIT_20250618 && <ProjectManifestConfigurator />}
          <br />
          <SpaceBetween justifyCenter>
            <StudioHotkeysModal
              trigger={(
                <FloatingPanelButton a8='click;studio;show-keyboard-shortcuts-button'>
                  {t(
                    'project_settings_page.dev_preference_settings_view.button.keyboard_shortcuts',
                    {ns: 'app-pages'}
                  )}
                </FloatingPanelButton>
              )}
            />
          </SpaceBetween>
        </>}

      {page === 'inputManager' &&
        <>
          <SubMenuHeading
            title={t('input_manager_configurator.title')}
            onBackClick={() => { setPage('home') }}
          />
          <InputManagerConfigurator />
        </>
      }
    </div>
  )
}

export {
  DefaultConfigurator,
  RIGHT_PANEL_PAGES,
}
