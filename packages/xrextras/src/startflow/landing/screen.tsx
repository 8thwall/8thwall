import {Fragment, h} from 'preact'
import type {FunctionComponent} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import {Panel} from '../panel'
import sharedStyles from '../shared.module.scss'
import type {
  StartFlowScreenOptions,
  StartFlowStrings,
} from '../types'
import {LandingLogo} from './components/logo'
import {LandingMedia} from './components/media'
import {LandingModel} from './components/model'
import {LandingPrompt} from './components/prompt'
import styles from './styles.module.scss'

interface Props {
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}

const LandingScreen: FunctionComponent<Props> = ({options, strings}) => {
  const [failed, setFailed] = useState(false)
  const [dimensions, setDimensions] = useState<{width: number, height: number}>()
  useEffect(() => setFailed(false), [options.mediaSrc])
  const {mediaSrc, url} = options
  const isModel = !!mediaSrc && /\.(glb|gltf)(?:[?#]|$)/i.test(mediaSrc)
  const info = (
    <Fragment>
      {url && <LandingPrompt url={url} strings={strings} />}
      <LandingLogo options={options} strings={strings} />
    </Fragment>
  )
  const mediaStyle = dimensions
    ? {
      '--xrextras-landing-media-width': dimensions.width,
      '--xrextras-landing-media-height': dimensions.height,
    }
    : undefined
  let content
  if (isModel && !failed) {
    content = (
      <Fragment>
        <LandingModel
          options={options}
          onError={() => setFailed(true)}
        />
        <div className={styles.modelInfo}>{info}</div>
      </Fragment>
    )
  } else {
    let panelContent
    if (mediaSrc && !failed) {
      const orientation = dimensions && dimensions.width < dimensions.height
        ? styles.portrait
        : styles.landscape
      panelContent = (
        <Panel
          animate={false}
          className={`${sharedStyles.expandedPanel} ${styles.mediaView} ${
            dimensions ? '' : styles.loading
          } ${orientation}`}
          style={mediaStyle}
        >
          <LandingMedia
            options={options}
            strings={strings}
            onSize={setDimensions}
            onError={() => setFailed(true)}
          />
          {info}
        </Panel>
      )
    } else {
      panelContent = (
        <Panel className={`${sharedStyles.expandedPanel} ${styles.blankView}`}>
          {info}
        </Panel>
      )
    }
    content = <div className={styles.centered}>{panelContent}</div>
  }
  return (
    <main className={`${sharedStyles.screen} ${styles.screen}`} data-screen='landing'>
      {content}
    </main>
  )
}

export {LandingScreen}
