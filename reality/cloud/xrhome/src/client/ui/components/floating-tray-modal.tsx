import React from 'react'
import {createUseStyles} from 'react-jss'
import {
  FloatingFocusManager, FloatingOverlay, FloatingPortal, useClick, useDismiss, useFloating,
  useInteractions,
  useRole,
} from '@floating-ui/react'

import {FloatingTray} from './floating-tray'
import {hexColorWithAlpha} from '../../../shared/colors'
import {darkBlue} from '../../static/styles/settings'
import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  container: {
    padding: '0.75em',
  },
  overlay: {
    zIndex: 100,
    display: 'grid',
    placeItems: 'center',
    background: hexColorWithAlpha(darkBlue, 0.2),
    height: '100%',
    backdropFilter: 'blur(10px)',
  },
})

interface IFloatingTrayModal {
  trigger: React.ReactNode
  children: (collapse: () => void) => React.ReactNode
  onOpenChange?: (open: boolean) => void
  startOpen?: boolean
  overlayClass?: string
}

const FloatingTrayModal: React.FC<IFloatingTrayModal> = ({
  trigger, onOpenChange, children, overlayClass, startOpen = false,
}) => {
  const classes = useStyles()
  const [modalOpen, setModalOpen] = React.useState(startOpen)

  const {refs, context} = useFloating({
    open: modalOpen,
    onOpenChange: (open) => {
      setModalOpen(open)
      onOpenChange?.(open)
    },
  })
  const click = useClick(context)
  const role = useRole(context)
  const dismiss = useDismiss(context)

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])

  const collapse = () => {
    setModalOpen(false)
  }

  return (
    <>
      <div ref={refs.setReference} {...getReferenceProps()}>
        {trigger}
      </div>
      {modalOpen &&
        <FloatingPortal>
          <FloatingOverlay className={combine(classes.overlay, overlayClass)} lockScroll>
            <FloatingFocusManager context={context}>
              <div className={classes.container} ref={refs.setFloating} {...getFloatingProps()}>
                <FloatingTray isOpaque>
                  {children(collapse)}
                </FloatingTray>
              </div>
            </FloatingFocusManager>
          </FloatingOverlay>
        </FloatingPortal>
        }
    </>
  )
}

export {
  FloatingTrayModal,
}
