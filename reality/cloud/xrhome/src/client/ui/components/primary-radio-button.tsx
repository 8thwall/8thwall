/* eslint-disable quote-props */
import React from 'react'
import {createUseStyles} from 'react-jss'
import uuidv4 from 'uuid/v4'

import {combine} from '../../common/styles'
import {brandPurple} from '../../static/styles/settings'

const useStyles = createUseStyles({
  radioDisplay: {
    boxSizing: 'border-box',
    display: 'inline-block',
    width: '1.5em',
    height: '1.5em',
    border: `1px solid ${brandPurple}`,
    padding: '3px',
    borderRadius: '50%',
    '$radioButton:not(:checked) + &:hover': {
      boxShadow: 'inset 0 0 0 2px #0004',
    },
  },
  radioButton: {
    position: 'absolute',
    transform: 'translate(0.5rem, 0.5rem) scale(0.01)',
    opacity: '0.01',
    zIndex: '-1',
    '&:checked + $radioDisplay': {
      background: brandPurple,
      backgroundClip: 'content-box',
    },
    '&:focus + $radioDisplay': {
      boxShadow: '0 0 0 2px #0004',
    },
  },
})

interface IPrimaryRadioButton extends React.InputHTMLAttributes<HTMLInputElement> {
  id: string
}

const PrimaryRadioButton: React.FunctionComponent<IPrimaryRadioButton> =
  ({id, className, ...rest}) => {
    const {radioButton, radioDisplay} = useStyles()
    return (
      <>
        <input type='radio' {...rest} id={id} className={radioButton} />
        <span className={combine(radioDisplay, className)} />
      </>
    )
  }

export default PrimaryRadioButton
