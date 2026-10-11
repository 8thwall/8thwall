import {h, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import {AlertIcon} from './icons'
import {Panel} from './panel'
import {PrimaryButton} from './primary-button'
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
    className={combine(sharedStyles.screen, sharedStyles.centered)}
    role='alert'
  >
    <Panel>
      <div className={combine(sharedStyles.iconBadge, sharedStyles.errorIconBadge)}>
        <AlertIcon />
      </div>
      <h1 className={sharedStyles.title}>{strings.unexpectedErrorTitle}</h1>
      <p className={sharedStyles.description}>{strings.unexpectedErrorDescription}</p>
      <code className={styles.errorDetail}>{screen.error}</code>
      <PrimaryButton onClick={options.onRefresh}>
        {strings.tryAgainCta}
      </PrimaryButton>
    </Panel>
  </main>
)

export {UnexpectedErrorScreen}
