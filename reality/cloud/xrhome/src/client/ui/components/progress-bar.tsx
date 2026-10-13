import React from 'react'
import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'
import {brandBlack, mint, popGradient} from '../../static/styles/settings'

const useStyles = createUseStyles({
  progressBar: {
    background: brandBlack,
    margin: '0',
    borderRadius: '0.5em',
    height: '0.5em',
    width: '100%',
  },
  filled: {
    background: mint,
    height: '0.5em',
    borderRadius: '0.5em',
    transition: 'width .5s',
  },
  gradient: {
    background: popGradient,
  },
})

interface IProgressBar {
  ariaLabel: string
  progress: number  // Value from [0, 1]
  color?: 'default' | 'gradient'
  transitionDuration?: string
}

const ProgressBar: React.FC<IProgressBar> = ({
  ariaLabel, progress = 0, color = 'default', transitionDuration,
}) => {
  const styles = useStyles()

  const percentFilled = Math.max(0, Math.min(1, progress)) * 100

  return (
    <div
      className={styles.progressBar}
      role='progressbar'
      aria-label={ariaLabel}
      aria-valuenow={percentFilled}
    >
      <div
        className={combine(styles.filled, color === 'gradient' && styles.gradient)}
        style={{
          transitionDuration,
          width: `${percentFilled}%`,
        }}
      />
    </div>
  )
}

export {
  ProgressBar,
}
