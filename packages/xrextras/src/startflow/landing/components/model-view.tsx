/* eslint-disable react/react-in-jsx-scope */
import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useEffect} from 'preact/hooks'

import type {StartFlowContentProps} from '../../types'
import {ViewContainer} from './view-container'
import {LogoSection} from './logo-section'
import {UrlPromptSection} from './url-prompt-section'
import {ModelScene} from './model-scene'
import {loadThreeJs} from '../3d/three'
import styles from '../styles.module.scss'

interface IModelView extends StartFlowContentProps {
  onError: () => void
}

const ModelView: FC<IModelView> = ({options, strings, onError}) => {
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
          onError()
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
        <UrlPromptSection options={options} strings={strings} />
        <LogoSection options={options} strings={strings} />
      </div>
      {isReady && <ModelScene options={options} onError={onError} />}
    </ViewContainer>
  )
}

export {
  ModelView,
}
