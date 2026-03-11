import React from 'react'
import type {Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import {
  Badge, BADGE_VARIANTS, BadgeHeight, BadgeSpacing, BADGE_COLORS,
} from '../components/badge'
import {SpaceBetween} from '../layout/space-between'

const useStyles = createUseStyles({
  palette: {
    display: 'grid',
    gridTemplateColumns: 'repeat(6, minmax(calc(var(--grid-size) * 1px), 1fr))',
    gridGap: '1rem',
    justifyItems: 'center',
  },
})

type BadgeShowCaseProps = {badgeSize: string, badgeHeight: BadgeHeight, badgeSpacing: BadgeSpacing}

const BadgeShowCase: React.FC<BadgeShowCaseProps> = ({badgeHeight, badgeSpacing, badgeSize}) => {
  const classes = useStyles()
  return (
    <SpaceBetween direction='vertical'>
      <div className={classes.palette} style={{'--grid-size': badgeSize} as any}>
        {BADGE_VARIANTS.flatMap(variant => BADGE_COLORS.map(color => (
          <Badge
            key={`${color}-${variant}`}
            color={color}
            variant={variant}
            height={badgeHeight}
            spacing={badgeSpacing}
          >
            {color} - {variant}
          </Badge>
        )))}
      </div>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/BadgeShowCase',
  render: BadgeShowCase,
  argTypes: {
    badgeSize: {
      control: 'select',
      options: ['64', '160', '400'],
    },
    badgeHeight: {
      control: 'select',
      options: ['tiny', 'small', 'micro'],
    },
    badgeSpacing: {
      control: 'select',
      options: ['normal', 'full'],
    },
  },
} as Meta<typeof BadgeShowCase>

export const All = {
  args: {
    badgeSize: '160',
    badgeHeight: 'tiny',
    badgeSpacing: 'normal',
  },
}
