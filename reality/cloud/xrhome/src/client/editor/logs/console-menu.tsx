import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  menu: {
    display: 'flex',
    boxSizing: 'border-box',
    fontFamily: '\'Nunito\', \'Helvetica Neue\', Arial, Helvetica, sans-serif',
    background: theme.mainEditorPane,
    border: '1px solid rgba(34, 36, 38, 0.15)',
    boxShadow: '0 1px 2px 0 rgba(34, 36, 38, 0.15)',
    minHeight: 0,
    margin: 0,
    borderRadius: 0,
    flexShrink: 0,
    '& *, & *::before, & *::after': {
      boxSizing: 'inherit',
    },
  },
  item: {
    'position': 'relative',
    'display': 'flex',
    'alignItems': 'center',
    'verticalAlign': 'middle',
    'flex': '0 0 auto',
    'userSelect': 'none',
    'background': 'none',
    'padding': '0.92857143em 1.14285714em',
    'color': theme.fgMain,
    'fontSize': '12px',
    'fontWeight': 'normal',
    'textDecoration': 'none',
    'textTransform': 'none',
    'WebkitTapHighlightColor': 'transparent',
    'transition': 'background 0.1s ease, box-shadow 0.1s ease, color 0.1s ease',
    'height': '1em',
    'lineHeight': '1em',
    '&::before': {
      position: 'absolute',
      content: '\'\'',
      top: 0,
      right: 0,
      height: '100%',
      width: '1px',
      background: 'rgba(34, 36, 38, 0.1)',
    },
  },
  button: {
    'cursor': 'pointer',
    '&:hover': {
      background: theme.toolbarBtnHoverBg,
      color: theme.fgMain,
    },
  },
  active: {
    background: theme.tabActiveBg,
    color: theme.fgMain,
  },
  inverted: {
    'borderWidth': '2px',
    'borderTopWidth': 0,
    '& > $item': {
      marginRight: '1px',
    },
  },
  device: {
    '& > $item': {
      borderRadius: 0,
    },
    '& > $item$active': {
      borderBottomLeftRadius: '4px',
      borderBottomRightRadius: '4px',
    },
  },
  subFilter: {
    'borderWidth': '0 !important',
    '& > $item > svg': {
      marginRight: '0.25em',
      verticalAlign: 'middle',
    },
    '& > $item:first-child': {
      borderTopLeftRadius: '4px',
      borderBottomLeftRadius: '4px',
    },
    '& > $item:last-child': {
      borderTopRightRadius: '4px',
      borderBottomRightRadius: '4px',
    },
  },
  tabButton: {
    color: 'inherit',
    cursor: 'pointer',
    display: 'flex',
    alignItems: 'center',
  },
  closeTab: {
    'margin': '0 0 0 0.5em',
    '& > button': {
      color: 'var(--text-color) !important',
      padding: 0,
      lineHeight: '0.75em',
      boxShadow: 'none',
    },
    '& > button:hover': {
      color: 'var(--text-inverted-color) !important',
    },
  },
}))

type ConsoleMenuVariant = 'default' | 'device' | 'subFilter'

interface IConsoleMenu extends React.HTMLAttributes<HTMLDivElement> {
  children?: React.ReactNode
  inverted?: boolean
  variant?: ConsoleMenuVariant
}

interface IConsoleMenuItem extends React.HTMLAttributes<HTMLDivElement> {
  active?: boolean
}

interface IConsoleMenuButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  active?: boolean
}

const ConsoleMenu: React.FC<IConsoleMenu> = ({
  className, children, inverted = false, variant = 'default', ...rest
}) => {
  const classes = useStyles()
  return (
    <div
      {...rest}
      className={combine(
        classes.menu,
        inverted && classes.inverted,
        variant !== 'default' && classes[variant],
        className
      )}
    >
      {children}
    </div>
  )
}

const ConsoleMenuItem: React.FC<IConsoleMenuItem> = ({
  active = false, className, children, ...rest
}) => {
  const classes = useStyles()
  return (
    <div {...rest} className={combine(classes.item, active && classes.active, className)}>
      {children}
    </div>
  )
}

const ConsoleMenuButton: React.FC<IConsoleMenuButton> = ({
  active = false, className, ...rest
}) => {
  const classes = useStyles()
  return (
    <button
      {...rest}
      type='button'
      className={combine(
        'style-reset', classes.item, classes.button, active && classes.active, className
      )}
    />
  )
}

const ConsoleMenuTabButton: React.FC<React.ButtonHTMLAttributes<HTMLButtonElement>> = ({
  className, ...rest
}) => {
  const classes = useStyles()
  return (
    <button
      {...rest}
      type='button'
      className={combine('style-reset', classes.tabButton, className)}
    />
  )
}

const ConsoleMenuCloseTab: React.FC<React.HTMLAttributes<HTMLSpanElement>> = ({
  className, ...rest
}) => {
  const classes = useStyles()
  return <span {...rest} className={combine(classes.closeTab, className)} />
}

export {
  ConsoleMenu,
  ConsoleMenuButton,
  ConsoleMenuCloseTab,
  ConsoleMenuItem,
  ConsoleMenuTabButton,
}
