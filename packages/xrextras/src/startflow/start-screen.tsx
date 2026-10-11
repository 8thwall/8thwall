import {h, FunctionComponent} from 'preact'

import {Panel} from './panel'
import sharedStyles from './shared.module.scss'
import styles from './start-screen.module.scss'
import type {StartFlowScreenOptions, StartFlowStrings} from './types'

interface StartScreenProps {
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}

const StartScreen: FunctionComponent<StartScreenProps> = ({options, strings}) => (
  <main
    className={`${sharedStyles.screen} ${sharedStyles.centered}`}
  >
    <Panel
      className={`${styles.card} ${sharedStyles.autoHeightPanel}`}
      ariaLabel={strings.startTitle}
    >
      {options.previewSrc && (
        <div className={styles.previewWrap}>
          <img
            className={styles.preview}
            src={options.previewSrc}
            alt={strings.previewAlt}
          />
        </div>
      )}
      <div className={styles.content}>
        <h1 className={sharedStyles.title}>{strings.startTitle}</h1>
        {strings.startDescription && (
          <p className={sharedStyles.description}>{strings.startDescription}</p>
        )}
        <button
          className={styles.button}
          type='button'
          onClick={options.onStart}
        >
          {strings.startCta}
        </button>
      </div>
    </Panel>
  </main>
)

export {StartScreen}
