import {Fragment, h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import {combine} from '../classnames'
import {useRecordButton} from './record-button-controller'
import type {CaptureMode} from './record-button-controller'
import styles from './record-button.module.scss'
import type {MediaRecorderStrings} from './strings'

interface RecordButtonViewProps {
  captureMode: CaptureMode
  strings: MediaRecorderStrings
}

const RecordButtonView: FC<RecordButtonViewProps> = ({captureMode, strings}) => {
  const controller = useRecordButton(captureMode)

  return (
    <Fragment>
      <div
        className={combine(
          styles.recorderContainer,
          controller.active && styles.active,
          controller.previewOpen && styles.fadeContainer,
          controller.fixedMode && styles.fixedMode,
          controller.loading && styles.loading,
          controller.recording && styles.recording
        )}
      >
        <svg
          viewBox='0 0 38 38'
          className={styles.progressContainer}
          aria-hidden='true'
          focusable='false'
        >
          <circle className={styles.progressTrack} r='16' cx='19' cy='19' />
          <circle
            className={styles.progressBar}
            r='16'
            cx='19'
            cy='19'
            style={{strokeDashoffset: controller.progress}}
          />
          <circle className={styles.loadingCircle} r='16' cx='19' cy='19' />
        </svg>

        <button
          className={styles.recorderButton}
          type='button'
          onMouseDown={controller.down}
          onTouchStart={controller.down}
        >
          {strings.recordCta}
        </button>
      </div>

      <div
        className={combine(styles.flashElement, controller.flashing && styles.flashing)}
      />
    </Fragment>
  )
}

export {RecordButtonView}
