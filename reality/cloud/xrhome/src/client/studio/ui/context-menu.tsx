import React from 'react'
import {createUseStyles} from 'react-jss'

import {
  autoUpdate,
  ExtendedRefs,
  flip,
  FloatingContext,
  FloatingFocusManager,
  FloatingPortal,
  offset,
  ReferenceType,
  shift,
  size,
  useDismiss,
  useFloating,
  useInteractions,
  useRole,
} from '@floating-ui/react'

import {combine} from '../../common/styles'
import {MenuOption, MenuOptions} from './option-menu'
import {useStudioMenuStyles} from './studio-menu-styles'

const useStyles = createUseStyles({
  contextMenu: {
    display: 'none',
    zIndex: 200,
  },
  menuOpen: {
    display: 'flex',
    width: 'fit-content',
  },
})

interface IFloatingContextMenuState {
  refs: ExtendedRefs<ReferenceType>
  floatingStyles: React.CSSProperties
  context: FloatingContext<ReferenceType>
  contextMenuOpen: boolean
  setContextMenuOpen: (open: boolean) => void
  getReferenceProps: () => Record<string, unknown>
  getFloatingProps: () => Record<string, unknown>
  handleContextMenu: (e: React.MouseEvent) => void
}

const useContextMenuState = (
  onChange?: (open: boolean) => void
): IFloatingContextMenuState => {
  const [contextMenuOpen, setContextMenuOpen] = React.useState(false)
  const {refs, floatingStyles, context} = useFloating({
    open: contextMenuOpen,
    onOpenChange: (open) => {
      setContextMenuOpen(open)
      if (onChange) {
        onChange(open)
      }
    },
    placement: 'right-start',
    whileElementsMounted: (reference, floating, update) => {
      if (contextMenuOpen) {
        return autoUpdate(reference, floating, update)
      }
      return undefined
    },
    middleware: [
      size({
        apply({elements}) {
          Object.assign(elements.floating.style, {
            width: `${elements.floating.children[0].clientWidth}px`,
          })
        },
      }),
      offset(8),
      flip({padding: 10}),
      shift({crossAxis: true, padding: 10}),
    ],
  })

  const dismiss = useDismiss(context)
  const role = useRole(context)

  const {getReferenceProps, getFloatingProps} = useInteractions(
    [dismiss, role]
  )

  const handleContextMenu = (e: React.MouseEvent) => {
    e.preventDefault()
    refs.setPositionReference({
      getBoundingClientRect() {
        return {
          width: 0,
          height: 0,
          x: e.clientX,
          y: e.clientY,
          top: e.clientY,
          right: e.clientX,
          bottom: e.clientY,
          left: e.clientX,
        }
      },
    })
    setContextMenuOpen(true)
    e.stopPropagation()
  }

  return {
    refs,
    floatingStyles,
    context,
    contextMenuOpen,
    setContextMenuOpen,
    getReferenceProps,
    getFloatingProps,
    handleContextMenu,
  }
}

interface IContextMenu {
  menuState: IFloatingContextMenuState
  options: MenuOption[] | ((collapse: () => void) => React.ReactNode)
}

const ContextMenu: React.FC<IContextMenu> = ({
  menuState, options,
}) => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()

  const {
    refs,
    floatingStyles,
    setContextMenuOpen,
    contextMenuOpen,
    context,
    getFloatingProps,
  } = menuState

  const collapse = () => setContextMenuOpen(false)

  return (contextMenuOpen &&
    <FloatingPortal>
      <FloatingFocusManager
        context={context}
        modal={false}
        initialFocus={refs.floating}
        returnFocus={false}
      >
        <div
          className={combine(classes.contextMenu, classes.menuOpen)}
          ref={refs.setFloating}
          tabIndex={-1}
          style={floatingStyles}
          {...getFloatingProps()}
        >
          <div className={menuStyles.studioMenu}>
            {Array.isArray(options)
              ? (
                <MenuOptions
                  options={options}
                  collapse={collapse}
                />
              )
              : (
                options(collapse)
              )}
          </div>
        </div>
      </FloatingFocusManager>
    </FloatingPortal>
  )
}

export {
  useContextMenuState,
  ContextMenu,
}
