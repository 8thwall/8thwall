import React from 'react'
import {useTranslation} from 'react-i18next'

import {useSceneContext} from './scene-context'
import {createThemedStyles} from '../ui/theme'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {LinkButton} from '../ui/components/link-button'
import {PrimaryButton} from '../ui/components/primary-button'
import {Icon} from '../ui/components/icon'
import {combine} from '../common/styles'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import AutoHeading from '../widgets/auto-heading'
import {ReleaseNotes} from './release-notes'
import {useResolvedRuntimeVersion} from './runtime-version/use-runtime-version'
import {getVersionSpecifier} from './runtime-version/runtime-version-patches'
import {updateVersionTarget} from './runtime-version/update-version-target'
import {isGreaterPatchVersion} from './runtime-version/compare-runtime-target'
import {useLatestRuntimeVersion} from './runtime-version/use-runtime-versions-query'

const useStyles = createThemedStyles(theme => ({
  container: {
    background: theme.modalBg,
    color: theme.modalFg,
    width: '80vw',
    maxHeight: '90vh',
    whiteSpace: 'pre-wrap',
    display: 'flex',
    flexDirection: 'column',
    padding: '2em',
    borderRadius: '15px',
  },
  bottomRow: {
    display: 'flex',
    flexDirection: 'row',
    marginTop: '1em',
    textAlign: 'center',
    alignItems: 'center',
  },
  projectVersionInfo: {
    display: 'flex',
    flexDirection: 'row',
    flex: '1 1 0',
  },
  projectVersionStatus: {
    marginLeft: '1em',
    fontStyle: 'italic',
    color: theme.fgSuccess,
  },
  projectVersionStatusNotUpToDate: {
    color: theme.fgError,
  },
  buttons: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'right',
    gap: '1em',
  },
}))

interface IReleaseNotesModal {
  trigger: React.ReactNode
  startOpen?: boolean
  onOpenChange?: (open: boolean) => void
}

const ReleaseNotesModal: React.FC<IReleaseNotesModal> = ({
  trigger, startOpen, onOpenChange,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const ctx = useSceneContext()

  const selectedVersion = useResolvedRuntimeVersion()
  const latestVersion = useLatestRuntimeVersion()
  const canUpgrade = isGreaterPatchVersion(latestVersion.patchTarget, selectedVersion.patchTarget)

  const selectedVersionString = getVersionSpecifier(selectedVersion.patchTarget)
  const latestVersionString = getVersionSpecifier(latestVersion.patchTarget)

  const upgradeToLatestRuntimeVersion = () => {
    const finalVersion = updateVersionTarget(
      latestVersion.patchTarget,
      selectedVersion.originalTarget.level
    )

    ctx.updateScene(s => ({...s, runtimeVersion: finalVersion}))
  }

  return (
    <FloatingTrayModal trigger={trigger} startOpen={startOpen} onOpenChange={onOpenChange}>
      {collapse => (
        <div className={classes.container}>
          <AutoHeadingScope level={2}>
            <AutoHeading>
              <Icon stroke='lightning' />&nbsp;
              {t('release_notes_modal.title')}
            </AutoHeading>
            <ReleaseNotes />
            <div className={classes.bottomRow}>
              <div className={classes.projectVersionInfo}>
                {t('release_notes_modal.project_version')}: {selectedVersionString}
                <span
                  className={combine(
                    classes.projectVersionStatus,
                    canUpgrade && classes.projectVersionStatusNotUpToDate
                  )}
                >
                  {canUpgrade
                    ? t('release_notes_modal.project_version.not_up_to_date')
                    : t('release_notes_modal.project_version.up_to_date')}
                </span>
              </div>
              <div className={classes.buttons}>
                <LinkButton
                  onClick={() => {
                    onOpenChange?.(false)
                    collapse()
                  }}
                >
                  {canUpgrade
                    ? t('release_notes_modal.button.not_now')
                    : t('release_notes_modal.button.close')}
                </LinkButton>
                {canUpgrade && (
                  <PrimaryButton
                    onClick={() => {
                      upgradeToLatestRuntimeVersion()
                      onOpenChange?.(false)
                      collapse()
                    }}
                  >
                    <Icon stroke='doubleChevronUp' inline />
                    {t('release_notes_modal.button.upgrade_to_version',
                      {version: latestVersionString})}
                  </PrimaryButton>
                )}
              </div>
            </div>
          </AutoHeadingScope>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {ReleaseNotesModal}
