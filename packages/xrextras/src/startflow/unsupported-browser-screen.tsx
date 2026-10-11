import {h, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import {OpenBrowserIcon} from './icons'
import {Panel} from './panel'
import sharedStyles from './shared.module.scss'
import type {StartFlowStrings} from './types'

interface UnsupportedBrowserScreenProps {
  strings: StartFlowStrings
}

const UnsupportedBrowserScreen: FunctionComponent<UnsupportedBrowserScreenProps> = ({
  strings,
}) => (
  <main className={combine(sharedStyles.screen, sharedStyles.centered)}>
    <Panel className={sharedStyles.statusContent}>
      <div className={sharedStyles.iconBadge}><OpenBrowserIcon /></div>
      <h1 className={sharedStyles.title}>{strings.unsupportedBrowserTitle}</h1>
      <p className={sharedStyles.instruction}>
        {strings.unsupportedBrowserInstructions}
      </p>
    </Panel>
  </main>
)

export {UnsupportedBrowserScreen}
