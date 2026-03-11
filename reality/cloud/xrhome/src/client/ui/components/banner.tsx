import React from 'react'
import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {Icon} from './icon'
import {IconButton} from './icon-button'

const useStyles = createThemedStyles(theme => ({
  banner: {
    display: 'flex',
    gap: '0.5rem',
    alignItems: 'flex-start',
    padding: '1rem',
    fontFamily: 'inherit',
    lineHeight: '20px',
    borderRadius: theme.bannerBorderRadius,
  },
  hasMarginTop: {
    marginTop: '1rem',
  },
  info: {
    color: theme.bannerFgInfo,
    border: theme.bannerBorderInfo,
    background: theme.bannerBgInfo,
  },
  warning: {
    color: theme.bannerFgWarning,
    border: theme.bannerBorderWarning,
    background: theme.bannerBgWarning,
  },
  success: {
    color: theme.bannerFgSuccess,
    border: theme.bannerBorderSuccess,
    background: theme.bannerBgSuccess,
  },
  danger: {
    color: theme.bannerFgDanger,
    border: theme.bannerBorderDanger,
    background: theme.bannerBgDanger,
  },
  icon: {
    position: 'relative',
    top: '4px',
  },
  closeIcon: {
    flexGrow: '1',
    display: 'flex',
    justifyContent: 'flex-end',
    color: theme.bannerCloseIconColor,
  },
  msgContainer: {
    display: 'flex',
    alignItems: 'baseline',
    gap: '0.5rem',
  },
}))

interface IBanner {
  type: 'info' | 'warning' | 'success' | 'danger'
  className?: string
  message?: string
  hasMarginTop?: boolean
  children?: React.ReactNode
  onClose?: () => void
  autoClose?: number  // in milliseconds
}

const StaticBanner: React.FC<IBanner> = (
  {type, className, message, hasMarginTop, children, onClose, autoClose}
) => {
  const {t} = useTranslation(['common'])
  const classes = useStyles()
  const autoCloseTimeoutRef = React.useRef(null)

  React.useEffect(() => {
    if (autoClose && onClose) {
      autoCloseTimeoutRef.current = setTimeout(onClose, autoClose)
    }
    return () => {
      clearTimeout(autoCloseTimeoutRef.current)
    }
  }, [])

  return (
    <div
      className={combine(
        classes.banner,
        classes[type],
        hasMarginTop && classes.hasMarginTop,
        className
      )}
    >
      <div className={classes.msgContainer}>
        <span className={classes.icon}>
          <Icon
            stroke={type}
            color={type}
          />
        </span>
        {message || children}
      </div>
      {onClose &&
        <span className={classes.closeIcon}>
          <IconButton
            stroke='cancel'
            onClick={onClose}
            text={t('button.close')}
          />
        </span>
      }
    </div>
  )
}

export {
  StaticBanner,
}

export type {
  IBanner,
}
