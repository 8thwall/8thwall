import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../common/styles'

const useStyles = createUseStyles({
  standardModalActions: {
    display: 'flex',
    justifyContent: 'center',
    gap: '1rem',
    padding: '1.5rem',
    flexWrap: 'wrap',
  },
  split: {
    justifyContent: 'space-between',
  },
  alignLeft: {
    justifyContent: 'flex-start',
  },
  alignRight: {
    justifyContent: 'flex-end',
  },
})

interface IStandardModalActions {
  children?: React.ReactNode
  // split: For actions on both left and right corners of the modal
  align?: 'center' | 'left' | 'right' | 'split'
}

const StandardModalActions: React.FC<IStandardModalActions> = ({children, align = 'center'}) => {
  const classes = useStyles()
  return (
    <div className={combine(
      classes.standardModalActions,
      align === 'split' && classes.split,
      align === 'left' && classes.alignLeft,
      align === 'right' && classes.alignRight
    )}
    >
      {children}
    </div>
  )
}

export {StandardModalActions}
