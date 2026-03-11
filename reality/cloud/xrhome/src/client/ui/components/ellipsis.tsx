import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  'ellipsis': {
    width: '38px',
    height: '38px',
  },
  'dot': {
    transition: 'fill 0.3s ease-in-out',
    fill: '#D9D9D9',
  },
  '@keyframes ellipsisAnimation': {
    '0%': {
      fill: '#D9D9D9',
    },
    '33%': {
      fill: '#464766',
    },
    '66%': {
      fill: '#D9D9D9',
    },
    '100%': {
      fill: '#D9D9D9',
    },
  },
  'dot1': {
    animation: '$ellipsisAnimation 1.5s infinite',
  },
  'dot2': {
    animation: '$ellipsisAnimation 1.5s infinite 0.5s',
  },
  'dot3': {
    animation: '$ellipsisAnimation 1.5s infinite 1s',
  },
})

const Ellipsis: React.FC = () => {
  const classes = useStyles()

  return (
    <div className={classes.ellipsis}>
      <svg
        width='38'
        height='38'
        viewBox='0 0 38 38'
        fill='none'
        xmlns='http://www.w3.org/2000/svg'
      >
        <circle className={combine(classes.dot, classes.dot1)} cx='5' cy='19' r='4' />
        <circle className={combine(classes.dot, classes.dot2)} cx='19' cy='19' r='4' />
        <circle className={combine(classes.dot, classes.dot3)} cx='33' cy='19' r='4' />
      </svg>
    </div>
  )
}

export {
  Ellipsis,
}
