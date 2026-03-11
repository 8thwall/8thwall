import React, {useRef, useState} from 'react'
import {
  useFloating,
  autoUpdate,
  offset,
  flip,
  shift,
  useHover,
  useFocus,
  useDismiss,
  useRole,
  useInteractions,
  FloatingPortal,
  FloatingArrow,
  arrow,
} from '@floating-ui/react'
import type {FloatingContext, Placement} from '@floating-ui/react'

import {createThemedStyles, UiThemeProvider, useUiTheme} from '../theme'
import {bodySanSerif} from '../../static/styles/settings'

const useStyles = createThemedStyles(theme => ({
  tooltip: {
    fontFamily: bodySanSerif,
    wordWrap: 'break-word',
    backgroundColor: theme.tooltipBg,
    borderRadius: '0.2rem',
    width: 'max-content',
    maxWidth: '30rem',
    fontSize: '14px',
    fontWeight: '400',
    padding: '0.5rem',
    color: theme.fgMain,
    boxShadow: '2px 2px 5px rgba(0, 0, 0, 0.2)',
    userSelect: 'none',
  },
  flex: {
    display: 'flex',
  },
}))

interface ITooltip {
  children: React.ReactNode
  content: React.ReactNode
  position?: Placement
  openDelay?: number
  closeDelay?: number
  zIndex?: number
}

interface ITooltipContent {
  context: FloatingContext
  floatingStyles: React.CSSProperties
  setFloating: (node: HTMLElement) => void
  arrowRef: React.RefObject<SVGSVGElement>
  getFloatingProps: (userProps?: React.HTMLProps<HTMLElement>) => Record<string, unknown>
  content: React.ReactNode
  zIndex: number
}

const TooltipContent: React.FC<ITooltipContent> = ({
  context, floatingStyles, setFloating, arrowRef, getFloatingProps, content, zIndex,
}) => {
  const classes = useStyles()
  const theme = useUiTheme()

  return (
    <div
      className={classes.tooltip}
      ref={setFloating}
      style={{zIndex, ...floatingStyles}}
      {...getFloatingProps()}
    >
      <FloatingArrow
        ref={arrowRef}
        context={context}
        height={10}
        tipRadius={2}
        fill={theme.tooltipBg}
      />
      {content}
    </div>
  )
}

// Note(Cindy): Floating UI: https://floating-ui.com/docs/tooltip
// note(owenmech): arrow: https://floating-ui.com/docs/floatingarrow
const Tooltip: React.FC<ITooltip> = ({
  children, content, position = 'top', openDelay = 0, closeDelay = 0, zIndex = 10,
}) => {
  const [isOpen, setIsOpen] = useState(false)
  const arrowRef = useRef<SVGSVGElement>(null)

  const {refs, floatingStyles, context} = useFloating({
    open: isOpen,
    onOpenChange: setIsOpen,
    placement: position,
    whileElementsMounted: autoUpdate,
    middleware: [
      offset(10),
      shift(),
      flip({
        fallbackAxisSideDirection: 'start',
      }),
      arrow({
        element: arrowRef,
        padding: 5,
      }),
    ],
  })

  const hover = useHover(context, {move: false, delay: {open: openDelay, close: closeDelay}})
  const focus = useFocus(context)
  const dismiss = useDismiss(context)
  const role = useRole(context, {role: 'tooltip'})

  const {getReferenceProps, getFloatingProps} = useInteractions([
    hover,
    focus,
    dismiss,
    role,
  ])

  const theme = useUiTheme()
  const classes = useStyles()

  return (
    <>
      <span ref={refs.setReference} {...getReferenceProps()} className={classes.flex}>
        {children}
      </span>
      {content &&
        <FloatingPortal>
          {isOpen && (
            <UiThemeProvider mode={theme.tooltipInverted ? 'dark' : 'light'}>
              <TooltipContent
                context={context}
                floatingStyles={floatingStyles}
                setFloating={refs.setFloating}
                arrowRef={arrowRef}
                getFloatingProps={getFloatingProps}
                content={content}
                zIndex={zIndex}
              />
            </UiThemeProvider>
          )}
        </FloatingPortal>
      }
    </>
  )
}

export {Tooltip}

export type {
  ITooltip,
}
