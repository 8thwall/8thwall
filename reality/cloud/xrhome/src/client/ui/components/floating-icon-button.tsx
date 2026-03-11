import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'
import {IIcon, Icon} from './icon'
import {LoadableButtonChildren} from './loadable-button-children'
import {useFloatingTrayButtonStyles} from './floating-tray-button'

const useStyles = createUseStyles({
  floatingIconButton: {
    padding: '0.625em',
  },
})

interface IFloatingIconButton
  extends Pick<IIcon, 'stroke' | 'color'>,
  Pick<React.ButtonHTMLAttributes<HTMLButtonElement>, 'onClick' | 'id' > {
  onKeyDown?: React.KeyboardEventHandler<HTMLButtonElement>
  isActive?: boolean
  isDisabled?: boolean
  text: string
  loading?: boolean
  tabIndex?: number
  a8?: string
}

const FloatingIconButton: React.FC<IFloatingIconButton> = ({
  isActive = false, isDisabled = false, text, stroke, color, loading,
  tabIndex, ...rest
}) => {
  const floatingButtonStyles = useFloatingTrayButtonStyles()
  const classes = useStyles()

  return (
    <button
      {...rest}
      type='button'
      title={text}
      aria-label={text}
      tabIndex={tabIndex}
      className={combine(
        classes.floatingIconButton, floatingButtonStyles.floatingTrayButton,
        isDisabled && floatingButtonStyles.disabled, isActive && floatingButtonStyles.active
      )}
    >
      <LoadableButtonChildren loading={loading} block>
        <Icon stroke={stroke} block color={color} />
      </LoadableButtonChildren>
    </button>
  )
}

export {
  FloatingIconButton,
}
