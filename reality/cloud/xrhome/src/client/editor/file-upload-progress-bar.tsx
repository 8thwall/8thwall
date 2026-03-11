import * as React from 'react'
import {Progress} from 'semantic-ui-react'

import {combine} from '../common/styles'
import {popGradient, brandWhite} from '../static/styles/settings'
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
    'margin': '0.5em 1em !important',

    '& .bar': {
      background: `${popGradient} !important`,
    },

    '& .bar .progress': {
      color: brandWhite,
    },
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

  return (
    <div
      className={totalNumFiles !== 0
        ? classes.uploadProgress
        : combine(classes.uploadProgress, classes.collapsed)
    }
    >
      <div className={classes.uploadMessage}>
        <span>{totalNumFiles !== 0 ? 'Uploading...' : 'Completed'}</span>
        <span>{totalNumFiles !== 0 ? `${numFileUploading} of ${totalNumFiles}` : ''}</span>
      </div>
      <Progress
        className={classes.uploadProgressBar}
        percent={totalBytes !== 0 ? Math.round(bytesUploaded / totalBytes * 100) : 100}
        size='small'
        progress
        color='purple'
      />
    </div>
  )
}

export default FileUploadProgressBar
