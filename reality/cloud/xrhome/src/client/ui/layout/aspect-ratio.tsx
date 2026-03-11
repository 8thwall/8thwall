import React from 'react'
import {createUseStyles} from 'react-jss'

const COVER_IMAGE_ASPECT_RATIO = 16 / 9

const useStyles = createUseStyles({
  aspectRatio: {
    display: 'block',
    height: 0,
    position: 'relative',
  },
})

interface IAspectRatio {
  ratio: number
  children?: React.ReactNode
}

const AspectRatio: React.FC<IAspectRatio> = ({ratio, children}) => (
  <div className={useStyles().aspectRatio} style={{paddingTop: `${100 / ratio}%`}}>{children}</div>
)

export {
  AspectRatio,
  COVER_IMAGE_ASPECT_RATIO,
}
