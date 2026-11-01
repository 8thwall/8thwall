import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {StartFlowContentProps} from '../../types'
import styles from '../styles.module.scss'

import {LogoSection} from './logo-section'

import {UrlPromptSection} from './url-prompt-section'
import {ViewContainer} from './view-container'

const BlankView: FC<StartFlowContentProps> = ({options, strings}) => (
  <ViewContainer>
    <div className={styles.blankView}>
      <UrlPromptSection options={options} strings={strings} />
      <LogoSection options={options} strings={strings} />
    </div>
  </ViewContainer>
)

export {
  BlankView,
}
