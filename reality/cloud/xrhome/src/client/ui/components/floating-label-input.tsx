import React from 'react'
import uuidv4 from 'uuid/v4'

import './floating-label-input.scss'
import {combine} from '../../common/styles'
import FloatingLabel from './floating-label'

interface IInputWrapper {
  className?: string
  hasError?: boolean
  hasFocus?: boolean
  hideOverflow?: boolean
  children?: React.ReactNode
}

export const InputWrapper: React.FunctionComponent<IInputWrapper> =
   ({className, hasError, hasFocus, hideOverflow, children}) => {
     const classes = combine(
       'floating-input-wrapper',
       hasError && 'has-error',
       hasFocus && 'has-focus',
       hideOverflow && 'hide-overflow',
       className
     )

     return (
       <div className={classes}>
         {children}
       </div>
     )
   }

export interface IInputElement extends React.InputHTMLAttributes<HTMLInputElement> {
  label: string
  errorText?: string
  value: string
}

export const InputElement = React.forwardRef<HTMLInputElement, IInputElement>(
  ({id, value, label, onChange, errorText, onFocus, onBlur, className, ...rest}, ref) => {
    const idToUse = React.useMemo(() => id || uuidv4(), [id])
    const [hasFocus, setHasFocus] = React.useState(false)
    const outerClassName = combine('floating-input', value && 'has-text', errorText && 'has-error')

    const handleFocus: typeof onFocus = (e) => {
      setHasFocus(true)
      onFocus?.(e)
    }

    const handleBlur: typeof onBlur = (e) => {
      setHasFocus(false)
      onBlur?.(e)
    }

    const labelsFloated = !!(errorText || value || hasFocus)

    React.useEffect(() => {
      if (labelsFloated) {
        return undefined
      }

      const autofillTimer = setTimeout(() => {
        const inputElem = document.getElementById(idToUse)
        if (inputElem?.matches(':-webkit-autofill')) {
          setHasFocus(true)
        }
      }, 900)

      return () => {
        clearTimeout(autofillTimer)
      }
    }, [])

    return (
      <label htmlFor={idToUse} className={outerClassName}>
        <FloatingLabel floated={labelsFloated} visible={!errorText}>{label}</FloatingLabel>
        <FloatingLabel
          error
          floated={labelsFloated}
          visible={!!errorText}
        >{errorText}
        </FloatingLabel>
        <input
          id={idToUse}
          ref={ref}
          className={combine('style-reset input-inner', className)}
          value={value}
          onChange={onChange}
          onFocus={handleFocus}
          onBlur={handleBlur}
          {...rest}
        />
      </label>
    )
  }
)

export const FloatingLabelInput: React.FunctionComponent<IInputElement> = props => (
  <InputWrapper hasError={!!props.errorText} hideOverflow>
    <InputElement {...props} />
  </InputWrapper>
)
