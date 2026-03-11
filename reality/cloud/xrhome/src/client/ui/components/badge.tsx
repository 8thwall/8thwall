import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const BADGE_COLORS = ['purple', 'blue', 'mango', 'mint', 'cherry', 'gray'] as const
const BADGE_VARIANTS = ['pastel', 'solid', 'outlined'] as const
type BadgeColor = typeof BADGE_COLORS[number]
type BadgeVariant = typeof BADGE_VARIANTS[number]
type BadgeHeight = 'small' | 'tiny' | 'micro'
type BadgeSpacing = 'normal' | 'full'

const useStyles = createThemedStyles(theme => ({
  badge: {
    display: 'inline-flex',
    alignItems: 'center',
    justifyContent: 'center',
    fontFamily: 'inherit',
    fontSize: '12px',
    fontWeight: 400,
    borderRadius: theme.badgeBorderRadius,
    whiteSpace: 'pre',
    lineHeight: '16px',
    fontStyle: 'normal',
    userSelect: 'none',
  },
  small: {
    height: '24px',
    padding: '2px 8px',
    borderRadius: '4px',
  },
  tiny: {
    height: '18px',
    padding: '1px 8px',
  },
  micro: {
    height: '16px',
    padding: '0px 4px',
  },
  full: {
    width: '100%',
  },
}))

const capitalize = (s: string) => s.charAt(0).toUpperCase() + s.slice(1)
interface IBadge {
  color?: BadgeColor
  variant?: BadgeVariant
  height?: BadgeHeight
  spacing?: BadgeSpacing
  title?: string
  children?: React.ReactNode
}

const useColorStyles = createThemedStyles((theme) => {
  const styles = {}
  BADGE_COLORS.forEach((color) => {
    // Outlined variant
    switch (color) {
      case 'mint':
      case 'mango':
      case 'gray':
      case 'purple':
        styles[`${color}Outlined`] = {
          color: theme[`badge${capitalize(color)}OutlinedColor`],
          background: 'transparent',
          boxShadow: `0 0 0 1px ${theme[`badge${capitalize(color)}OutlinedColor`]} inset`,
        }
        break
      default:
        styles[`${color}Outlined`] = {
          color: theme[`badge${capitalize(color)}Color`],
          background: 'transparent',
          boxShadow: `0 0 0 1px ${theme[`badge${capitalize(color)}Color`]} inset`,
        }
    }

    // Pastel variant
    styles[`${color}Pastel`] = {
      color: theme[`badge${capitalize(color)}PastelFgColor`],
      background: theme[`badge${capitalize(color)}PastelBgColor`],
    }

    // Solid variant
    switch (color) {
      case 'mango':
      case 'gray':
        styles[`${color}Solid`] = {
          color: theme[`badge${capitalize(color)}SolidFgColor`],
          background: theme[`badge${capitalize(color)}Color`],
        }
        break
      default:
        styles[`${color}Solid`] = {
          color: theme.badgeSolidFg,
          background: theme[`badge${capitalize(color)}Color`],
        }
    }
  })
  return styles
})

const Badge: React.FC<IBadge> = ({
  children, color = 'gray', variant = 'solid', height = 'tiny', spacing = 'normal',
  title,
}) => {
  const classes = useStyles()
  const colorClasses = useColorStyles()

  return (
    <span
      className={combine(
        classes.badge,
        colorClasses[`${color}${capitalize(variant)}`],
        classes[height],
        classes[spacing]
      )}
      title={title}
    >{children}
    </span>
  )
}

export {
  Badge,
  BADGE_COLORS,
  BADGE_VARIANTS,
}

export type {
  IBadge,
  BadgeColor,
  BadgeHeight,
  BadgeSpacing,
  BadgeVariant,
}
