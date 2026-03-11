import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {StandardFieldLabel} from './standard-field-label'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  element: {
    'fontFamily': 'inherit',
    'background': 'transparent',
    'boxSizing': 'border-box',
    'border': 'none',
    'outline': 'none',
    'color': theme.fgMain,
    'width': '100%',
    'borderRight': '0.5rem solid transparent',  // This properly aligns the dropdown arrow
    'appearance': 'none',
    'WebkitAppearance': 'none',
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
  errorText: {
    color: theme.fgError,
  },
  chevron: {
    'borderRight': `1.5px solid ${theme.fgMuted}`,
    'borderBottom': `1.5px solid ${theme.fgMuted}`,
    'width': '0.5rem',
    'height': '0.5rem',
    'transform': 'translateY(-65%) rotate(45deg)',
    'position': 'absolute',
    'right': '1rem',
    'top': '50%',
    'pointerEvents': 'none',
    'borderRadius': '1px',
    'select:disabled + &': {
      'opacity': 0.5,
    },
  },
}))

interface IStandardSelectField extends React.SelectHTMLAttributes<HTMLSelectElement> {
  id?: string
  label: React.ReactNode
  tooltip?: React.ReactNode
  errorMessage?: React.ReactNode
  height?: 'medium' | 'small' | 'tiny'
  disabled?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
}

const StandardSelectField: React.FC<IStandardSelectField> = ({
  id: idOverride, className, label, errorMessage, height = 'medium', disabled, children,
  boldLabel, starredLabel, ...rest
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  const errorId = `${id}-errormessage`

  return (
    <div>
      <label htmlFor={id}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardFieldContainer disabled={disabled}>
          <select
            {...rest}
            id={id}
            className={combine(classes.element, classes[height], className)}
            aria-invalid={!!errorMessage}
            aria-errormessage={errorMessage ? errorId : undefined}
            disabled={disabled}
          >
            {children}
          </select>
          <span className={classes.chevron} aria-hidden />
        </StandardFieldContainer>
      </label>
      {errorMessage && <p className={classes.errorText}>{errorMessage}</p>}
    </div>
  )
}

export {
  StandardSelectField,
}

export type {
  IStandardSelectField,
}
