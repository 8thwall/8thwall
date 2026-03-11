import React from 'react'

import {Icon, IconStroke, IconColor} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'

interface HeadlineRowProps {
  iconStroke: IconStroke
  iconSize: number
  iconColor: IconColor
  backgroundColor: string
  text: string
}

const useStyles = createThemedStyles(theme => ({
  headline: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '0.5rem',
    alignSelf: 'stretch',
    paddingBottom: '1rem',
  },
  headlineIcon: {
    display: 'flex',
    width: '1.5rem',
    height: '1.5rem',
    padding: '0.25rem',
    justifyContent: 'center',
    alignItems: 'center',
    gap: '0.75rem',
    aspectRato: '1/1',
    borderRadius: '0.25rem',
  },
  headlineText: {
    display: 'flex',
    fontStyle: 'normal',
    color: theme.publishModalText,
  },
}))

const HeadlineRow: React.FC<HeadlineRowProps> = ({
  iconStroke,
  iconSize,
  iconColor,
  backgroundColor,
  text,
}) => {
  const classes = useStyles()

  return (
    <div className={classes.headline}>
      <div className={classes.headlineIcon} style={{backgroundColor}}>
        <Icon
          stroke={iconStroke}
          size={iconSize}
          color={iconColor}
        />
      </div>
      <span className={classes.headlineText}>
        {text}
      </span>
    </div>
  )
}

export {HeadlineRow}
