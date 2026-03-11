import React from 'react'

import {combine} from '../../common/styles'

import {createThemedStyles, UiThemeProvider, useUiTheme} from '../theme'

const useStyles = createThemedStyles(theme => ({
  popupWrapper: {
    'position': 'relative',
    // Note(Dale): if we have a child element that is aria-expanded, we don't want to show the popup
    // content
    '&:not(:has([aria-expanded="true"]))': {
      '--popup-display': 'block',
    },
  },
  popupContent: {
    display: 'var(--popup-display, none)',
    wordWrap: 'break-word',
    position: 'absolute',
    zIndex: 3,
    backgroundColor: theme.tooltipBg,
    borderRadius: '0.2rem',
    width: 'max-content',
    maxWidth: '22rem',
    fontSize: '14px',
    fontWeight: '400',
    color: theme.fgMain,
    boxShadow: '2px 2px 5px rgba(0, 0, 0, 0.2)',
    userSelect: 'none',
  },
  medium: {
    padding: '1.25rem',
  },
  small: {
    padding: '0.75rem',
  },
  tiny: {
    padding: '0.5rem',
  },
  arrow: {
    display: 'var(--popup-display, none)',
    content: '""',
    position: 'absolute',
    transform: 'rotate(45deg)',
    backgroundColor: theme.tooltipBg,
    padding: '0.4rem',
    zIndex: 4,
  },
  arrowCenterX: {
    left: 'calc(50% - 0.4rem)',
  },
  arrowCenterY: {
    top: 'calc(50% - 0.4rem)',
  },
  top: {
    bottom: '100%',
    margin: '0 -0.5rem 0.5rem -0.5rem',
  },
  bottom: {
    top: '100%',
    margin: '0.5rem -0.5rem 0 -0.5rem',
  },
  left: {
    right: 'calc(100% + 0.8rem)',
  },
  right: {
    left: 'calc(100% + 0.8rem)',
  },
  arrowTop: {
    top: '-1rem',
  },
  arrowBottom: {
    bottom: '-1rem',
  },
  arrowLeft: {
    left: '-1.2rem',
  },
  arrowRight: {
    right: '-1.2rem',
  },
  leftAlign: {
    left: 0,
  },
  rightAlign: {
    right: 0,
  },
  centerX: {
    left: '50%',
    transform: 'translateX(calc(-50% + 0.5rem))',
  },
  centerY: {
    top: '50%',
    transform: 'translateY(-50%)',
  },
  hidden: {
    visibility: 'hidden',
  },
}))

type PopupPosition = 'top' | 'right' | 'bottom' | 'left'
type PopupAlignment = 'left' | 'center' | 'right'

interface IPopupContent {
  content: React.ReactNode
  position?: PopupPosition
  alignment?: PopupAlignment
  popupDisabled?: boolean
  size?: 'tiny' | 'small' | 'medium'
}

interface IPopup extends IPopupContent {
  delay?: number
  children?: React.ReactNode
}

const PopupContent: React.FC<IPopupContent> = ({
  content, position = 'top', alignment = 'left', popupDisabled = false, size = 'medium',
}) => {
  const classes = useStyles()

  const disabled = popupDisabled && classes.hidden

  const getPositionStyles = () => {
    let contentPlacement
    let arrowPlacement
    switch (position) {
      case 'left':
        contentPlacement = classes.left
        arrowPlacement = classes.arrowLeft
        break
      case 'right':
        contentPlacement = classes.right
        arrowPlacement = classes.arrowRight
        break
      case 'bottom':
        contentPlacement = classes.bottom
        arrowPlacement = classes.arrowBottom
        break
      default:
        contentPlacement = classes.top
        arrowPlacement = classes.arrowTop
    }

    const centerX = position === 'top' || position === 'bottom'

    let contentAlignment = classes.centerY
    if (centerX) {
      switch (alignment) {
        case 'left':
          contentAlignment = classes.leftAlign
          break
        case 'right':
          contentAlignment = classes.rightAlign
          break
        default:
          contentAlignment = classes.centerX
      }
    }

    const arrowCenter = centerX ? classes.arrowCenterX : classes.arrowCenterY

    return [combine(contentPlacement, contentAlignment), combine(arrowPlacement, arrowCenter)]
  }

  const [contentStyles, arrowStyles] = getPositionStyles()

  return (
    <>
      <div className={combine(classes.arrow, arrowStyles, disabled)} />
      <span className={combine(classes.popupContent, contentStyles, disabled, classes[size])}>
        {content}
      </span>
    </>
  )
}

const Popup: React.FC<IPopup> = ({
  delay = 0, children, ...rest
}) => {
  const classes = useStyles()
  const theme = useUiTheme()
  // NOTE(Julie): tooltip is "opposite" of current theme, specified by tooltipInverted

  const [showPopup, setShowPopup] = React.useState(false)
  const popupTimeout = React.useRef(null)

  const handleShowPopup = () => {
    clearTimeout(popupTimeout.current)
    if (!delay) {
      setShowPopup(true)
      return
    }
    popupTimeout.current = setTimeout(() => {
      setShowPopup(true)
    }, delay)
  }

  const handleHidePopup = () => {
    clearTimeout(popupTimeout.current)
    setShowPopup(false)
  }

  return (
    // eslint-disable-next-line jsx-a11y/no-static-element-interactions
    <span
      className={classes.popupWrapper}
      onMouseEnter={handleShowPopup}
      onMouseLeave={handleHidePopup}
      onFocus={handleShowPopup}
      onBlur={handleHidePopup}
      onMouseDown={handleHidePopup}
    >
      {children}
      <UiThemeProvider mode={theme.tooltipInverted ? 'dark' : 'light'}>
        {showPopup && <PopupContent {...rest} />}
      </UiThemeProvider>
    </span>
  )
}

export {
  Popup,
}

export type {
  IPopup,
  IPopupContent,
}
