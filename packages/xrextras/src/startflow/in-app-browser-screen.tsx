import {h, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import {OpenBrowserIcon} from './icons'
import {Panel} from './panel'
import sharedStyles from './shared.module.scss'
import type {StartFlowStrings} from './types'

interface InAppBrowserScreenProps {
  strings: StartFlowStrings
}

const InAppBrowserScreen: FunctionComponent<InAppBrowserScreenProps> = ({strings}) => (
  <main className={combine(sharedStyles.screen, sharedStyles.centered)}>
    <Panel className={sharedStyles.statusContent}>
      <div className={sharedStyles.iconBadge}><OpenBrowserIcon /></div>
      <h1 className={sharedStyles.title}>{strings.inAppBrowserTitle}</h1>
      <p className={sharedStyles.instruction}>{strings.inAppBrowserInstructions}</p>
    </Panel>
  </main>
)

export {InAppBrowserScreen}
