import React, {FunctionComponent} from 'react'
import {createUseStyles} from 'react-jss'

import {brandHighlight, gray2} from '../../static/styles/settings'

const useStyles = createUseStyles({
  '@keyframes loadBreathe': {
    from: {
      'background-color': 'rgba(255, 255, 255, .75)',
    },
    to: {
      'background-color': 'rgba(255, 255, 255, .4)',
    },
  },
  'switch': {
    'display': 'flex',
    'align-items': 'center',
    '& input': {
      opacity: 0,
      width: 0,
      height: 0,
    },
    '& input:checked + .slider': {
      'background-color': brandHighlight,
    },
    '& input:focus + .slider': {
      'box-shadow': `0 0 2px ${brandHighlight}`,
    },
    '& input:checked + .slider:before': {
      transform: 'translateX(22px)',
    },
    '& .slider': {
      'position': 'relative',
      'width': '48px',
      'height': '26px',
      'cursor': 'pointer',
      'background-color': gray2,
      'transition': 'transform .4s',
      'border-radius': '34px',
      'margin-right': '1em',
    },
    '& .slider:before': {
      'position': 'absolute',
      'content': '""',
      'height': '22px',
      'width': '22px',
      'left': '2px',
      'bottom': '2px',
      'background-color': 'white',
      'transition': 'transform .4s',
      'border-radius': '50%',
    },
  },
  'loading': {
    '& .slider:before': {
      animation: '$loadBreathe 0.75s ease-in-out infinite alternate both',
    },
  },
})

interface ISlider8 {
  loading?: boolean
  disabled?: boolean
  checked?: boolean
  onChange?: (checked: boolean) => void
  id: string
  children?: any
}

export const Slider8: FunctionComponent<ISlider8> = ({
  loading = false,
  disabled = false,
  checked = false,
  onChange = undefined,
  id,
  children,
}) => {
  const classes = useStyles()

  return (
    <label htmlFor={id} className={`${classes.switch} ${loading ? classes.loading : ''}`}>
      <input
        id={id}
        type='checkbox'
        disabled={disabled}
        checked={checked}
        onChange={(e) => {
          if (onChange) {
            onChange(e.target.checked)
          }
        }}
      />
      <span className='slider' />
      {children}
    </label>
  )
}
