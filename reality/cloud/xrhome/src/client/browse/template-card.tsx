import React from 'react'

import {Icon} from '../ui/components/icon'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  templateCard: {
    'display': 'block',
    'position': 'relative',
    'height': '200px',
    'minWidth': '300px',
    'overflow': 'hidden',
    'borderRadius': '1rem',
    'background': 'black',
    'backgroundSize': 'cover',
    'backgroundPosition': 'center center',
    'cursor': 'pointer',
    'margin': '4px',
    '&:has(:focus-visible)': {
      boxShadow: ` 0 0 0 2px #fff, 0 0 0 4px ${theme.fgPrimary}`,
    },
  },
  input: {
    appearance: 'none',
  },
  titleBar: {
    position: 'absolute',
    bottom: 0,
    width: '100%',
    left: 0,
    background: 'linear-gradient(#0000,#000)',
    color: 'white',
    padding: '0.5rem 1rem',
  },
  icon: {
    position: 'absolute',
    right: '1rem',
    top: '1rem',
    color: 'white',
    filter: 'drop-shadow(0 0 0.75rem black)',
  },
}))

interface ITemplateCard {
  title: string
  checked: boolean
  name: string
  onChange: () => void
  imageUrl: string
}

const TemplateCard: React.FC<ITemplateCard> = ({checked, onChange, name, title, imageUrl}) => {
  const classes = useStyles()
  const id = React.useId()
  return (
    <label
      htmlFor={id}
      className={classes.templateCard}
      style={{backgroundImage: `url(${imageUrl})`}}
    >
      <input
        id={id}
        type='radio'
        className={classes.input}
        name={name}
        checked={checked}
        onChange={onChange}
      />
      <div>
        <div className={classes.titleBar}>{title}</div>
      </div>
      {checked &&
        <span className={classes.icon}><Icon
          size={1.5}
          block
          stroke='checkCircle'
        />
        </span>}
    </label>
  )
}

export {
  TemplateCard,
}
