import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import {combine} from '../classnames'
import DOWNLOAD_SRC from './download.svg'
import styles from './media-preview.module.scss'
import type {
  MediaPreviewOptions,
  MediaPreviewState,
  PreviewActionButton,
  PreviewActions,
} from './media-preview-types'
import type {MediaRecorderStrings} from './strings'

interface MediaPreviewControlsProps {
  actions: PreviewActions
  onAction: () => void
  onDownload: () => void
  options: MediaPreviewOptions
  state: MediaPreviewState
  strings: MediaRecorderStrings
}

const actionButtonClass = ({layout}: PreviewActionButton) => {
  if (layout === 'alone') {
    return styles.showAlone
  }
  return styles.showWithDownloadButton
}

const MediaPreviewControls: FC<MediaPreviewControlsProps> = ({
  actions,
  onAction,
  onDownload,
  options,
  state,
  strings,
}) => {
  const actionButton = actions.actionButton?.layout === 'after-download' && !state.downloaded
    ? undefined
    : actions.actionButton

  return (
    <div className={styles.bottomBar}>
      {state.finalizeWaiting &&
        <div className={styles.finalizeContainer}>
          <div className={styles.finalizeText}>
            {options.finalizeText || strings.preparingLabel}
          </div>
          <progress
            className={styles.finalizeProgressBar}
            value={state.finalizeProgress}
            max={state.finalizeTotal}
            aria-label={strings.finalizeProgressLabel}
          />
        </div>
      }
      {actions.showDownloadButton &&
        <button
          className={combine(styles.buttonReset, styles.iconButton)}
          type='button'
          aria-label={strings.downloadLabel}
          onClick={onDownload}
        >
          <img alt='' src={DOWNLOAD_SRC} />
        </button>
      }
      {actions.disabledDownload && state.previewType === 'video' &&
        <div className={styles.openSafariText}>
          {strings.safariDownloadOnlyMessage}
        </div>
      }
      {actions.disabledDownload && state.previewType === 'image' &&
        <div className={styles.tapAndHoldText}>
          {strings.tapAndHoldToShareMessage}
        </div>
      }
      {actionButton &&
        <button
          className={combine(
            styles.buttonReset,
            styles.actionButton,
            actionButtonClass(actionButton)
          )}
          type='button'
          onClick={onAction}
        >
          {actionButton.text}
          <img
            className={styles.actionButtonImg}
            alt=''
            src={actionButton.icon}
          />
        </button>
      }
    </div>
  )
}

export {MediaPreviewControls}
