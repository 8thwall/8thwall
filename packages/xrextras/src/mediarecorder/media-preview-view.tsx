import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import {combine} from '../classnames'
import {useMediaPreview} from './media-preview-controller'
import {MediaPreviewControls} from './media-preview-controls'
import {MediaPreviewMedia} from './media-preview-media'
import styles from './media-preview.module.scss'
import type {MediaPreviewOptions} from './media-preview-types'
import type {MediaRecorderStrings} from './strings'

interface MediaPreviewViewProps {
  options: MediaPreviewOptions
  strings: MediaRecorderStrings
}

const MediaPreviewView: FC<MediaPreviewViewProps> = ({options, strings}) => {
  const controller = useMediaPreview(options, strings)
  const {actions, state} = controller
  const className = combine(
    styles.previewContainer,
    state.visible && styles.fadeIn
  )

  return (
    <div
      className={className}
      style={state.interactable ? {pointerEvents: 'auto'} : undefined}
    >
      <MediaPreviewMedia
        onClose={controller.closePreview}
        onToggleMuted={controller.toggleMuted}
        state={state}
        strings={strings}
        videoRef={controller.videoRef}
      />
      <MediaPreviewControls
        actions={actions}
        onAction={controller.performAction}
        onDownload={controller.downloadFile}
        options={options}
        state={state}
        strings={strings}
      />
    </div>
  )
}

export {MediaPreviewView}
