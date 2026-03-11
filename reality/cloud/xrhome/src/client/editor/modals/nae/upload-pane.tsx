import React from 'react'
import {useTranslation} from 'react-i18next'

import type {HtmlShell} from '../../../../shared/nae/nae-types'

import {
  NAE_ANDROID_ICON_MIN_SIZE,
  NAE_IOS_ICON_MIN_SIZE,
} from '../../../../shared/nae/nae-constants'
import {createThemedStyles} from '../../../ui/theme'
import {combine, bool} from '../../../common/styles'
import {
  type IconDisplayShape,
  ICON_DISPLAY_SHAPES,
  IconPreview,
} from '../../../apps/widgets/icon-preview'
import {gray2, smallMonitorViewOverride} from '../../../static/styles/settings'
import {TextNotification} from '../../../ui/components/text-notification'
import {PublishingPrimaryButton} from '../../publishing/publish-primary-button'

const useStyles = createThemedStyles(theme => ({
  appIconPane: {
    display: 'flex',
    padding: '1rem',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.5rem',
    alignSelf: 'stretch',
    borderRadius: '0.5rem',
    backgroundColor: theme.inputFieldBg,
    [smallMonitorViewOverride]: {
      flexDirection: 'row',
      gap: '1rem',
    },
  },
  smallMonitorViewHidden: {
    [smallMonitorViewOverride]: {
      display: 'none !important',
    },
  },
  appIconRight: {
    display: 'none',
    flexDirection: 'column',
    justifyContent: 'space-between',
    height: '100%',
    [smallMonitorViewOverride]: {
      display: 'flex',
    },
  },
  appName: {
    fontSize: '12px',
    lineHeight: '18px',
    fontWeight: 600,
    wordBreak: 'break-word',
  },
  appIconPreview: {
    lineHeight: 0,
  },
  cursorPointer: {
    cursor: 'pointer',
  },
  appIconImg: {
    width: '5rem',
    height: '5rem',
    aspectRatio: '1/1',
    border: `1px solid ${theme.subtleBorder}`,
    borderRadius: '0.5rem',
    objectFit: 'cover',
  },
  appIconBottom: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.5rem',
    alignSelf: 'stretch',
  },
  appIconText: {
    alignSelf: 'stretch',
    fontSize: '12px',
    lineHeight: '16px',
    color: theme.fgMuted,
  },
  appIconUploadButton: {
    display: 'flex',
    height: '24px',
    padding: '4px 8px',
    justifyContent: 'center',
    alignSelf: 'stretch',
    fontSize: '12px',
    lineHeight: '16px',
    borderRadius: '0.25rem',
  },
  launchScreenPreview: {
    width: '5rem',
    height: '8.875rem',
    border: `0.5px solid ${gray2}`,
    borderRadius: '0.5rem',
    // eslint-disable-next-line max-len
    background: 'radial-gradient(142.09% 149.79% at 50% 0%, #000 0%, rgba(91, 49, 119, 0.25) 35%, rgba(87, 191, 255, 0.35) 73.57%), #000;',
    position: 'relative',
    overflow: 'hidden',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
  },
  launchScreenIcon: {
    width: '3.75rem',
    height: '3.75rem',
    objectFit: 'cover',
    borderRadius: '0.5rem',
    border: `1px solid ${gray2}`,
    backgroundColor: 'lightgray',
    backgroundSize: 'cover',
  },
}))

const getSizeLimit = (platform: HtmlShell) => {
  switch (platform) {
    case 'android':
      return NAE_ANDROID_ICON_MIN_SIZE
    case 'ios':
      return NAE_IOS_ICON_MIN_SIZE
    default:
      return NAE_ANDROID_ICON_MIN_SIZE
  }
}

interface IUploadPane {
  uploadType: 'app-icon' | 'launch-screen'
  iconPreview: string
  setIconPreview: (iconUrl: string) => void
  setIconFile: (file: File) => void
  appDisplayName?: string
  platform: HtmlShell
}

