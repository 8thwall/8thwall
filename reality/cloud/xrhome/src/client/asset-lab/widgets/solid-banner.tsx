import React from 'react'

import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {cherryLight, gray2, mango, mint} from '../../static/styles/settings'

const useStyles = createThemedStyles(theme => ({
  staticBanner: {
    'borderRadius': '0.5rem',
    'padding': '0.75rem 0.625rem',
    'alignItems': 'center',
    'display': 'flex',
    'flexDirection': 'row',
    'alignSelf': 'flex-start',
    'border': `1px solid ${theme.sfcBorderDefault}`,
    '&.info': {
      color: theme.fgMuted,
      background: theme.sfcBackgroundDefault,
    },
    '&.warning': {
      color: mango,
      borderColor: mango,
      background: `${mango}${theme.bannerBgOpacityHex}`,
    },
    '&.success': {
      color: theme.fgSuccess,
      borderColor: mint,
      background: `${mint}${theme.bannerBgOpacityHex}`,
    },
    '&.danger': {
      color: theme.fgError,
      borderColor: cherryLight,
      background: `${cherryLight}${theme.bannerBgOpacityHex}`,
    },
  },
  indicatorIcon: {
    'flex': '0 0 auto',
    'marginRight': '0.5rem',
    '&.info': {
      color: gray2,
    },
    '&.warning': {
      color: mango,
    },
    '&.success': {
      color: mint,
    },
    '&.danger': {
      color: cherryLight,
    },
  },
  message: {
    flex: '1 1 auto',
  },
}))

interface ISolidBanner {
  type: 'info' | 'warning' | 'success' | 'danger'
  message: string
}

const SolidMessageBanner: React.FC<ISolidBanner> = ({type, message}) => {
  const classes = useStyles()
  return (
    <div className={combine(type, classes.staticBanner)}>
      <div className={combine(type, classes.indicatorIcon)}>
        <Icon inline stroke={type} />
      </div>
      <div className={classes.message}>{message}</div>
    </div>
  )
}

export {SolidMessageBanner}
