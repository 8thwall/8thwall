import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'

const useStyles = createThemedStyles(theme => ({
  element: {
    'fontFamily': 'inherit',
    'background': 'transparent',
    'border': 'none',
    'outline': 'none',
    'cursor': 'text',
    'color': theme.fgMain,
    'width': '100%',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::placeholder': {
      color: theme.fgMuted,
    },
  },
  medium: {
    height: '38px',
    paddingLeft: '0.75rem',
  },
  small: {
    height: '32px',
    paddingLeft: '0.75rem',
  },
  tiny: {
    height: '24px',
    fontSize: '12px',
    paddingLeft: '0.5rem',
  },
}))

interface IStandardTextInput extends React.InputHTMLAttributes<HTMLInputElement> {
  id: string
  errorMessage?: React.ReactNode
  height?: 'medium' | 'small' | 'tiny'
  disabled?: boolean
}

const StandardTextInput = React.forwardRef<HTMLInputElement, IStandardTextInput>(({
  id, errorMessage, height = 'medium', disabled, type = 'text', ...rest
}, ref) => {
  const classes = useStyles()

  const errorId = `${id}-errormessage`

  return (
    <StandardFieldContainer invalid={!!errorMessage} disabled={disabled}>
      <input
        {...rest}
        type={type}
        id={id}
        ref={ref}
        className={combine(classes.element, classes[height])}
        aria-invalid={!!errorMessage}
        aria-errormessage={errorMessage ? errorId : undefined}
        disabled={disabled}
      />
    </StandardFieldContainer>
  )
})

export {
  StandardTextInput,
}

export type {
  IStandardTextInput,
}
