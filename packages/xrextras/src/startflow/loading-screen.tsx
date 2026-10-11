import {h, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import {Panel} from './panel'
import styles from './loading-screen.module.scss'
import sharedStyles from './shared.module.scss'
import type {StartFlowStrings} from './types'

interface LoadingScreenProps {
  requestingPermissions?: boolean
  strings: StartFlowStrings
}

const LoadingScreen: FunctionComponent<LoadingScreenProps> = ({
  requestingPermissions = false, strings,
}) => (
  <main
    className={combine(sharedStyles.screen, requestingPermissions
      ? styles.requesting
      : sharedStyles.centered)}
    role='status'
    aria-label={requestingPermissions ? undefined : strings.loadingLabel}
    aria-live={requestingPermissions ? 'polite' : undefined}
  >
    <div className={styles.spinner} aria-hidden='true' />
    {requestingPermissions && (
      <Panel className={combine(sharedStyles.autoHeightPanel, styles.requestingPanel)}>
        <h1 className={sharedStyles.title}>{strings.permissionsTitle}</h1>
        <p className={sharedStyles.description}>{strings.permissionsRequestingDescription}</p>
      </Panel>
    )}
  </main>
)

export {LoadingScreen}
