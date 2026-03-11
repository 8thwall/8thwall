import React from 'react'

import type {RuntimeVersionTarget} from '@ecs/shared/runtime-version'

import {useTranslation} from 'react-i18next'

import {updateVersionTarget, type Levels} from '../runtime-version/update-version-target'
import {
  useLatestRuntimeVersion, useRuntimeVersions,
} from '../runtime-version/use-runtime-versions-query'
import {timeBetweenI18n} from '../../common/time-between'
import {timeSinceI18n} from '../../common/time-since'
import type {DropdownOption} from '../../ui/components/standard-dropdown-field'
import {useSceneContext} from '../scene-context'
import {RowSelectField, useStyles as useRowStyles} from './row-fields'
import {Badge} from '../../ui/components/badge'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {PrimaryButton} from '../../ui/components/primary-button'
import {Popup} from '../../ui/components/popup'
import {SpaceBetween} from '../../ui/layout/space-between'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {RuntimeTargetOption} from './runtime-target-option'
import {
  resolveSelectedVersion, filterResolvableVersions,
} from '../runtime-version/version-resolution'
import {
  getVersionSpecifier, getVersionSpecifierAtLevel,
} from '../runtime-version/runtime-version-patches'
import {
  compareVersionInfo, isGreaterPatchVersion,
} from '../runtime-version/compare-runtime-target'
import {ReleaseNotesModal} from '../release-notes-modal'
import {ErrorBoundary} from '../../common/error-boundary'
import {StaticBanner} from '../../ui/components/banner'
import {Loader} from '../../ui/components/loader'
import {useResolvedRuntimeVersion} from '../runtime-version/use-runtime-version'

const DEFAULT_VERSION_LEVEL = 'major'

const UPDATE_LEVELS = [
  {key: 'runtime_configurator.auto_update_level.patch', value: 'patch'},
  {key: 'runtime_configurator.auto_update_level.minor', value: 'minor'},
  {key: 'runtime_configurator.auto_update_level.major', value: 'major'},
] as const

const makeVersionString = (target: RuntimeVersionTarget) => (
  `${getVersionSpecifier(target)}`
)

const parseVersionString = (value: string): RuntimeVersionTarget => {
  const [versionSpecifier] = value.split('-')
  const version: number[] = versionSpecifier.split('.').map(s => Number(s))

  const res: RuntimeVersionTarget = {
    type: 'version',
    level: DEFAULT_VERSION_LEVEL,
    major: version[0],
    minor: version[1],
    patch: version[2],
  }

  return res
}

const RuntimeVersionConfiguratorInner: React.FC = () => {
  const rowClasses = useRowStyles()
  const menuStyles = useStudioMenuStyles()
  const ctx = useSceneContext()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])

  const versions = useRuntimeVersions()
  const selectedVersion = useResolvedRuntimeVersion()
  const target = selectedVersion.originalTarget

  const UPDATES = UPDATE_LEVELS.map(level => ({
    content: t(level.key),
    value: level.value,
  }))

  const setVersion = (versionSpecifier: string) => {
    const baseVersion = parseVersionString(versionSpecifier)

    const finalVersion = updateVersionTarget(baseVersion, target.level)

    ctx.updateScene(old => ({
      ...old,
      runtimeVersion: finalVersion,
    }))
  }

  const setVersionUpdate = (update: Levels) => {
    if (target.type !== 'version') {
      return
    }
    // The current target with new level as selected
    const newTarget = updateVersionTarget(target, update)

    // The latest version that matches the new level
    const resolvedVersion = resolveSelectedVersion(newTarget, versions)
    if (!resolvedVersion) {
      return
    }

    // Use the latest patch, with the selected level
    const finalTarget = updateVersionTarget(resolvedVersion.patchTarget, update)
    ctx.updateScene(old => ({
      ...old,
      runtimeVersion: finalTarget,
    }))
  }

  const selectedValue = makeVersionString(selectedVersion?.patchTarget || target)

  const specificity = target.level

  const filteredVersions = filterResolvableVersions(specificity, versions)

  const visibleVersions: DropdownOption[] = filteredVersions?.map((vi) => {
    const value = makeVersionString(vi.patchTarget)
    const selected = compareVersionInfo(selectedVersion, vi) === 0
    const specifier = getVersionSpecifierAtLevel(vi.patchTarget, specificity)
    const timeSinceText = timeSinceI18n(new Date(vi.publishTime), t)

    return {
      content: (
        <RuntimeTargetOption
          selected={selected}
          description={specifier}
          rightContent={t('runtime_configurator.version_option.time_since', {timeSinceText})}
        />
      ),
      value: selected ? selectedValue : value,
    }
  }) || []

  const latestVersion = useLatestRuntimeVersion()
  const latestVersionString = getVersionSpecifier(latestVersion.patchTarget)
  const canUpgrade = isGreaterPatchVersion(latestVersion.patchTarget, selectedVersion.patchTarget)
  const timeBehindText = canUpgrade
    ? timeBetweenI18n(selectedVersion.publishTime, latestVersion.publishTime, t)
    : ''

  return (
    <>
      <div className={rowClasses.flexItem}>
        <RowSelectField
          disabled={!versions}
          value={target.level}
          onChange={setVersionUpdate}
          label={(
            <Popup
              content={t('runtime_configurator.auto_update_select.tooltip')}
              position='top'
              alignment='left'
              size='tiny'
              delay={250}
            >
              {t('runtime_configurator.auto_update_select.label')}
            </Popup>
          )}
          options={UPDATES}
        />
      </div>
      <div className={rowClasses.flexItem}>
        <RowSelectField
          disabled={!versions}
          value={selectedValue}
          onChange={setVersion}
          label={t('runtime_configurator.version_select.label')}
          options={visibleVersions}
          menuWrapperClassName={menuStyles.studioMenu}
        />
      </div>
      <div className={rowClasses.row}>
        <SpaceBetween direction='horizontal' narrow grow>
          <div className={rowClasses.flexItem}>
            {canUpgrade
              ? (
                <Badge
                  height='small'
                  spacing='full'
                  variant='outlined'
                >
                  {t('runtime_configurator.version_status.time_behind', {timeBehindText})}
                </Badge>
              )
              : (
                <Badge
                  height='small'
                  spacing='full'
                  color='mint'
                  variant='outlined'
                >
                  {t('runtime_configurator.version_status.up_to_date')}
                </Badge>
              )
            }
          </div>
          <div className={rowClasses.flexItem}>
            <ReleaseNotesModal
              trigger={canUpgrade
                ? (
                  <PrimaryButton height='tiny' spacing='full'>
                    {t('release_notes_modal.button.upgrade_to_version',
                      {version: latestVersionString})}
                  </PrimaryButton>)
                : (
                  <FloatingPanelButton height='tiny' spacing='full'>
                    {t('release_notes_modal.title')}
                  </FloatingPanelButton>)
                }
            />
          </div>
        </SpaceBetween>
      </div>
    </>
  )
}

const RuntimeVersionConfigurator: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <FloatingTraySection title={t('runtime_configurator.title')}>
      <React.Suspense fallback={<Loader centered size='small' inline />}>
        <ErrorBoundary
          fallback={({onReset}) => (
            <StaticBanner
              type='danger'
              message={t('runtime_configurator.load_error')}
              onClose={onReset}
            />
          )}
        >
          <RuntimeVersionConfiguratorInner />
        </ErrorBoundary>
      </React.Suspense>
    </FloatingTraySection>
  )
}

export {
  RuntimeVersionConfigurator,
}
