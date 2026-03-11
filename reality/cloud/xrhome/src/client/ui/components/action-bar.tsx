import React from 'react'
import {createUseStyles} from 'react-jss'

const useActionBarStyles = createUseStyles({
  actionBar: {
    display: 'flex',
    justifyContent: 'flex-end',
  },
  floater: {
    display: 'flex',
    flexWrap: 'wrap',
    alignItems: 'center',
    margin: '-0.5em',
  },
  spacer: {
    margin: '0.5em',
  },
})

const ActionBar = ({children}) => {
  const classes = useActionBarStyles()
  const childElements = Array.isArray(children) ? children : [children]
  return (
    <div className={classes.actionBar}>
      <div className={classes.floater}>
        {/* eslint-disable-next-line react/no-array-index-key */}
        {childElements.map((e, i) => e && <div className={classes.spacer} key={i}>{e}</div>)}
      </div>
    </div>
  )
}

export default ActionBar
