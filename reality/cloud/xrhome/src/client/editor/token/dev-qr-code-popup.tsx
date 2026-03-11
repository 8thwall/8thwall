import React from 'react'
import {Icon} from 'semantic-ui-react'
import {createUseStyles} from 'react-jss'
import {useEffect} from 'react'
import {
  FloatingPortal, useFloating, offset, shift,
  useClick, useDismiss, useRole, useInteractions,
  Placement,
} from '@floating-ui/react'

import {Trans, useTranslation} from 'react-i18next'

import type {IApp} from '../../common/types/models'
import editorActions from '../editor-actions'
import '../../static/styles/code-editor.scss'
import useActions from '../../common/use-actions'
import {
  almostBlack, brandBlack, brandWhite, gray1, gray4, gray5, moonlight,
} from '../../static/styles/settings'
import {BasicQrCode} from '../../widgets/basic-qr-code'
import {useSelector} from '../../hooks'
import {UiThemeProvider} from '../../ui/theme'
import {hexColorWithAlpha} from '../../../shared/colors'
import {useAppPreviewWindow} from '../../common/app-preview-window-context'
import {combine} from '../../common/styles'
import {useProjectPreviewUrl} from '../app-preview/use-project-preview-url'
import {Loader} from '../../ui/components/loader'

interface IProps {
  app: IApp
  trigger: React.ReactNode | ((isOpen: boolean) => React.ReactNode)
  placement?: Placement
  shrink?: boolean
}

const useStyles = createUseStyles({
  withUpwardArrow: {
    '&::before': {
      'background-color': `${gray1} !important`,
      'content': '""',
      'transform': 'rotate(45deg)',
      'width': '12px',
      'position': 'absolute',
      'display': 'block',
      'height': '12px',
      'left': 'calc(50% - 6px)',
      'top': '-6px',
      'border-left': `1px solid ${gray4}`,
      'border-top': `1px solid ${gray4}`,
    },
  },
  qrPopupContainer: {
    'z-index': '1000',
    'border': `1px solid ${gray4}`,
    'border-radius': '12px',
    'color': almostBlack,
  },
  qrPopup: {
    'fontSize': '14px',
    'font-weight': '700',
    'text-align': 'center',
    'border-radius': '12px',
    '& .ui.button': {
      'background-color': brandWhite,
      'color': almostBlack,
    },
    'background-color': brandWhite,
  },
  qr: {
    position: 'relative',
    padding: '1em',
    minWidth: '163px',
    minHeight: '163px',
  },
  qrImg: {
    display: 'block',
    width: 'calc(163px-1em)',
    height: 'calc(163px-1em)',
  },
  qrImgContainer: {
    position: 'relative',
  },
  hudToggleSection: {
    'background-color': gray1,
    'padding': '.5em',
    'border-top-right-radius': '12px',
    'border-top-left-radius': '12px',
  },
  hudControl: {
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'gap': '0.5em',
    'position': 'relative',
  },
  hudCheckbox: {
    'appearance': 'none',
    'backgroundColor': moonlight,
    'border': `2px solid ${brandBlack}`,
    'border-radius': '3px',
    'display': 'inline-block',
    'position': 'relative',
    'padding': '5px',
    '&:checked': {
      'background-color': brandBlack,
    },
    '&:checked:after': {
      content: '"\u2714"',
      fontSize: '10px',
      position: 'absolute',
      top: '0',
      left: '1px',
      color: moonlight,
    },
  },
  actions: {
    display: 'inline-flex',
    gap: '4px',
    margin: '0 1em .5em 1em',
  },
  actionButton: {
    lineHeight: 'normal',
    fontFamily: 'inherit',
    fontWeight: '700',
    fontSize: '14px',
    boxSizing: 'border-box',
    background: gray5,
    color: brandWhite,
    padding: '5px 8px',
    display: 'flex',
    justifyContent: 'center',
    gap: '4px',
    alignItems: 'center',
    borderRadius: '6px',
    border: 'none',
    margin: 0,
    cursor: 'pointer',
  },
  linkOut: {
    '&:hover': {
      color: brandWhite,
      textDecoration: 'none',
    },
  },
  qrOverlayLink: {
    'position': 'absolute',
    'top': '-4px',
    'left': '-4px',
    'right': '-4px',
    'bottom': '-4px',
    'cursor': 'pointer',
    'color': brandWhite,
    'opacity': 0,
    'backgroundColor': hexColorWithAlpha(gray5, 0.7),
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'borderRadius': '6px',
    'transition': 'opacity 0.3s, backdrop-filter 0.3s',
    '&:hover, &:focus-visible': {
      'backdropFilter': 'blur(2px)',
      'color': brandWhite,
      'opacity': 1,
    },
  },
  // PreviewOverLANOverlay classes
  PreviewOverLANOverlay: {
    aspectRatio: '1 / 1',
    padding: '.2rem',
    textAlign: 'left',
    position: 'absolute',
    top: '0',
    left: '0',
    right: '0',
    fontWeight: 'normal',
    backgroundColor: 'rgba(255, 255, 255, 0.9)',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'flex-end',
  },
  bold: {
    fontWeight: 800,
    display: 'block',
  },
  minWidth: {
    minWidth: '0',
  },
})

