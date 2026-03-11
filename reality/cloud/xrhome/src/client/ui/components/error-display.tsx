import React from 'react'

import {createUseStyles} from 'react-jss'

import {FloatingTray} from './floating-tray'
import {StaticBanner} from './banner'

const useStyles = createUseStyles({
  centeredContainer: {
    display: 'flex',
    justifyContent: 'center',
    marginTop: '1em',
  },
  body: {
    margin: '1em',
    width: '30vw',
    display: 'flex',
    flexDirection: 'column',
    gap: '1em',
  },
})

interface IErrorDisplay {
  error: Error | null
  children?: React.ReactNode
}

const ErrorDisplay: React.FC<IErrorDisplay> = ({error, children}) => {
  const classes = useStyles()
  return (
    <div className={classes.centeredContainer}>
      <FloatingTray>
        <div className={classes.body}>
          {error && (
            <StaticBanner type='danger' message={`${error.name}: ${error.message}`} />
          )}
          {children}
        </div>
      </FloatingTray>
    </div>
  )
}

export {
  ErrorDisplay,
}
