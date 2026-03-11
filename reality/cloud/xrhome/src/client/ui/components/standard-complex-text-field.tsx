import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {Icon, type IconStroke} from './icon'
import {tinyViewOverride} from '../../static/styles/settings'

const useStyles = createThemedStyles(theme => ({
  element: {
    'fontFamily': 'inherit',
    'background': 'transparent',
    'border': 'none',
    'outline': 'none',
    'cursor': 'text',
    'color': theme.fgMain,
    'width': '100%',
    'paddingLeft': '0',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::placeholder': {
      color: theme.fgMuted,
    },
  },
  fieldContainer: {
    display: 'flex',
    gap: '1.125em',
    padding: '1.125em',
    color: theme.fgMuted,
    [tinyViewOverride]: {
      padding: '1em 0.5em 1em 1em',
      gap: '0.5em',
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
  containerWithLabel: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
  },
  prefix: {
    color: theme.fgMuted,
    flexBasis: 'unset !important',
  },
  inputWithPrefix: {
    display: 'flex',
    alignItems: 'center',
    gap: '0',
  },
  disabled: {
    color: theme.sfcDisabledColor,
  },
}))

interface IStandardComplexTextField extends React.InputHTMLAttributes<HTMLInputElement> {
  id: string
  errorMessage?: React.ReactNode
  height?: 'medium' | 'small' | 'tiny'
  disabled?: boolean
  iconStroke?: IconStroke
  label: string
  showLabel?: boolean
  inputPrefix?: string
}

const StandardComplexTextField = React.forwardRef<HTMLInputElement, IStandardComplexTextField>(({
  id, errorMessage, height = 'medium', disabled, type = 'text', iconStroke = null,
  label, showLabel = true, inputPrefix, ...rest
}, ref) => {
  const classes = useStyles()

  const errorId = `${id}-errormessage`

  const inputField = (
    <input
      {...rest}
      type={type}
      id={id}
      ref={ref}
      className={combine(classes.element, !showLabel && classes[height],
        disabled && classes.disabled)}
      aria-invalid={!!errorMessage}
      aria-errormessage={errorMessage ? errorId : undefined}
      disabled={disabled}
    />
  )

  return (
    <StandardFieldContainer invalid={!!errorMessage} disabled={disabled}>
      <label className={classes.fieldContainer} tabIndex={-1} htmlFor={id}>
        {iconStroke && <Icon stroke={iconStroke} />}
        <div className={classes.containerWithLabel}>
          {showLabel && <div>{label}</div>}
          {inputPrefix
            ? (
              <div className={classes.inputWithPrefix}>
                <div className={classes.prefix}>{inputPrefix}</div>
                {inputField}
              </div>)
            : inputField
          }
        </div>
      </label>
    </StandardFieldContainer>
  )
})

export {
  StandardComplexTextField,
}

export type {
  IStandardComplexTextField,
}
