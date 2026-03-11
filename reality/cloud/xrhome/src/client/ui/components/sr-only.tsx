import React from 'react'
import {createUseStyles} from 'react-jss'

const useStyles = createUseStyles({
  srOnly: {
    position: 'absolute',
    width: '1px',
    height: '1px',
    padding: 0,
    margin: '-1px',
    overflow: 'hidden',
    clip: 'rect(0, 0, 0, 0)',
    whiteSpace: 'nowrap',
    borderWidth: 0,
  },
})

interface ISrOnly {
  children: React.ReactNode
}

const SrOnly: React.FC<ISrOnly> = ({children}) => {
  const classes = useStyles()
  return (
    <span className={classes.srOnly}>{children}</span>
  )
}

export {SrOnly}
