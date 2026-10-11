import {h, FunctionComponent} from 'preact'

import {Panel} from './panel'
import sharedStyles from './shared.module.scss'
import type {StartFlowStrings} from './types'
import {AlertIcon} from './icons'

interface UnsupportedDeviceScreenProps {
  strings: StartFlowStrings
}

const UnsupportedDeviceScreen: FunctionComponent<UnsupportedDeviceScreenProps> = ({strings}) => (
  <main
    className={`${sharedStyles.screen} ${sharedStyles.centered}`}
  >
    <Panel className={sharedStyles.statusContent}>
      <div className={`${sharedStyles.iconBadge} ${sharedStyles.errorIconBadge}`}>
        <AlertIcon />
      </div>
      <h1 className={sharedStyles.title}>{strings.unsupportedDeviceTitle}</h1>
      <p className={sharedStyles.instruction}>{strings.unsupportedDeviceDescription}</p>
    </Panel>
  </main>
)

export {UnsupportedDeviceScreen}
