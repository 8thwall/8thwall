import React from 'react'
import {Trans, useTranslation} from 'react-i18next'
import * as Semver from 'semver'

import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {ErrorBoundary} from '../../common/error-boundary'
import {StaticBanner} from '../../ui/components/banner'
import {Loader} from '../../ui/components/loader'
import {RowContent} from './row-content'
import {
  useLatestRuntimeVersion,
  useRuntimeVersions,
} from '../runtime-version/use-runtime-versions-query'
import {timeSinceI18n} from '../../common/time-since'
import type {DropdownOption} from '../../ui/components/standard-dropdown-field'
import {RowSelectField, useStyles as useRowStyles} from './row-fields'
import {PrimaryButton} from '../../ui/components/primary-button'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {RuntimeTargetOption} from './runtime-target-option'
import {useRuntimeMetadata} from '../runtime-version/use-runtime-metadata'
import {useEnclosedAppKey} from '../../apps/enclosed-app-context'
import {installPackages, migrateProject} from '../local-sync-api'
import {useLocalSyncContext} from '../local-sync-context'
import {BoldButton} from '../../ui/components/bold-button'
import {StandardModalActions} from '../../ui/components/standard-modal-actions'
import {StandardModalContent} from '../../ui/components/standard-modal-content'
import {StandardModal} from '../../ui/components/standard-modal'
import {StandardModalHeader} from '../../editor/standard-modal-header'
import AutoHeading from '../../widgets/auto-heading'
import AutoHeadingScope from '../../widgets/auto-heading-scope'
import {StandardLink} from '../../ui/components/standard-link'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {usePlaybackContext} from '../playback-context'

const RuntimeVersionConfiguratorInner: React.FC = () => {
  const rowClasses = useRowStyles()
  const menuStyles = useStudioMenuStyles()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const appKey = useEnclosedAppKey()

  const versions = useRuntimeVersions()
  const currentVersion = useRuntimeMetadata().version
  const [updating, setUpdating] = React.useState(false)
  const localSync = useLocalSyncContext()
  const latestVersion = useLatestRuntimeVersion()

  const setVersion = async (versionSpecifier: string) => {
    if (updating) {
      return
    }
    try {
      setUpdating(true)
      await installPackages(appKey, [{name: '@8thwall/ecs', version: versionSpecifier}])
      await localSync.restartServer()
    } finally {
      setUpdating(false)
    }
  }

  const visibleVersions: DropdownOption[] = versions
    .filter(v => BuildIf.ALL_QA || v.version === currentVersion || !Semver.prerelease(v.version))
    .map((vi) => {
      const value = vi.version
      const selected = vi.version === currentVersion
      const timeSinceText = timeSinceI18n(new Date(vi.publishTime), t)

      return {
        content: (
          <RuntimeTargetOption
            selected={selected}
            description={vi.version}
            rightContent={t('runtime_configurator.version_option.time_since', {timeSinceText})}
          />
        ),
        value,
      }
    })

  const canUpgrade = Semver.gt(currentVersion, latestVersion.version)

  return (
    <>
      <div className={rowClasses.flexItem}>
        <RowSelectField
          disabled={!versions || updating}
          value={currentVersion}
          onChange={setVersion}
          label={(
            <>
              {t('runtime_configurator.version_select.label')}{' '}
              {updating && <Loader inline size='tiny' />}
            </>
          )}
          options={visibleVersions}
          menuWrapperClassName={menuStyles.studioMenu}
        />
      </div>
      {canUpgrade &&
        <div className={rowClasses.row}>
          <PrimaryButton
            height='tiny'
            spacing='full'
            disabled={updating}
            onClick={() => setVersion(latestVersion.version)}
          >
            {t('release_notes_modal.button.upgrade_to_version',
              {version: latestVersion.version})}
          </PrimaryButton>
        </div>
      }
    </>
  )
}

const NeedsMigrationView: React.FC = () => {
  const rowClasses = useRowStyles()
  const localSync = useLocalSyncContext()
  const appKey = useEnclosedAppKey()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])

  const [updating, setUpdating] = React.useState(false)
  const [didError, setDidError] = React.useState(false)

  return (
    <>
      <div className={rowClasses.row}>
        <p>
          {t('runtime_version_configurator.warning.needs_config_update')}
        </p>
      </div>
      <div className={rowClasses.row}>
        <StandardModal
          width='narrow'
          trigger={(
            <FloatingPanelButton>
              {t('button.update', {ns: 'common'})}
            </FloatingPanelButton>
          )}
        >
          {collapse => (
            <AutoHeadingScope level={2}>
              <StandardModalHeader>
                <AutoHeading>{t('runtime_version_configurator.update_modal.title')}</AutoHeading>
              </StandardModalHeader>
              <StandardModalContent>
                <p>
                  {t('runtime_version_configurator.update_modal.body_1')}
                </p>
                <p>
                  <Trans
                    ns='cloud-studio-pages'
                    i18nKey='runtime_version_configurator.update_modal.body_2'
                    components={{
                      changeLink: <StandardLink
                        newTab
                        href='https://8th.io/studio-config-migration'
                      />,
                    }}
                  />
                </p>
                {didError &&
                  <StaticBanner
                    type='danger'
                    message={t('runtime_version_configurator.error.migration_failed')}
                  />
                }
              </StandardModalContent>
              <StandardModalActions>
                <BoldButton onClick={collapse}>
                  {t('button.cancel', {ns: 'common'})}
                </BoldButton>
                <PrimaryButton
                  onClick={async () => {
                    if (updating) {
                      return
                    }
                    try {
                      setDidError(false)
                      setUpdating(true)
                      await migrateProject(appKey)
                      await localSync.restartServer()
                      collapse()
                    } catch (err) {
                      setDidError(true)
                    } finally {
                      setUpdating(false)
                    }
                  }}
                  loading={updating}
                >
                  {t('runtime_version_configurator.update_modal.submit')}
                </PrimaryButton>
              </StandardModalActions>
            </AutoHeadingScope>
          )}
        </StandardModal>
      </div>
    </>
  )
}

const RuntimeVersionConfiguratorMigrationCheck: React.FC = () => {
  const {simulatorEnabled} = usePlaybackContext()
  if (!simulatorEnabled) {
    return <NeedsMigrationView />
  }
  return <RuntimeVersionConfiguratorInner />
}

const RuntimeVersionConfigurator: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <FloatingTraySection title={t('runtime_configurator.title')}>
      <React.Suspense fallback={<Loader centered size='small' inline />}>
        <ErrorBoundary
          fallback={({onReset}) => (
            <RowContent>
              <StaticBanner
                type='danger'
                message={t('runtime_configurator.load_error')}
                onClose={onReset}
              />
            </RowContent>
          )}
        >
          <RuntimeVersionConfiguratorMigrationCheck />
        </ErrorBoundary>
      </React.Suspense>
    </FloatingTraySection>
  )
}

export {
  RuntimeVersionConfigurator,
}
