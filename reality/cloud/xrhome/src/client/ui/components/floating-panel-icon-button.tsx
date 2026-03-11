import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'
import {IIcon, Icon, IconColor} from './icon'
import {LoadableButtonChildren} from './loadable-button-children'
import {useFloatingPanelButtonStyles} from './floating-panel-button'

const useStyles = createUseStyles({
  floatingIconButton: {
    border: 'none',
  },
  normal: {
    padding: '0.5rem',
  },
  tiny: {
    padding: '0.35rem',
  },
  large: {
    padding: '9px',
  },
})

interface IFloatingPanelIconButton
  extends Pick<IIcon, 'stroke' >,
  Pick<React.ButtonHTMLAttributes<HTMLButtonElement>, 'onClick' > {
  onKeyDown?: React.KeyboardEventHandler<HTMLButtonElement>
  text: string
  loading?: boolean
  buttonSize?: 'normal' | 'tiny' | 'large'
  disabled?: boolean
  a8?: string
  iconSize?: number
  iconColor?: IconColor
}

const FloatingPanelIconButton: React.FC<IFloatingPanelIconButton> = ({
  text, stroke, loading, buttonSize = 'normal', disabled, iconSize, iconColor, ...rest
}) => {
  const floatingButtonStyles = useFloatingPanelButtonStyles()
  const classes = useStyles()

  return (
    <button
      {...rest}
      type='button'
      title={text}
      aria-label={text}
      disabled={disabled}
      className={combine(classes.floatingIconButton, floatingButtonStyles.floatingTrayButton,
        classes[buttonSize])}
    >
      <LoadableButtonChildren loading={loading} block>
        <Icon stroke={stroke} block color={iconColor} size={iconSize} />
      </LoadableButtonChildren>
    </button>
  )
}

export {
  FloatingPanelIconButton,
}
