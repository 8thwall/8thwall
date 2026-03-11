import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  playButtonTray: {
    borderRadius: '0.5em',
    overflow: 'hidden',
    background: `${theme.studioBgMain}`,
    pointerEvents: 'auto',
    backdropFilter: 'blur(50px)',
    border: theme.studioBgBorder,
    color: theme.studioBtnHoverFg,
  },
  horizontal: {
    display: 'flex',
    flexDirection: 'row',
  },
  vertical: {
    display: 'flex',
    flexDirection: 'column',
  },
  scrollable: {
    'overflow': 'auto',
  },
  showScrollbar: {
    overflowY: 'scroll',
  },
  fillContainer: {
    flex: 1,
  },
  opaque: {
    background: theme.studioBgOpaque,
  },
  shrink: {
    minWidth: 0,
  },
  nonInteractive: {
    pointerEvents: 'none',
    userSelect: 'none',
  },
  overflowHidden: {
    overflow: 'hidden',
  },
}))

interface IFloatingTray {
  id?: string
  children: React.ReactNode
  orientation?: 'horizontal' | 'vertical'
  isScrollable?: boolean
  fillContainer?: boolean
  isOpaque?: boolean
  shrink?: boolean
  reserveSpaceForScroll?: boolean
  nonInteractive?: boolean
  overflowHidden?: boolean
}

const FloatingTray: React.FC<IFloatingTray> = ({
  children, orientation = 'horizontal', isScrollable = false, fillContainer = false,
  isOpaque = false, shrink = false, reserveSpaceForScroll = false, nonInteractive = false,
  overflowHidden = false, id: idOverride,
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  return (
    <div className={combine(classes.playButtonTray,
      orientation === 'vertical' ? classes.vertical : classes.horizontal,
      fillContainer && classes.fillContainer,
      shrink && classes.shrink)}
    >
      <div
        className={combine(
          orientation === 'vertical' ? classes.vertical : classes.horizontal,
          fillContainer && classes.fillContainer,
          isScrollable && classes.scrollable,
          reserveSpaceForScroll && classes.showScrollbar,
          isOpaque && classes.opaque,
          nonInteractive && classes.nonInteractive,
          overflowHidden && classes.overflowHidden
        )}
        id={id}
      >
        {children}
      </div>
    </div>
  )
}

export {
  FloatingTray,
}
