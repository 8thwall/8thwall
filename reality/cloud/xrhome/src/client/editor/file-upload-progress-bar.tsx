import * as React from 'react'
import {useTranslation} from 'react-i18next'

import {combine} from '../common/styles'
import {ProgressBar} from '../ui/components/progress-bar'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  uploadProgress: {
    backgroundColor: theme.mainEditorPane,
    width: '100%',
    height: '65px',
    whiteSpace: 'nowrap',
    opacity: '0.9',
    overflow: 'hidden',
    position: 'sticky',
    bottom: 0,
    left: 0,
    transition: 'height 0.5s ease-out',
    transitionDelay: '0.5s',
  },

  uploadMessage: {
    display: 'flex',
    justifyContent: 'space-between',
    margin: '0.5em 1em',
    color: theme.fgMain,
  },

  uploadProgressBar: {
    margin: '0.5em 1em',
    width: 'calc(100% - 2em)',
  },

  collapsed: {
    height: '0px',
    transition: 'height 0.5s ease-in',
  },
}))

interface IFileUploadProgressBar {
  numFileUploading: number
  totalNumFiles: number
  bytesUploaded: number
  totalBytes: number
}

const FileUploadProgressBar: React.FunctionComponent<IFileUploadProgressBar> = ({
  numFileUploading, totalNumFiles, bytesUploaded, totalBytes,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const status = totalNumFiles !== 0
    ? t('asset_configurator.image_target_configurator.uploading', {ns: 'cloud-studio-pages'})
    : t('status.complete', {ns: 'common'})

  return (
    <div
      className={totalNumFiles !== 0
        ? classes.uploadProgress
        : combine(classes.uploadProgress, classes.collapsed)
      }
    >
      <div className={classes.uploadMessage}>
        <span>{status}</span>
        <span>{totalNumFiles !== 0 ? `${numFileUploading} of ${totalNumFiles}` : ''}</span>
      </div>
      <div className={classes.uploadProgressBar}>
        <ProgressBar
          ariaLabel={status}
          progress={totalBytes !== 0 ? bytesUploaded / totalBytes : 1}
          color='gradient'
          transitionDuration={BuildIf.LOCAL_DEV ? '2s' : undefined}
        />
      </div>
    </div>
  )
}

export default FileUploadProgressBar
