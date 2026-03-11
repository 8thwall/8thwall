import React from 'react'
import {createUseStyles} from 'react-jss'

import {bodySanSerif} from '../static/styles/settings'

const useStyles = createUseStyles({
  standardModalHeader: {
    display: 'flex',
    flexDirection: 'column',
    padding: '1.5rem',
    fontFamily: bodySanSerif,
    fontWeight: 400,
    fontSize: '16px',
    lineHeight: '24px',
    alignItems: 'center',
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
