import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {LandingParameters} from '../parameters'

import {LogoSection} from './logo-section'

import {UrlPromptSection} from './url-prompt-section'
import {ViewContainer} from './view-container'

const BlankView: FC<LandingParameters> = props => (
  <ViewContainer
    backgroundColor={props.backgroundColor}
    backgroundSrc={props.backgroundSrc}
    backgroundBlur={props.backgroundBlur}
  >
    <div
      className={`landing8-blank-view ${props.textShadow ? 'landing8-text-shadow' : ''}`}
      style={{color: props.textColor, fontFamily: props.font}}
    >
      <UrlPromptSection
        url={props.url}
        promptPrefix={props.promptPrefix}
        promptSuffix={props.promptSuffix}
        vrPromptPrefix={props.vrPromptPrefix}
      />
      <LogoSection />
    </div>
  </ViewContainer>
)

export {
  BlankView,
}
