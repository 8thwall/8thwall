import {h, FunctionComponent} from 'preact'

import {AlertIcon} from './icons'
import {Panel} from './panel'
import sharedStyles from './shared.module.scss'
import styles from './unexpected-error-screen.module.scss'
import type {StartFlowScreenOf, StartFlowScreenOptions, StartFlowStrings} from './types'

interface UnexpectedErrorScreenProps {
  options: StartFlowScreenOptions
  screen: StartFlowScreenOf<'unexpected-error'>
  strings: StartFlowStrings
}

const UnexpectedErrorScreen: FunctionComponent<UnexpectedErrorScreenProps> = ({
  options, screen, strings,
}) => (
  <main
    className={`${sharedStyles.screen} ${sharedStyles.centered}`}
    role='alert'
  >
    <Panel>
      <div className={styles.errorIcon}><AlertIcon /></div>
      <h1 className={sharedStyles.title}>{strings.unexpectedErrorTitle}</h1>
      <p className={sharedStyles.description}>{strings.unexpectedErrorDescription}</p>
      <code className={styles.errorDetail}>{screen.error}</code>
      <button
        className={sharedStyles.primaryButton}
        type='button'
        onClick={options.onRefresh}
      >
        {strings.tryAgainCta}
      </button>
    </Panel>
  </main>
)

export {UnexpectedErrorScreen}