const UploadPane: React.FC<IUploadPane> = ({
  uploadType,
  iconPreview,
  setIconPreview,
  setIconFile,
  appDisplayName,
  platform,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const fileInputRef = React.useRef<HTMLInputElement>(null)
  const [iconDisplayShape, setIconDisplayShape] = React.useState<IconDisplayShape>('square')
  const [showSizeError, setShowSizeError] = React.useState(false)
  const sizeLimit = getSizeLimit(platform)
  const sizeLimitString = `${sizeLimit} x ${sizeLimit}`

  const handleIconUpload = (file: File, limit: number) => {
    if (!file) {
      return
    }
    setIconFile(file)
    const reader = new FileReader()
    reader.onload = (e) => {
      if (e.target?.result && typeof e.target.result === 'string') {
        const img = new Image()
        img.onload = () => {
          const {width, height} = img
          if (width >= limit && height >= limit) {
            setIconPreview(e.target.result as string)
            setShowSizeError(false)
          } else {
            setShowSizeError(true)
          }
        }
        img.src = e.target.result
      }
    }
    reader.readAsDataURL(file)
  }

  const handleIconPreviewClick = () => {
    setIconDisplayShape((prev) => {
      const currentIndex = ICON_DISPLAY_SHAPES.indexOf(prev)
      const nextIndex = (currentIndex + 1) % ICON_DISPLAY_SHAPES.length
      return ICON_DISPLAY_SHAPES[nextIndex]
    })
  }

  const UploadSection: React.FC = () => (
    <>
      <span className={classes.appIconText}>
        {t('editor_page.export_modal.app_icon.description',
          {sizeLimit: sizeLimitString})}
      </span>
      {showSizeError && (
        <TextNotification type='danger'>
          {t('editor_page.export_modal.app_icon.size_error',
            {sizeLimit: sizeLimitString})}
        </TextNotification>
      )}
      <PublishingPrimaryButton
        type='button'
        className={classes.appIconUploadButton}
        onClick={() => fileInputRef.current?.click()}
        a8={`click;${uploadType}-upload;`}
      >
        {uploadType === 'app-icon' && (
          t('editor_page.export_modal.app_icon.upload')
        )}
        {uploadType === 'launch-screen' && (
          t('editor_page.export_modal.launch_screen.upload')
        )}
      </PublishingPrimaryButton>
    </>
  )

  return (
    <div className={classes.appIconPane}>
      {appDisplayName && (
        <div className={combine(classes.appName, classes.smallMonitorViewHidden)}>
          {appDisplayName}
        </div>
      )}
      {uploadType === 'app-icon' && (
        <button
          type='button'
          className={
          combine('style-reset',
            classes.appIconPreview,
            bool(platform === 'android', classes.cursorPointer))
        }
          onClick={platform === 'android' ? handleIconPreviewClick : undefined}
          a8='click;app-icon-preview;'
        >
          <IconPreview
            className={classes.appIconImg}
            src={iconPreview}
            shape={iconDisplayShape}
          />
        </button>
      )}
      {uploadType === 'launch-screen' && (
        <div className={classes.launchScreenPreview}>
          <div
            style={{backgroundImage: iconPreview ? `url(${iconPreview})` : 'none'}}
            className={classes.launchScreenIcon}
          />
        </div>
      )}
      <input
        type='file'
        id='iconUpload'
        accept='image/png,image/jpeg'
        onChange={(e) => {
          const file = e.target.files?.[0]
          if (file) {
            handleIconUpload(file, sizeLimit)
          }
        }}
        ref={fileInputRef}
        hidden
      />
      <div className={classes.appIconRight}>
        {appDisplayName && (
          appDisplayName && <div className={classes.appName}>{appDisplayName}</div>
        )}
        <UploadSection />
      </div>

      <div className={combine(classes.appIconBottom, classes.smallMonitorViewHidden)}>
        <UploadSection />
      </div>
    </div>
  )
}

export {UploadPane}
