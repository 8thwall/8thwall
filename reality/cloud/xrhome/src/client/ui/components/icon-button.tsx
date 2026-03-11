import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'

import {Icon, IIcon} from './icon'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createUseStyles({
  button: {
    'cursor': 'pointer',
    'padding': '0.25em',
    'display': 'block',
    '&:disabled': {
      cursor: 'default',
      opacity: '0.5',
    },
  },
})

interface IIconButton extends Pick<IIcon, 'stroke' | 'color' | 'size'> {
  onClick: React.MouseEventHandler<HTMLButtonElement>
  onKeyDown?: React.KeyboardEventHandler<HTMLButtonElement>
  text: string
  disabled?: boolean
  loading?: boolean
  a8?: string
  inline?: boolean
  tabIndex?: number
}

const IconButton: React.FC<IIconButton> = ({
  stroke, onClick, onKeyDown, text, color, disabled, loading, a8, inline, tabIndex, size,
}) => {
  const classes = useStyles()
  return (
    <button
      a8={a8}
      type='button'
      aria-label={text}
      title={text}
      className={combine('style-reset', classes.button)}
      onClick={onClick}
      onKeyDown={onKeyDown}
      disabled={disabled || loading}
      tabIndex={tabIndex}
    >
      <LoadableButtonChildren loading={loading} block>
        <Icon stroke={stroke} inline={inline} block={!inline} color={color} size={size} />
      </LoadableButtonChildren>
    </button>
  )
}
export {
  IconButton,
}

export type {
  IIconButton,
}
