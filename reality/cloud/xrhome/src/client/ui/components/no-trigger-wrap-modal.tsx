import React from 'react'
import {createUseStyles} from 'react-jss'
import {
  autoUpdate, FloatingFocusManager, FloatingOverlay, FloatingPortal, Placement, useClick,
  useDismiss, useFloating, useInteractions, useRole,
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

interface INoTriggerWrapModal {
  trigger: React.ReactElement
  children: (collapse: () => void) => React.ReactNode
  isOpen: boolean
  setIsOpen: (open: boolean) => void
  overlayClass?: string
  placement?: Placement
}

const NoTriggerWrapModal: React.FC<INoTriggerWrapModal> = ({
  trigger, children, overlayClass, isOpen, setIsOpen, placement = 'right-end',
}) => {
  const classes = useStyles()

  const {refs, context} = useFloating({
    open: isOpen,
    onOpenChange: setIsOpen,
    placement,
    whileElementsMounted: autoUpdate,
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
    setIsOpen(false)
  }

  return (
    <>
      {React.cloneElement(trigger, {
        ...getReferenceProps({
          ref: refs.setReference,
        }),
      })}
      <FloatingPortal>
        {isOpen &&
          <FloatingOverlay className={combine(classes.overlay, overlayClass)} lockScroll>
            <FloatingFocusManager context={context}>
              <div className={classes.container} {...getFloatingProps()}>
                <FloatingTray isOpaque>
                  {children(collapse)}
                </FloatingTray>
              </div>
            </FloatingFocusManager>
          </FloatingOverlay>
        }
      </FloatingPortal>
    </>
  )
}

export {
  NoTriggerWrapModal,
}
