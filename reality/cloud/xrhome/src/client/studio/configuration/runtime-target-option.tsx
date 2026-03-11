import React from 'react'

import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  description: {
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    overflow: 'hidden',
  },
  time: {
    color: theme.fgMuted,
    fontStyle: 'italic',
  },
}))

interface IRuntimeTargetOption {
  selected: boolean
  description: React.ReactNode
  rightContent?: React.ReactNode
}

const RuntimeTargetOption: React.FC<IRuntimeTargetOption> = ({
  selected, description, rightContent,
}) => {
  const classes = useStyles()
  return (
    <div>
      <div className={classes.description}>
        {description}
      </div>
      {!selected &&
        <div className={classes.time}>
          {rightContent}
        </div>
      }
    </div>
  )
}

export {RuntimeTargetOption}
