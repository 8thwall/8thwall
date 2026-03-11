/* eslint-disable quote-props */
import React from 'react'

import {combine} from '../../common/styles'

import {mobileViewOverride} from '../../static/styles/settings'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  floatingLabel: {
    position: 'absolute',
    top: '50%',
    left: 'var(--input-left-spacing, 0)',
    transform: 'translate(0, -50%)',
    zIndex: '15',
    transformOrigin: 'top left',
    transition: '0.25s transform, 0.5s opacity',
    pointerEvents: 'none',
    whiteSpace: 'nowrap',
    color: theme.fgMuted,
    '&.error': {
      color: theme.fgError,
    },
    '&.floated': {
      transform: 'translate(0, calc(-0.5em - 50%)) scale(0.75)',
      [mobileViewOverride]: {
        transform: 'translate(0, calc(-0.5em - 60%)) scale(0.75)',
      },
    },
    '&.faded': {
      opacity: '0',
    },
  },
}))

interface IFloatingLabel {
  error?: boolean
  visible?: boolean
  floated?: boolean
  children?: React.ReactNode
}

const FloatingLabel: React.FunctionComponent<IFloatingLabel> =
  ({error, visible, floated, children}) => {
    const {floatingLabel} = useStyles()

    const classes = combine(
      floatingLabel, error && 'error', (!visible) && 'faded', floated && 'floated'
    )
    // This isn't a <label> because it is meant to be nested inside a <label> with the input
    return (
      <span className={classes}>
        {children}
      </span>
    )
  }

export default FloatingLabel
