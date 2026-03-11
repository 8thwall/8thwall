import React from 'react'

import {bool, combine} from '../../common/styles'
import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import type {UiTheme} from '../theme'

const useStyles = createCustomUseStyles<{size: string}>()((theme: UiTheme) => ({
  '@keyframes spinAnimation': {
    'from': {transform: 'rotate(0deg)'},
    'to': {transform: 'rotate(360deg)'},
  },
  '@keyframes borderColorCycle': {
    '0%': {borderTopColor: '#BA4FFF'},
    '33%': {borderTopColor: '#FF4713'},
    '66%': {borderTopColor: '#FFC828'},
    '100%': {borderTopColor: '#BA4FFF'},
  },
  'loader': {
    'maxWidth': '100%',
    'zIndex': '1000',
    'fontSize': theme.loaderFontSize,
    'color': theme.loaderFontColor,
  },
  'spinner': {
    'position': 'absolute',
    'top': '50%',
    'left': '50%',
    'transform': 'translate(-50%, -50%)',
  },
  'spinnerInner': {
    'display': 'block',
    'height': ({size}) => size,
    'width': ({size}) => size,
    'background': theme.loaderSecondary,
    'animation': '$spinAnimation linear infinite 0.8s',
    '&:before': {
      content: '""',
      display: 'block',
      width: '100%',
      height: '100%',
      background: theme.loaderPrimary,
    },
  },
  // These were exported out of Figma.
  /* eslint-disable max-len */
  'tiny': {
    'clipPath': 'path(\'M 16 8 C 16 12.4183 12.4183 16 8 16 C 3.5817 16 0 12.4183 0 8 C 0 3.5817 3.5817 0 8 0 C 12.4183 0 16 3.5817 16 8 Z M 3.2 8 C 3.2 10.651 5.349 12.8 8 12.8 C 10.651 12.8 12.8 10.651 12.8 8 C 12.8 5.349 10.651 3.2 8 3.2 C 5.349 3.2 3.2 5.349 3.2 8 Z\')',
    '&:before': {
      'clipPath': 'path(\'M 11.0589 2.4339 C 11.4951 1.6654 11.2275 0.6714 10.3877 0.3966 C 9.5747 0.1306 8.721 -0.0043 7.8587 0.0001 C 6.4605 0.0073 5.0886 0.3809 3.8797 1.0835 C 2.6709 1.7862 1.6673 2.7934 0.969 4.0048 C 0.5384 4.7519 0.2332 5.5605 0.0619 6.3986 C -0.115 7.2644 0.6164 7.9888 1.5 7.9902 C 2.3837 7.9915 3.0762 7.2571 3.3666 6.4225 C 3.4652 6.1392 3.5905 5.8648 3.7414 5.6029 C 4.1604 4.8761 4.7625 4.2717 5.4878 3.8501 C 6.2131 3.4285 7.0363 3.2044 7.8752 3.2001 C 8.1775 3.1985 8.478 3.2255 8.7729 3.28 C 9.6418 3.4407 10.6228 3.2024 11.0589 2.4339 Z\')',
    },
  },
  'small': {
    'clipPath': 'path(\'M 24 12.0333 C 24 18.6607 18.6274 24.0333 12 24.0333 C 5.3726 24.0333 0 18.6607 0 12.0333 C 0 5.4058 5.3726 0.0333 12 0.0333 C 18.6274 0.0333 24 5.4058 24 12.0333 Z M 3 12.0333 C 3 17.0038 7.0294 21.0333 12 21.0333 C 16.9706 21.0333 21 17.0038 21 12.0333 C 21 7.0627 16.9706 3.0332 12 3.0332 C 7.0294 3.0332 3 7.0627 3 12.0333 Z\')',
    '&:before': {
      'clipPath': 'path(\'M 17.0826 2.9014 C 17.4915 2.1809 17.2415 1.2557 16.4756 0.9398 C 15.0097 0.3352 13.4337 0.0252 11.8381 0.0334 C 9.7407 0.0442 7.6829 0.6046 5.8696 1.6586 C 4.0563 2.7126 2.5509 4.2234 1.5035 6.0405 C 0.7067 7.4229 0.1961 8.9458 -0.0041 10.5188 C -0.1086 11.3406 0.5716 12.0158 1.4 12.0171 C 2.2284 12.0184 2.8883 11.3433 3.027 10.5265 C 3.205 9.4786 3.5679 8.4664 4.1027 7.5387 C 4.8882 6.1759 6.0172 5.0427 7.3772 4.2522 C 8.7372 3.4617 10.2806 3.0415 11.8536 3.0334 C 12.9244 3.0279 13.9835 3.2135 14.9822 3.5775 C 15.7605 3.8612 16.6737 3.6219 17.0826 2.9014 Z\')',
    },
  },
  'medium': {
    'clipPath': 'path(\'M 40 20.0166 C 40 31.0623 31.0457 40.0166 20 40.0166 C 8.9543 40.0166 0 31.0623 0 20.0166 C 0 8.9709 8.9543 0.0166 20 0.0166 C 31.0457 0.0166 40 8.9709 40 20.0166 Z M 4 20.0166 C 4 28.8532 11.1634 36.0166 20 36.0166 C 28.8366 36.0166 36 28.8532 36 20.0166 C 36 11.1801 28.8366 4.0166 20 4.0166 C 11.1634 4.0166 4 11.1801 4 20.0166 Z\')',
    '&:before': {
      'clipPath': 'path(\'M 28.7845 4.4121 C 29.3297 3.4514 28.9961 2.2206 27.9858 1.774 C 25.4139 0.6371 22.6237 0.0523 19.7968 0.0669 C 16.3012 0.0849 12.8715 1.0188 9.8493 2.7755 C 6.8272 4.5321 4.3182 7.0502 2.5726 10.0787 C 1.1608 12.5279 0.288 15.2418 0.003 18.0393 C -0.109 19.1382 0.7955 20.0372 1.9 20.0389 C 3.0046 20.0406 3.8885 19.1431 4.0279 18.0474 C 4.2948 15.95 4.9756 13.9195 6.038 12.0763 C 7.4346 9.6535 9.4418 7.639 11.8595 6.2337 C 14.2772 4.8284 17.021 4.0813 19.8174 4.0668 C 21.9449 4.0559 24.0463 4.4692 26.0008 5.2754 C 27.0219 5.6966 28.2393 5.3727 28.7845 4.4121 Z\')',
    },
  },
  'large': {
    'clipPath': 'path(\'M 56.032 27.968 C 56.032 43.432 43.496 55.968 28.032 55.968 C 12.568 55.968 0.032 43.432 0.032 27.968 C 0.032 12.504 12.568 -0.032 28.032 -0.032 C 43.496 -0.032 56.032 12.504 56.032 27.968 Z M 4.131 27.968 C 4.131 41.1682 14.8318 51.869 28.032 51.869 C 41.2322 51.869 51.933 41.1682 51.933 27.968 C 51.933 14.7678 41.2322 4.067 28.032 4.067 C 14.8318 4.067 4.131 14.7678 4.131 27.968 Z\')',
    '&:before': {
      'clipPath': 'path(\'M 40.7049 5.4377 C 41.2657 4.4495 40.922 3.1863 39.8953 2.6996 C 36.1102 0.9049 31.962 -0.0213 27.7555 0.0004 C 22.8618 0.0256 18.0601 1.3331 13.8291 3.7924 C 9.5981 6.2517 6.0855 9.777 3.6416 14.0169 C 1.5408 17.6613 0.2927 21.7242 -0.0213 25.9015 C -0.1064 27.0345 0.8211 27.9584 1.9574 27.9601 C 3.0936 27.9619 4.007 27.0404 4.1064 25.9085 C 4.4103 22.4512 5.465 19.0929 7.2064 16.0717 C 9.2913 12.4549 12.2876 9.4477 15.8968 7.3498 C 19.5061 5.2519 23.6021 4.1366 27.7767 4.115 C 31.2638 4.097 34.704 4.8427 37.8586 6.2898 C 38.8914 6.7635 40.144 6.4259 40.7049 5.4377 Z\')',
    },
  },
  /* eslint-enable max-len */
  'spinnerColorAnim': {
    '&:before': {
      background: '#BA4FFF',
      animation:
        '$spinAnimation linear infinite 0.8s, $borderColorCycle 4s linear infinite',
    },
  },
  'inverted': {
    'background': theme.loaderPrimary,
    '&:before': {
      background: theme.loaderSecondary,
    },
  },
  'inline': {
    'display': 'inline-flex',
    'gap': theme.loaderGap,
    'flexDirection': 'column',
    'alignItems': 'center',
    'position': 'relative',
    'verticalAlign': 'middle',
    'transform': 'none',
    'top': '0',
    'left': '0',
    'margin': '0',

    '& $spinner': {
      display: 'block',
      position: 'static',
      transform: 'none',
    },
  },
  'centered': {
    'display': 'flex',
    'marginLeft': 'auto',
    'marginRight': 'auto',
  },
  'children': {
    position: 'absolute',
    maxWidth: '100%',
    top: ({size}) => `calc(50% + ${size} / 2 + 6px)`,
    left: '50%',
    transform: 'translateX(-50%)',
  },
}))

const spinnerSizing = {
  'tiny': '16px',
  'small': '24px',
  'medium': '40px',
  'large': '56px',
}
type ButtonSizes = keyof typeof spinnerSizing
interface ILoader {
  size?: ButtonSizes
  className?: string
  inline?: boolean
  centered?: boolean
  children?: React.ReactNode
  animateSpinnerColor?: boolean
  inverted?: boolean
}

const Loader: React.FC<ILoader> = ({
  size = 'medium', className, centered, inline, children, animateSpinnerColor, inverted,
}) => {
  const classes = useStyles({size: spinnerSizing[size]})

  return (
    <div
      className={combine(classes.loader,
        className,
        inline && classes.inline,
        centered && classes.centered)}
    >
      <div className={classes.spinner}>
        <span className={combine(classes.spinnerInner,
          classes[size],
          inverted && classes.inverted,
          animateSpinnerColor && classes.spinnerColorAnim)}
        />
      </div>
      {children && <div className={bool(!inline, classes.children)}>{children}</div>}
    </div>
  )
}

export {
  Loader,
}

export type {
  ILoader,
}
