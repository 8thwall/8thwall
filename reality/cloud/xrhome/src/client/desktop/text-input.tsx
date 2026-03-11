import React from 'react'

import {combine} from '../common/styles'
import {Icon, type IconStroke} from '../ui/components/icon'
import {SrOnly} from '../ui/components/sr-only'
import {SpaceBetween} from '../ui/layout/space-between'
import {createThemedStyles} from '../ui/theme'
import {StandardFieldContainer} from '../ui/components/standard-field-container'

const useStyles = createThemedStyles(theme => ({
  complexInput: {
    padding: '1rem',
    display: 'grid',
    gridTemplateColumns: 'min-content 1fr',
    gridTemplateRows: 'auto auto',
    columnGap: '1rem',
    rowGap: '0.5rem',
    alignItems: 'center',
    fontFamily: 'Geist Mono',
  },
  simpleInput: {
    padding: '0rem 1rem',
    display: 'flex',
    fontFamily: 'Geist Mono',
    width: '100%',
    height: '100%',
  },
  label: {
    color: theme.fgMain,
    fontWeight: 600,
  },
  prefix: {
    color: theme.fgMuted,
  },
  inputContainer: {
    display: 'flex',
    flexDirection: 'column',
  },
  textInput: {
    'flexGrow': 1,
    'height': '100%',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::-moz-selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::placeholder': {
      'color': theme.fgMuted,
    },
    '&::-webkit-input-placeholder': {
      'color': theme.fgMuted,
    },
    '&::-moz-placeholder': {
      'color': theme.fgMuted,
    },
    '&:-ms-input-placeholder': {
      'color': theme.fgMuted,
    },
  },
}))

interface ITextInput extends Omit<React.InputHTMLAttributes<HTMLInputElement>, 'className'> {
  label: React.ReactNode
  grow?: boolean
  iconStroke?: IconStroke
}

const DetailedTextInput: React.FC<Omit<ITextInput, 'grow'>> = ({
  id, label, iconStroke, prefix, ...rest
}) => {
  const classes = useStyles()
  return (
    <StandardFieldContainer>
      <label
        htmlFor={id}
        className={classes.complexInput}
      >
        {iconStroke && <Icon stroke={iconStroke} />}
        <div className={classes.label}>
          {label}
        </div>
        <div />
        <div>
          <div className={classes.inputContainer}>
            {prefix && <span className={classes.prefix}>{prefix}</span>}
            <input
              id={id}
              className={combine('style-reset', classes.textInput)}
              {...rest}
            />
          </div>
        </div>
      </label>
    </StandardFieldContainer>
  )
}

const SimpleTextInput: React.FC<Omit<ITextInput, 'prefix'>> = ({
  id, label, iconStroke, grow, ...rest
}) => {
  const classes = useStyles()

  return (
    <StandardFieldContainer grow={grow}>
      <label
        htmlFor={id}
        className={classes.simpleInput}
      >
        <SpaceBetween centered narrow grow>
          {iconStroke && <Icon stroke={iconStroke} />}
          <SrOnly>{label}</SrOnly>
          <input
            id={id}
            className={combine('style-reset', classes.textInput)}
            {...rest}
          />
        </SpaceBetween>
      </label>
    </StandardFieldContainer>
  )
}

export {
  DetailedTextInput,
  SimpleTextInput,
}
