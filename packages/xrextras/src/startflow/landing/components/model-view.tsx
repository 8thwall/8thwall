/* eslint-disable react/react-in-jsx-scope */
import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'
import {ViewContainer} from './view-container'
import {LogoSection} from './logo-section'
import {UrlPromptSection} from './url-prompt-section'
import {ModelScene} from './model-scene'
import {loadThreeJs} from '../3d/three'
import styles from '../styles.module.scss'

interface IModelView extends LandingParameters {
  onError: () => void
}

const ModelView: FC<IModelView> = (props) => {
  const [isReady, setIsReady] = useState(false)

  useEffect(() => {
    let mounted = true
    const load = async () => {
      try {
        await loadThreeJs()
        if (mounted) {
          setIsReady(true)
        }
      } catch (err) {
        // eslint-disable-next-line no-console
        console.error('Error loading model view', err)
        if (mounted) {
          props.onError()
        }
      }
    }
    load()

    return () => {
      mounted = false
    }
  }, [])

  return (
    <ViewContainer centered={false}>
      <div className={styles.modelViewerInfo}>
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
      {isReady && <ModelScene {...props} />}
    </ViewContainer>
  )
}

export {
  ModelView,
}
