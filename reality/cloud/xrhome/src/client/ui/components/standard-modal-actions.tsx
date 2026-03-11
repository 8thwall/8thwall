import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  standardModalActions: {
    display: 'flex',
    justifyContent: 'flex-end',
    gap: '1rem',
    padding: '1.2rem',
    flexWrap: 'wrap',
  },
  split: {
    justifyContent: 'space-between',
  },
  alignLeft: {
    justifyContent: 'flex-start',
  },
  center: {
    justifyContent: 'center',
  },
})

interface IStandardModalActions {
  children?: React.ReactNode
  // split: For actions on both left and right corners of the modal
  align?: 'center' | 'left' | 'right' | 'split'
}

const StandardModalActions: React.FC<IStandardModalActions> = ({children, align = 'right'}) => {
  const classes = useStyles()
  return (
    <div className={combine(
      classes.standardModalActions,
      align === 'split' && classes.split,
      align === 'left' && classes.alignLeft,
      align === 'center' && classes.center
    )}
    >
      {children}
    </div>
  )
}

export {StandardModalActions}
