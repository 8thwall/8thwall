import React from 'react'
import {createUseStyles} from 'react-jss'

const useStyles = createUseStyles({
  standardModalHeader: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    padding: '1.2rem',
  },
})

interface IStandardModalHeader {
  children: React.ReactNode
}

const StandardModalHeader: React.FC<IStandardModalHeader> = ({children}) => {
  const classes = useStyles()
  return (
    <div className={classes.standardModalHeader}>
      {children}
    </div>
  )
}

export {StandardModalHeader}
