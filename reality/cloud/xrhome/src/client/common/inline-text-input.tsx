import React from 'react'

import {combine} from './styles'
import {cherry} from '../static/styles/settings'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  error: {
    border: `1px solid ${cherry} !important`,
  },
  selection: {
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
  },
}
))

interface IInlineTextInput extends Omit<React.InputHTMLAttributes<HTMLInputElement>, 'onSubmit'> {
  onCancel?: () => void
  onSubmit: () => void
  invalid?: boolean
  formClassName?: string
  inputClassName?: string
}

const InlineTextInput: React.FC<IInlineTextInput> = (props) => {
  const {onCancel, onSubmit, formClassName, inputClassName, invalid, ...rest} = props
  const classes = useStyles()

  const formSubmit = (e) => {
    e.preventDefault()
    onSubmit()
  }

  const checkKey = (e) => {
    if (e.keyCode === 27) {
      onCancel()
    }
  }

  return (
    <form className={formClassName} onSubmit={formSubmit}>
      <input
        {...rest}
        ref={element => element && element.focus()}
        type='text'
        className={combine(invalid && classes.error, inputClassName, classes.selection)}
        onBlur={onSubmit}
        spellCheck='false'
        onKeyDown={checkKey}
      />
    </form>
  )
}

export default InlineTextInput
