import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const ROUNDED_BUTTON_SMALL_HEIGHT_IN_PX = '32px'

const useStyles = createThemedStyles(theme => ({
  roundedButton: {
    'lineHeight': 'normal',
    'fontFamily': 'inherit',
    'boxSizing': 'border-box',
    'fontSize': '14px',
    'fontWeight': theme.roundedBtnFontWeight,
    'borderRadius': theme.roundedBtnBorderRadius,
    'border': 'none',
    'margin': 0,
    'cursor': 'pointer',
    '&:disabled': {
      cursor: 'default',
    },
  },
  medium: {
    height: theme.roundedBtnMediumHeight,
    padding: theme.roundedBtnMediumPadding,
  },
  small: {
    height: ROUNDED_BUTTON_SMALL_HEIGHT_IN_PX,
    padding: theme.roundedBtnSmallPadding,
    borderRadius: theme.roundedBtnSmallBorderRadius,
  },
  tiny: {
    height: '24px',
    padding: theme.roundedBtnTinyPadding,
    fontSize: '12px',
    borderRadius: theme.roundedBtnTinyBorderRadius,
  },
  mediumShort: {
    extend: 'medium',
    padding: '9px 8px',
  },
  smallShort: {
    extend: 'small',
    padding: '6px 6px',
  },
  tinyShort: {
    extend: 'tiny',
    padding: '4px 4px',
  },
  mediumWide: {
    extend: 'medium',
    padding: '9px 39px',
  },
  smallWide: {
    extend: 'small',
    padding: '6px 26px',
  },
  tinyWide: {
    extend: 'tiny',
    padding: '4px 19px',
  },
  mediumFull: {
    extend: 'medium',
    padding: '9px 8px',
    width: '100%',
  },
  smallFull: {
    extend: 'small',
    padding: '6px 6px',
    width: '100%',
  },
  tinyFull: {
    extend: 'tiny',
    padding: '4px 4px',
    width: '100%',
  },
}))

type ButtonHeight = 'medium' | 'small' | 'tiny'
type ButtonSpacing = 'wide' | 'normal' | 'short' | 'full'

const getClassName = (height: ButtonHeight, spacing: ButtonSpacing) => (
  spacing === 'normal' ? height : `${height}${spacing[0].toUpperCase()}${spacing.slice(1)}`
)

const useRoundedButtonStyling = (height: ButtonHeight, spacing: ButtonSpacing) => {
  const classes = useStyles()
  return combine(classes.roundedButton, classes[getClassName(height, spacing)])
}

export {
  useRoundedButtonStyling,
  ROUNDED_BUTTON_SMALL_HEIGHT_IN_PX,
}

export type {
  ButtonHeight,
  ButtonSpacing,
}
