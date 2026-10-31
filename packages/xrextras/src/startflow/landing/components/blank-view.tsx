import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {LandingParameters} from '../parameters'
import styles from '../styles.module.scss'

import {LogoSection} from './logo-section'

import {UrlPromptSection} from './url-prompt-section'
import {ViewContainer} from './view-container'

const BlankView: FC<LandingParameters> = props => (
  <ViewContainer>
    <div className={styles.blankView}>
      <UrlPromptSection
        url={props.url}
        promptPrefix={props.promptPrefix}
        promptSuffix={props.promptSuffix}
        vrPromptPrefix={props.vrPromptPrefix}
        qrAlt={props.qrAlt}
        messengerLabel={props.messengerLabel}
      />
      <LogoSection {...props} />
    </div>
  </ViewContainer>
)

export {
  BlankView,
}
