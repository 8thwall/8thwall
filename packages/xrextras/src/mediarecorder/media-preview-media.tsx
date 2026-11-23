import {Fragment, h} from 'preact'
import type {FunctionComponent as FC, RefObject} from 'preact'

import {combine} from '../classnames'
import CLOSE_SRC from './close.svg'
import styles from './media-preview.module.scss'
import type {MediaPreviewState} from './media-preview-types'
import SOUND_OFF_SRC from './sound-off.svg'
import SOUND_ON_SRC from './sound-on.svg'
import type {MediaRecorderStrings} from './strings'

interface MediaPreviewMediaProps {
  onClose: () => void
  onToggleMuted: () => void
  state: MediaPreviewState
  strings: MediaRecorderStrings
  videoRef: RefObject<HTMLVideoElement>
}

const MediaPreviewMedia: FC<MediaPreviewMediaProps> = ({
  onClose,
  onToggleMuted,
  state,
  strings,
  videoRef,
}) => (
  <div className={styles.topBar}>
    <div className={styles.previewBox}>
      <button
        className={combine(styles.buttonReset, styles.iconButton, styles.closePreviewButton)}
        type='button'
        aria-label={strings.closePreviewLabel}
        onClick={onClose}
      >
        <img alt='' src={CLOSE_SRC} />
      </button>
      {state.previewType === 'image' &&
        <img
          className={styles.imagePreview}
          alt={strings.capturedPreviewAlt}
          src={state.imageSrc}
        />
      }
      {state.previewType === 'video' &&
        <Fragment>
          {/* eslint-disable-next-line jsx-a11y/media-has-caption */}
          <video
            ref={videoRef}
            className={styles.videoPreview}
            src={state.videoSrc}
            muted={state.muted}
            aria-label={strings.capturedVideoLabel}
            playsInline
            loop
          />
          <button
            className={combine(styles.buttonReset, styles.iconButton, styles.toggleMuteButton)}
            type='button'
            aria-label={state.muted ? strings.unmutePreviewLabel : strings.mutePreviewLabel}
            onClick={onToggleMuted}
          >
            <img alt='' src={state.muted ? SOUND_OFF_SRC : SOUND_ON_SRC} />
          </button>
        </Fragment>
      }
    </div>
  </div>
)

export {MediaPreviewMedia}
