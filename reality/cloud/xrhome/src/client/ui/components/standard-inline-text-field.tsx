import React from 'react'

import {combine} from '../../common/styles'
import {tinyViewOverride} from '../../static/styles/settings'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {StandardFieldLabel} from './standard-field-label'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  container: {
    display: 'flex',
    alignItems: 'center',
    gap: '1rem',
    [tinyViewOverride]: {
      display: 'block',
    },
  },
  label: {
    display: 'flex',
    alignItems: 'center',
    gap: '1rem',
    [tinyViewOverride]: {
      display: 'block',
    },
  },
  element: {
    'fontFamily': 'inherit',
    'background': 'transparent',
    'border': 'none',
    'outline': 'none',
    'cursor': 'text',
    'color': theme.fgMain,
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::placeholder': {
      color: theme.fgMuted,
    },
    [tinyViewOverride]: {
      width: '100% !important',
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
  mediumContainer: {
    [tinyViewOverride]: {
      maxWidth: '300px',
    },
  },
  smallContainer: {
    [tinyViewOverride]: {
      maxWidth: '118px',
    },
  },
  mediumInput: {
    width: '300px',
  },
  smallInput: {
    width: '118px',
  },
  errorText: {
    'color': theme.fgError,
    [tinyViewOverride]: {
      'display': 'block',
      'marginTop': '0.5rem',
    },
  },
}))

interface IStandardInlineTextField extends React.InputHTMLAttributes<HTMLInputElement> {
  id?: string
  label: React.ReactNode
  errorMessage?: React.ReactNode
  width?: 'medium' | 'small'
  height?: 'medium' | 'small' | 'tiny'
  disabled?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
}

const StandardInlineTextField: React.FC<IStandardInlineTextField> = ({
  id: idOverride, className, label, errorMessage, width = 'medium', height = 'medium', disabled,
  boldLabel, starredLabel, ...rest
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()
  const errorId = `${id}-errormessage`

  return (
    <div className={combine(classes.container, classes[`${width}Container`])}>
      <label tabIndex={-1} htmlFor={id} className={classes.label}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          inline
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardFieldContainer invalid={!!errorMessage} disabled={disabled}>
          <input
            {...rest}
            type='text'
            id={id}
            className={
              combine(classes.element, classes[height], className, classes[`${width}Input`])
            }
            aria-invalid={!!errorMessage}
            aria-errormessage={errorMessage ? errorId : undefined}
            disabled={disabled}
          />
        </StandardFieldContainer>
      </label>
      {errorMessage && <span id={errorId} className={classes.errorText}>{errorMessage}</span>}
    </div>
  )
}

export {
  StandardInlineTextField,
}

export type {
  IStandardInlineTextField,
}
