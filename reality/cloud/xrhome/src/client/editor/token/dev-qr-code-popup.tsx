import React from 'react'
import {createUseStyles} from 'react-jss'
import {useEffect} from 'react'
import {
  FloatingPortal, useFloating, offset, shift, flip, autoUpdate,
  useClick, useDismiss, useRole, useInteractions,
  Placement,
} from '@floating-ui/react'

import type {IApp} from '../../common/types/models'
import editorActions from '../editor-actions'
import '../../static/styles/code-editor.scss'
import useActions from '../../common/use-actions'
import {
  almostBlack, brandWhite, gray4,
} from '../../static/styles/settings'
import {UiThemeProvider} from '../../ui/theme'
import {RemoteSetupView} from './remote-setup-view'

interface IProps {
  app: IApp
  trigger: React.ReactNode | ((isOpen: boolean) => React.ReactNode)
  placement?: Placement
  shrink?: boolean
}

const useStyles = createUseStyles({
  devQrCodePopup: {
    'z-index': '1000',
    'color': almostBlack,
    'background-color': brandWhite,
    'border-radius': '12px',
    'minWidth': '20rem',
    'minHeight': '10rem',
    'overflow': 'hidden',
    'border': `1px solid ${gray4}`,
  },
  minWidth: {
    minWidth: '0',
  },
})

const DevQRCodePopup: React.FunctionComponent<IProps> = React.memo(({
  app,
  trigger,
  shrink = false,
  placement,
}) => {
  const [popupOpen, setPopupOpen] = React.useState(false)
  const {
    loadPreviewLinkDebugModeSelected,
    ensureSimulatorStateReady,
  } = useActions(editorActions)
  const classes = useStyles()

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
      flip(),
    ],
    whileElementsMounted: (ref, floating, update) => autoUpdate(ref, floating, update),
  })

  const click = useClick(context)
  const dismiss = useDismiss(context)
  const role = useRole(context)

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])

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
            style={floatingStyles}
            className={classes.devQrCodePopup}
            {...getFloatingProps()}
          >
            <UiThemeProvider mode='light'>
              <RemoteSetupView />
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
