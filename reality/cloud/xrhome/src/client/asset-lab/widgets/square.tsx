import React from 'react'

import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import type {UiTheme} from '../../ui/theme'
import {combine} from '../../common/styles'
import {useSelector} from '../../hooks'

type ISquarePlaceholder = {
  size: number
  children?: React.ReactNode
}

const SquarePlaceholder: React.FC<ISquarePlaceholder> = ({size, children}) => {
  // Randomly generate a color for the square
  const randomColor = `#${Math.floor(Math.random() * 16777215).toString(16).padStart(6, '0')}`
  const randomColor2 = `#${Math.floor(Math.random() * 16777215).toString(16).padStart(6, '0')}`
  const randomDeg = Math.floor(Math.random() * 360)
  // Create a random circular gradient with these two colors
  const randomGradient = `linear-gradient(${randomDeg}deg, ${randomColor}, ${randomColor2})`
  return (
    <div
      style={{
        width: size,
        height: size,
        background: randomGradient,
        borderRadius: '4px',
        display: 'flex',
        justifyContent: 'center',
        alignItems: 'center',
      }}
    >
      {children}
    </div>
  )
}

interface PlaceholderBoxStyles {
  width: string
  height: string
}
const useStyles = createCustomUseStyles<PlaceholderBoxStyles>()((theme: UiTheme) => ({
  placeholderBox: {
    width: props => props.width,
    height: props => props.height,
    position: 'relative',
    borderRadius: '10px',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    objectFit: 'cover',
    overflow: 'hidden',
    background: theme.studioBgMain,
  },
  border: {
    border: `1px solid ${theme.studioAssetBorder}`,
  },
}))

interface IPlaceholderBox extends PlaceholderBoxStyles {
  requestId?: string
  children?: React.ReactNode
}
const PlaceholderBox: React.FC<IPlaceholderBox> = ({width, height, requestId, children}) => {
  const classes = useStyles({width, height})
  const assetReq = useSelector(state => state.assetLab.assetRequests[requestId])

  const isLoading = assetReq?.status === 'REQUESTED' || assetReq?.status === 'PROCESSING'

  return (
    <div className={combine(classes.placeholderBox, isLoading && classes.border)}>{children}</div>
  )
}

export {
  SquarePlaceholder,
  PlaceholderBox,
}
