import React from 'react'
import {createUseStyles} from 'react-jss'

const useStyles = createUseStyles({
  spaceBelow: {
    marginBottom: '1rem',
  },
})

const SpaceBelow = ({children}) => {
  const {spaceBelow} = useStyles()
  return (
    <div className={spaceBelow}>
      {children}
    </div>
  )
}

export default SpaceBelow
