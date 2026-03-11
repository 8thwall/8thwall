import React, {useState} from 'react'
import {createUseStyles} from 'react-jss'
import {
  FloatingPortal, autoUpdate, useFloating, offset, shift, size,
  useClick, useDismiss, useRole, useInteractions, FloatingFocusManager, flip,
  Placement,
} from '@floating-ui/react'

import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  selectMenuContainer: {
    position: 'relative',
    userSelect: 'none',
    cursor: 'pointer',
  },
  menuWrapper: {
    position: 'absolute',
    display: 'none',
    left: 0,
    right: 0,
    overflowY: 'auto',
    zIndex: 100,
  },
  menuOpen: {
    display: 'block',
  },
  shrink: {
    minWidth: 0,
  },
  grow: {
    flexGrow: 1,
  },
})

interface ISelectMenu {
  id: string
  children: (collapse: () => void) => React.ReactNode
  disabled?: boolean
  menuWrapperClassName?: string
  trigger: React.ReactNode
  matchTriggerWidth?: boolean
  minTriggerWidth?: boolean
  maxWidth?: number
  onOpenChange?: (open: boolean) => void
  onDismiss?: () => void
  placement?: Placement
  margin?: number
  shrink?: boolean
  grow?: boolean
  staticWidth?: boolean
  isOpen?: boolean
  returnFocus?: boolean
}

const SelectMenu: React.FC<ISelectMenu> = ({
  id, children, menuWrapperClassName, trigger, matchTriggerWidth, maxWidth, onOpenChange,
  onDismiss, placement, margin, disabled, minTriggerWidth, shrink = false, grow = false,
  staticWidth = false, isOpen: isControlledOpen, returnFocus = true,
}) => {
  const [uncontrolledMenuOpen, setUncontrolledMenuOpen] = useState(false)
  const [widthOnFirstOpen, setWidthOnFirstOpen] = useState(-1)
  const classes = useStyles()

  const menuOpen = isControlledOpen ?? uncontrolledMenuOpen
  const {refs, floatingStyles, context} = useFloating({
    open: menuOpen,
    onOpenChange: (open, event, reason) => {
      if (!disabled) {
        setUncontrolledMenuOpen(open)
        onOpenChange?.(open)
      }
      if (onDismiss) {
        if (reason === 'escape-key' || reason === 'outside-press') {
          onDismiss()
        }
      }
    },
    placement: placement ?? 'bottom-start',
    whileElementsMounted: autoUpdate,
    middleware: [
      size({
        apply({rects, elements}) {
          let width
          if (matchTriggerWidth) {
            width = rects.reference.width
          } else if (staticWidth) {
            if (widthOnFirstOpen === -1) {
              width = rects.floating.width
              setWidthOnFirstOpen(width)
            } else {
              width = widthOnFirstOpen
            }
          }
          Object.assign(elements.floating.style, {
            width: width ? `${width}px` : undefined,
            minWidth: minTriggerWidth ? `${rects.reference.width}px` : undefined,
          })
        },
        padding: 10,
      }),
      offset(margin ?? 8),
      shift(),
      flip(),
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

  const collapse = () => {
    setUncontrolledMenuOpen(false)
    onOpenChange?.(false)
  }

  return (
    <div
      className={combine(
        classes.selectMenuContainer, shrink && classes.shrink, grow && classes.grow
      )}
    >
      <div ref={refs.setReference} {...getReferenceProps()}>
        {trigger}
      </div>
      {menuOpen && (
        <FloatingPortal>
          <FloatingFocusManager
            context={context}
            modal={false}
            initialFocus={refs.floating}
            returnFocus={returnFocus}
          >
            {/* eslint-disable-next-line jsx-a11y/no-noninteractive-element-interactions */}
            <div
              role='dialog'
              ref={refs.setFloating}
              className={combine(classes.menuOpen, classes.menuWrapper, menuWrapperClassName)}
              aria-labelledby={id}
              style={{
                ...floatingStyles,
                maxWidth: maxWidth ? `${maxWidth}px` : undefined,
              }}
              {...getFloatingProps()}
            >
              {children(collapse)}
            </div>
          </FloatingFocusManager>
        </FloatingPortal>
      )}
    </div>
  )
}

export {SelectMenu}
