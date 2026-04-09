/* eslint-disable react/react-in-jsx-scope */
import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'
import {ViewContainer} from './view-container'
import {LogoSection} from './logo-section'
import {UrlPromptSection} from './url-prompt-section'
import {ModelScene} from './model-scene'
import {loadThreeJs} from '../3d/three'

interface IModelView extends LandingParameters {
  onError: () => void
}

const ModelView: FC<IModelView> = (props) => {
  const [isReady, setIsReady] = useState(false)

  useEffect(() => {
    let mounted = true
    loadThreeJs().then(() => {
      if (mounted) {
        setIsReady(true)
      }
    }).catch((err) => {
      // eslint-disable-next-line no-console
      console.error('Error loading model view', err)
      if (mounted) {
        props.onError()
      }
    })

    return () => {
      mounted = false
    }
  }, [])

  return (
    <ViewContainer
      backgroundColor={props.backgroundColor}
      backgroundSrc={props.backgroundSrc}
      backgroundBlur={props.backgroundBlur}
    >
      <div
        className={`landing8-model-viewer-info ${props.textShadow ? 'landing8-text-shadow' : ''}`}
        style={{color: props.textColor, fontFamily: props.font}}
      >
        <UrlPromptSection
          url={props.url}
          promptPrefix={props.promptPrefix}
          promptSuffix={props.promptSuffix}
          vrPromptPrefix={props.vrPromptPrefix}
        />
        <LogoSection logoSrc={props.logoSrc} logoAlt={props.logoAlt} />
      </div>
      {isReady && <ModelScene {...props} />}
    </ViewContainer>
  )
}

export {
  ModelView,
}
