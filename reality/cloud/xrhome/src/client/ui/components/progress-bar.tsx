import React from 'react'
import {createUseStyles} from 'react-jss'

import {brandBlack, mint} from '../../static/styles/settings'

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
})

interface IProgressBar {
  progress: number  // Value from [0, 1]
}

const ProgressBar: React.FC<IProgressBar> = ({progress = 0}) => {
  const styles = useStyles()

  const percentFilled = Math.max(0, Math.min(1, progress)) * 100

  return (
    <div
      className={styles.progressBar}
      role='progressbar'
      aria-valuenow={percentFilled}
    >
      <div
        className={styles.filled}
        style={{width: `${percentFilled}%`}}
      />
    </div>
  )
}

export {
  ProgressBar,
}
