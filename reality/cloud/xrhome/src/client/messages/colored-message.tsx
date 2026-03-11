import React from 'react'
import {Icon, SemanticICONS} from 'semantic-ui-react'

import {combine} from '../common/styles'
import {
  brandBlack,
  moonlight,
  gray2,
  orangish,
  orangishHighlight,
  darkOrange,
  purpleish,
  brandHighlight,
  brandPurple,
  darkBlueberry,
  blueberry,
  cherry,
  mint,
  mango,
} from '../static/styles/settings'
import {createCustomUseStyles} from '../common/create-custom-use-styles'

const useStyles = createCustomUseStyles<ReturnType<typeof getBorderColorAndBackground>>()({
  container: {
    'padding': '1em',
    'fontSize': '1em',
    'lineHeight': '1.1em',
    'color': brandBlack,
    'border': ({borderColor}) => `1px solid ${borderColor}`,
    'display': 'flex',
    'flex-direction': 'row',
    'background': ({background}) => background,
    'borderRadius': '0.5rem',
  },
  icon: {
    alignSelf: 'start',
    fontSize: '2em',
    marginRight: '0.5em !important',
    color: ({borderColor}) => borderColor,
  },
  children: {
    'flex': 1,
    'color': ({textColor}) => textColor,
    '& > p > span': {
      textDecoration: 'underline',
    },
  },
})

type MessageColor = 'blue' | 'gray' | 'orange' | 'purple' | 'red' | 'green' | 'yellow'

interface Props {
  color: MessageColor
  className?: string
  iconName?: SemanticICONS
  iconClass?: string
  children?: React.ReactNode
}

const getBorderColorAndBackground = (color: MessageColor) => {
  switch (color) {
    case 'blue':
      return {borderColor: blueberry, background: `${blueberry}05`, textColor: darkBlueberry}
    case 'orange':
      return {borderColor: orangishHighlight, background: orangish, textColor: darkOrange}
    case 'purple':
      return {borderColor: brandHighlight, background: purpleish, textColor: brandPurple}
    case 'gray':
    default:
      return {borderColor: gray2, background: moonlight, textColor: brandBlack}
    case 'red':
      return {borderColor: cherry, background: `${cherry}05`, textColor: cherry}
    case 'green':
      return {borderColor: mint, background: `${mint}05`, textColor: mint}
    case 'yellow':
      return {borderColor: mango, background: `${mango}05`, textColor: mango}
  }
}

const ColoredMessage: React.FC<Props> = ({
  color, className = '', iconClass = '', iconName, children,
}) => {
  const classes = useStyles(getBorderColorAndBackground(color))
  const iconElem = (
    <Icon
      name={iconName}
      className={combine(classes.icon, iconClass)}
    />
  )
  return (
    <div className={combine(classes.container, className)}>
      {iconName && iconElem}
      <div className={classes.children}>
        {children}
      </div>
    </div>
  )
}

export default ColoredMessage
