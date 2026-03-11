import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  spaceBetween: {
    display: 'flex',
    gap: '1rem',
  },
  horizontal: {
    flexDirection: 'row',
    flexWrap: 'wrap',
  },
  vertical: {
    flexDirection: 'column',
  },
  narrow: {
    gap: '0.5em',
  },
  extraNarrow: {
    gap: '0.25em',
  },
  wide: {
    gap: '1.5rem',
  },
  extraWide: {
    gap: '2rem',
  },
  centered: {
    alignItems: 'center',
  },
  justifyCenter: {
    justifyContent: 'center',
  },
  noWrap: {
    flexWrap: 'nowrap',
  },
  grow: {
    flexGrow: 1,
    minHeight: 0,
  },
  between: {
    justifyContent: 'space-between',
  },
})

interface ISpaceBetween {
  direction?: 'horizontal' | 'vertical'
  narrow?: boolean
  extraNarrow?: boolean
  wide?: boolean
  extraWide?: boolean
  centered?: boolean
  justifyCenter?: boolean
  noWrap?: boolean
  grow?: boolean
  between?: boolean
  children?: React.ReactNode
}

const SpaceBetween: React.FC<ISpaceBetween> = ({
  children, direction = 'horizontal', narrow, centered, wide, justifyCenter, extraNarrow, extraWide,
  noWrap, grow, between,
}) => {
  const classes = useStyles()
  return (
    <div className={combine(
      classes.spaceBetween,
      classes[direction],
      centered && classes.centered,
      justifyCenter && classes.justifyCenter,
      extraNarrow && classes.extraNarrow,
      narrow && classes.narrow,
      wide && classes.wide,
      extraWide && classes.extraWide,
      noWrap && classes.noWrap,
      between && classes.between,
      grow && classes.grow
    )}
    >
      {children}
    </div>
  )
}

export {SpaceBetween}
