import React from 'react'
import {
  FloatingFocusManager, FloatingOverlay, FloatingPortal, useClick, useDismiss, useFloating,
  useInteractions,
  useRole,
} from '@floating-ui/react'

import {hexColorWithAlpha} from '../../../shared/colors'
import {brand8Black} from '../colors'
import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

type ModalWidth = 'narrow' | 'unset'

const useStyles = createThemedStyles(theme => ({
  triggerContainer: {
    'width': 'fit-content',
    'height': 'fit-content',
    'pointerEvents': 'none',
    '& *': {
      pointerEvents: 'auto',
    },
  },
  container: {
    padding: '0.75em',
  },
  overlay: {
    zIndex: 100,
    display: 'grid',
    placeItems: 'center',
    height: '100%',
  },
  dimmer: {
    background: theme.modalDimmerBg,
    backdropFilter: theme.modalDimmerBackdropFilter,
  },
  modal: {
    color: theme.fgMain,
    background: theme.standardModalBg,
    borderRadius: '14px',
    border: theme.modalBorder,
    backdropFilter: theme.modalBackdropFilter,
    boxShadow: `0px 8px 16px ${hexColorWithAlpha(brand8Black, 0.2)}`,
  },
  narrow: {
    width: '32rem',
  },
}))

interface IStandardModal {
  trigger: React.ReactElement | 'render'
  children: React.ReactNode | ((collapse: () => void) => React.ReactNode)
  onOpenChange?: (open: boolean) => void
  hasDimmer?: boolean
  closable?: boolean
  width?: ModalWidth
}

const StandardModal: React.FC<IStandardModal> = ({
  trigger, onOpenChange, children, hasDimmer = true, closable = true,
  width = 'unset',
}) => {
  const classes = useStyles()
  const [modalOpenState, setModalOpen] = React.useState(false)
  const modalOpen = trigger === 'render' ? true : modalOpenState

  const handleOpenChange = (open: boolean) => {
    setModalOpen(open)
    onOpenChange?.(open)
  }

  const {refs, context} = useFloating({
    open: modalOpen,
    onOpenChange: handleOpenChange,
  })
  const click = useClick(context)
  const role = useRole(context)
  const dismiss = useDismiss(context, {
    enabled: closable,
  })

  const {getReferenceProps, getFloatingProps} = useInteractions([
    click,
    dismiss,
    role,
  ])

  return (
    <>
      {trigger !== 'render' &&
        <div className={classes.triggerContainer} ref={refs.setReference} {...getReferenceProps()}>
          {trigger}
        </div>
      }
      {modalOpen &&
        <FloatingPortal>
          <FloatingOverlay
            className={combine(classes.overlay, hasDimmer && classes.dimmer)}
            lockScroll
          >
            <FloatingFocusManager context={context}>
              <div className={classes.container} {...getFloatingProps()}>
                <div className={combine(classes.modal, classes[width])}>
                  {typeof children === 'function'
                    ? children(() => handleOpenChange(false))
                    : children}
                </div>
              </div>
            </FloatingFocusManager>
          </FloatingOverlay>
        </FloatingPortal>
      }
    </>
  )
}

export {
  StandardModal,
}

export type {
  ModalWidth,
}