const DevQRCodePopup: React.FunctionComponent<IProps> = React.memo(({
  app,
  trigger,
  shrink = false,
  placement = 'bottom',
}) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const [popupOpen, setPopupOpen] = React.useState(false)
  const {
    setPreviewLinkDebugModeSelected,
    loadPreviewLinkDebugModeSelected,
    ensureSimulatorStateReady,
  } = useActions(editorActions)
  const classes = useStyles()
  const previewLinkDebugMode = useSelector(state => state.editor.previewLinkDebugMode)

  const remoteDeviceUrl = useProjectPreviewUrl(app, 'remote-device')
  const sameDeviceUrl = useProjectPreviewUrl(app, 'same-device')

  const {setPreviewWindow} = useAppPreviewWindow()

  useEffect(() => {
    loadPreviewLinkDebugModeSelected()
    ensureSimulatorStateReady(app.appKey)
  }, [app.appKey])

  const {refs, floatingStyles, context} = useFloating({
    open: popupOpen,
    onOpenChange: setPopupOpen,
    placement,
    middleware: [
      offset(14),
      shift(),
    ],
  })

  const click = useClick(context)
  const dismiss = useDismiss(context)
  const role = useRole(context)

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])

  const renderQrCodeOverlay = () => (
    <div
      role='presentation'
      className={classes.qrOverlayLink}
        // @ts-ignore
      a8='click;cloud-editor;preview-link'
      onClick={() => {
        const newTab = window.open(sameDeviceUrl)
        setPreviewWindow(newTab)
      }}
    >
      <span>
        <Trans
          ns='cloud-editor-pages'
          i18nKey='editor_page.dev_qr_code_popup.open_in_new_tab_2'
          components={{
            icon: <Icon name='external' />,
          }}
        />
      </span>
    </div>
  )

  return (
    <>
      <div
        className={shrink ? classes.minWidth : undefined}
        ref={refs.setReference}
        {...getReferenceProps()}
      >
        {typeof trigger === 'function' ? trigger(popupOpen) : trigger}
      </div>
      <FloatingPortal>
        {popupOpen &&
          <div
            ref={refs.setFloating}
            style={{...floatingStyles}}
            className={combine(
              classes.qrPopupContainer,
              placement === 'bottom' && classes.withUpwardArrow
            )}
            {...getFloatingProps()}
          >
            <UiThemeProvider mode='light'>
              <div className={classes.qrPopup}>
                <div className={classes.hudToggleSection}>
                  <label className={classes.hudControl} htmlFor='editor-hud-debug-checkbox'>
                    {t('editor_page.dev_qr_code_popup.debug_mode')}
                    <input
                      id='editor-hud-debug-checkbox'
                      type='checkbox'
                      checked={previewLinkDebugMode}
                      className={classes.hudCheckbox}
                      onChange={() => setPreviewLinkDebugModeSelected(!previewLinkDebugMode)}
                    />
                  </label>
                </div>
                <div className={classes.qr}>
                  {remoteDeviceUrl
                    ? (
                      <div className={classes.qrImgContainer}>
                        {renderQrCodeOverlay()}
                        <BasicQrCode
                          url={remoteDeviceUrl}
                          ecl='l'
                          margin={0}
                          className={classes.qrImg}
                        />
                      </div>
                    )
                    : <Loader />
                  }
                </div>
              </div>
            </UiThemeProvider>
          </div>
          }
      </FloatingPortal>
    </>
  )
})

export {
  DevQRCodePopup,
}
