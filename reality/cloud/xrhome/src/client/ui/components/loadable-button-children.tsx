import React from 'react'
import {createUseStyles} from 'react-jss'

import {Loader} from './loader'
import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  loadableButtonChildren: {
    position: 'relative',
    display: 'inline-block',
  },
  invisibleChildren: {
    visibility: 'hidden',
  },
  block: {
    display: 'block',
  },
})

interface ILoadableButtonChildren {
  loading?: boolean
  block?: boolean
  children?: React.ReactNode
}

const LoadableButtonChildren: React.FC<ILoadableButtonChildren> = ({children, loading, block}) => {
  const classes = useStyles()

  if (!loading) {
    return <>{children}</>
  }

  return (
    <span className={combine(classes.loadableButtonChildren, block && classes.block)}>
      <Loader size='tiny' />
      <span className={classes.invisibleChildren}>{children}</span>
    </span>
  )
}

export {
  LoadableButtonChildren,
}
