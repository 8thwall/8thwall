import React from 'react'

import {createThemedStyles} from '../../../ui/theme'
import {combine} from '../../../common/styles'

const useStyles = createThemedStyles(theme => ({
  'progressBar': {
    background: theme.descriptionBackground,
    margin: '0',
    borderRadius: '0.75rem',
    height: '0.5rem',
    width: '100%',
    overflow: 'hidden',
  },
  'filled': {
    // TODO:(xiaokai) Add glow effect
    background: 'linear-gradient(90deg, #5C0D8F 1.86%, #FF4713 56.39%, #FFC828 101%)',
    boxShadow: '0px -2px 4px 0px rgba(0, 0, 0, 0.25) inset, ' +
    '0px 3px 4px 0px rgba(255, 255, 255, 0.50) inset',
    height: '0.5em',
    borderRadius: '0.75rem',
    transition: 'width .5s',
  },
  '@keyframes progressAnimation': {
    '0%': {width: '0%'},
    '10%': {width: '15%'},
    '20%': {width: '25%'},
    '35%': {width: '45%'},
    '50%': {width: '60%'},
    '65%': {width: '72%'},
    '80%': {width: '82%'},
    '90%': {width: '88%'},
    '95%': {width: '92%'},
    '100%': {width: '95%'},
  },
  'fauxProgressBar': {
    animation: '$progressAnimation 10s',
    animationTimingFunction: 'ease-in-out',
    animationIterationCount: 1,
    animationFillMode: 'forwards',
  },
}))

interface IProgressBar {
  progress?: number  // Value from [0, 1]
  fauxProgressBar?: boolean
}

const ProgressBar: React.FC<IProgressBar> = ({progress = 0, fauxProgressBar = false}) => {
  const styles = useStyles()

  const percentFilled = Math.max(0, Math.min(1, progress)) * 100

  return (
    <div
      className={styles.progressBar}
      role='progressbar'
      aria-valuenow={percentFilled}
    >
      {fauxProgressBar
        ? (
          <div
            className={combine(styles.filled, styles.fauxProgressBar)}
          />
        )
        : (
          <div
            className={styles.filled}
            style={{width: `${percentFilled}%`}}
          />
        )}
    </div>
  )
}

export {
  ProgressBar,
}
