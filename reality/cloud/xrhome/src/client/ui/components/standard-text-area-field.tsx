import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {StandardFieldLabel} from './standard-field-label'
import {useIdFallback} from '../../studio/use-id-fallback'

const STANDARD_TEXT_AREA_WIDTH = 150
const STANDARD_TEXT_AREA_HEIGHT = 3

const useStyles = createThemedStyles(theme => ({
  errorText: {
    color: `${theme.fgError} !important`,
  },
  textarea: {
    'padding': '1rem',
    'fontFamily': 'inherit',
    'background': 'transparent',
    'border': 'none',
    'outline': 'none',
    'cursor': 'text',
    'color': theme.fgMain,
    'width': '100%',
    'resize': 'none',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&::placeholder': {
      color: theme.fgMuted,
    },
    '&::-webkit-scrollbar-corner': {
      backgroundColor: 'transparent',
    },
  },
  charCount: {
    'color': theme.fgMuted,
    'position': 'absolute',
    'right': '0.7em',
    'bottom': '0.2em',
  },
  resize: {
    resize: 'vertical',
  },
}))

interface IStandardTextAreaField extends React.TextareaHTMLAttributes<HTMLTextAreaElement> {
  label: React.ReactNode
  value: string
  errorMessage?: React.ReactNode
  resize?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
}

const StandardTextAreaField = React.forwardRef<HTMLTextAreaElement, IStandardTextAreaField>(({
  id: idOverride, className, label, errorMessage, disabled, maxLength,
  rows = STANDARD_TEXT_AREA_HEIGHT, cols = STANDARD_TEXT_AREA_WIDTH, value, resize = false,
  boldLabel, starredLabel, ...rest
}, ref) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  const errorId = `${id}-errormessage`

  return (
    <div>
      <label tabIndex={-1} htmlFor={id}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardFieldContainer>
          <textarea
            {...rest}
            id={id}
            ref={ref}
            className={combine(classes.textarea, resize && classes.resize, className)}
            maxLength={maxLength && maxLength}
            value={value}
            cols={cols}
            rows={rows}
            aria-invalid={!!errorMessage}
            aria-errormessage={errorMessage ? errorId : undefined}
          />
          {maxLength &&
            <span className={
              combine(classes.charCount, (value?.length >= maxLength) && classes.errorText)}
            >
              <span>{value?.length}/{maxLength}</span>
            </span>
          }
        </StandardFieldContainer>
      </label>
      {errorMessage && <p id={errorId} className={classes.errorText}>{errorMessage}</p>}
    </div>
  )
})

export {
  StandardTextAreaField,
}

export type {
  IStandardTextAreaField,
}
