import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../common/styles'

import {gray4, green, mango} from '../static/styles/settings'

const useLogTabStyles = createUseStyles({
  '@keyframes fadeWithDelay': {
    '0%': {'opacity': 1},
    '35%': {'opacity': 1},
    '100%': {'opacity': 0},
  },
  'recencyIndicator': {
    'position': 'relative',
    'width': '1em',
    'height': '1em',
    'border-radius': '50%',
    'box-shadow': `0 0 0 1px ${gray4} inset`,
    'margin-right': '0.5em',
  },
  'light': {
    'position': 'absolute',
    'top': '50%',
    'left': '50%',
    'border-radius': '50%',
    'width': '100%',
    'height': '100%',
    'background-color': green,
    'opacity': 0,
    'transform': 'translate(-50%, -50%)',
  },
  'lit': {
    'animation-duration': '120s',
    'animation-name': '$fadeWithDelay',
    'animation-timing-function': 'cubic-bezier(0.25, 0.46, 0.45, 0.94)',
  },
  'mango': {
    backgroundColor: mango,
  },
})

interface IRecencyIndicator {
  lastLogTime: number
  title?: string
  color?: 'green' | 'mango'
}

const RecencyIndicator: React.FunctionComponent<IRecencyIndicator> = ({
  lastLogTime, title, color,
}) => {
  const ref = React.useRef<HTMLDivElement>()
  const classes = useLogTabStyles()

  React.useEffect(() => {
    if (ref.current) {
      ref.current.classList.remove(classes.lit)
      if (lastLogTime) {
        // Accessing offsetWidth triggers a reflow to force the animation to reset
        // eslint-disable-next-line no-unused-expressions
        ref.current.offsetWidth
        ref.current.classList.add(classes.lit)
      }
    }
  }, [lastLogTime])

  return (
    <span className={classes.recencyIndicator} title={title}>
      <span className={combine(classes.light, color === 'mango' && classes.mango)} ref={ref} />
    </span>
  )
}

export default RecencyIndicator
