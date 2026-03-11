import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../theme'
import {StandardFieldLabel} from './standard-field-label'
import {StandardTextInput} from './standard-text-input'
import {IconButton} from './icon-button'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  errorText: {
    color: theme.fgError,
    marginTop: theme.stfErrorTextMargin,
    marginBottom: theme.stfErrorTextMargin,
  },
  container: {
    position: 'relative',
  },
  passwordIcon: {
    cursor: 'pointer',
    position: 'absolute',
    right: '0.5em',
    bottom: '0.5em',
    zIndex: 10,
    backgroundColor: theme.sfcBackgroundDefault,
  },
  fieldContainer: {
    display: theme.stfDisplay,
    flexDirection: theme.stfFlexDirection,
    gap: theme.stfGap,
  },
}))

interface IStandardTextField extends React.InputHTMLAttributes<HTMLInputElement> {
  id?: string
  label: React.ReactNode
  errorMessage?: React.ReactNode
  height?: 'medium' | 'small' | 'tiny'
  disabled?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
}

const StandardTextField = React.forwardRef<HTMLInputElement, IStandardTextField>(({
  id: idOverride, label, errorMessage, height = 'medium', disabled, boldLabel, starredLabel,
  type = 'text', ...rest
}, ref) => {
  const {t} = useTranslation(['common'])
  const classes = useStyles()
  const [passwordInputType, setPasswordInputType] = React.useState('password')
  const [passwordIconOpen, setPasswordIconOpen] = React.useState(false)
  const id = useIdFallback(idOverride)

  const setShowPassword = () => {
    if (passwordInputType === 'password') {
      setPasswordInputType('text')
      setPasswordIconOpen(true)
    } else {
      setPasswordInputType('password')
      setPasswordIconOpen(false)
    }
  }

  return (
    <div className={classes.container}>
      <label tabIndex={-1} htmlFor={id} className={classes.fieldContainer}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardTextInput
          {...rest}
          id={id}
          type={type === 'password' ? passwordInputType : type}
          errorMessage={errorMessage}
          height={height}
          disabled={disabled}
          ref={ref}
        />
        {type === 'password' &&
          <div
            className={classes.passwordIcon}
          >
            <IconButton
              text={passwordIconOpen
                ? t('button.password_visible')
                : t('button.password_hidden')}
              stroke={passwordIconOpen ? 'visible' : 'hidden'}
              color='gray4'
              onClick={setShowPassword}
              tabIndex={-1}
            />
          </div>
        }
      </label>
      {errorMessage && <p className={classes.errorText}>{errorMessage}</p>}
    </div>
  )
})

export {
  StandardTextField,
}

export type {
  IStandardTextField,
}
