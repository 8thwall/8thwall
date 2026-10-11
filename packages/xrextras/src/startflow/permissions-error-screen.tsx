import {h, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import {AlertIcon} from './icons'
import {Panel} from './panel'
import {PrimaryButton} from './primary-button'
import styles from './permissions-error-screen.module.scss'
import sharedStyles from './shared.module.scss'
import type {
  Permission,
  StartFlowScreenOf,
  StartFlowStrings,
  StartFlowScreenOptions,
} from './types'

interface PermissionsErrorScreenProps {
  options: StartFlowScreenOptions
  screen: StartFlowScreenOf<'permissions-error'>
  strings: StartFlowStrings
}

const permissionLabel = (permission: Permission, strings: StartFlowStrings) => {
  switch (permission) {
    case 'camera':
      return strings.cameraLabel
    case 'microphone':
      return strings.microphoneLabel
    default:
      return strings.motionLabel
  }
}

interface PermissionInstruction {
  message: string
  permissions: Permission[]
}

const instructionsFor = (
  screen: StartFlowScreenOf<'permissions-error'>, strings: StartFlowStrings
): PermissionInstruction[] => {
  if (!screen.failed.length) {
    return []
  }
  switch (screen.environment) {
    case 'ios-safari': {
      const instructions: PermissionInstruction[] = []
      const websitePermissions = screen.failed.filter(permission => permission !== 'motion')
      if (websitePermissions.length) {
        instructions.push({
          message: strings.permissionsErrorIosInstructions,
          permissions: websitePermissions,
        })
      }
      if (screen.failed.includes('motion')) {
        instructions.push({
          message: strings.permissionsErrorIosMotionInstructions,
          permissions: ['motion'],
        })
      }
      return instructions
    }
    case 'ios-chrome':
      return [{
        message: strings.permissionsErrorIosChromeInstructions,
        permissions: screen.failed,
      }]
    case 'android-chrome':
      return [{
        message: strings.permissionsErrorAndroidChromeInstructions,
        permissions: screen.failed,
      }]
    case 'desktop-safari':
      return [{
        message: strings.permissionsErrorSafariInstructions,
        permissions: screen.failed,
      }]
    case 'desktop-chrome':
      return [{
        message: strings.permissionsErrorChromeInstructions,
        permissions: screen.failed,
      }]
    default:
      return [{
        message: strings.permissionsErrorOtherInstructions,
        permissions: screen.failed,
      }]
  }
}

const PermissionsErrorScreen: FunctionComponent<PermissionsErrorScreenProps> = ({
  options, screen, strings,
}) => {
  const refresh = options.onRefresh || (() => window.location.reload())
  const failed = new Set(screen.failed)
  const instructions = instructionsFor(screen, strings)

  return (
    <main
      className={combine(sharedStyles.screen, sharedStyles.centered)}
      role='alert'
    >
      <Panel className={sharedStyles.statusContent}>
        <div className={combine(sharedStyles.iconBadge, sharedStyles.errorIconBadge)}>
          <AlertIcon />
        </div>
        <h1 className={sharedStyles.title}>{strings.permissionsTitle}</h1>
        <ul className={styles.permissionList}>
          {screen.requested.map(permission => (
            <li key={permission} className={styles.permissionRow}>
              <span>{permissionLabel(permission, strings)}</span>
              <span className={failed.has(permission) ? styles.failed : styles.allowed}>
                {failed.has(permission)
                  ? strings.permissionFailedLabel
                  : strings.permissionAllowedLabel}
              </span>
            </li>
          ))}
        </ul>
        {instructions.length > 0 && (
          <div className={combine(sharedStyles.instruction, styles.instructions)}>
            {instructions.map(({message, permissions}) => (
              <div className={styles.instructionGroup} key={message}>
                <p className={styles.instructionText}>{message}</p>
                <ul className={styles.instructionPermissionList}>
                  {permissions.map(permission => (
                    <li key={permission}>{permissionLabel(permission, strings)}</li>
                  ))}
                </ul>
              </div>
            ))}
          </div>
        )}
        <PrimaryButton onClick={refresh}>
          {strings.tryAgainCta}
        </PrimaryButton>
      </Panel>
    </main>
  )
}

export {PermissionsErrorScreen}
