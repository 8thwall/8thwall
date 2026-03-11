import React from 'react'

import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import {useSelector} from '../../hooks'
import {getImageUrl, useAssetGenImageUrl} from '../urls'
import {combine} from '../../common/styles'
import type {UiTheme} from '../../ui/theme'
import type {AssetGeneration} from '../../common/types/db'
import {AssetGenTypeIcon} from './asset-gen-type-icon'
import useCurrentAccount from '../../common/use-current-account'

const useStyles = createCustomUseStyles<{size: number}>()((theme: UiTheme) => ({
  square: {
    width: '100%',
    maxWidth: '100%',
    aspectRatio: '1 / 1',
    borderRadius: ({size}) => (size > 200 ? '10px' : '3px'),
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    objectFit: 'cover',
    position: 'relative',
    background: theme.studioBgMain,
  },
  image: {
    borderRadius: ({size}) => (size > 200 ? '0.75rem' : '0.25rem'),
    width: '100%',
    height: '100%',
    objectFit: 'contain',
  },
  border: {
    border: `1px solid ${theme.studioAssetBorder}`,
  },
  bottomLeft: {
    position: 'absolute',
    bottom: '0',
    left: '0',
    padding: '0.5rem',
    color: theme.overlayFg,
  },
  bottomRight: {
    position: 'absolute',
    bottom: '0',
    right: '0',
    padding: '0.5rem',
    color: theme.overlayFg,
  },
}))

type ISquareAsset = {
  size: number
  generationId: string
  requestId?: string
  children?: React.ReactNode
  onHoverContent?: (imgTag: React.MutableRefObject<HTMLImageElement> | null) => React.ReactNode
  onMouseLeave?: () => void
  bottomLeftContent?: (assetGeneration?: AssetGeneration) => React.ReactNode
  bottomRightContent?: (assetGeneration?: AssetGeneration) => React.ReactNode
}

const SquareAssetFwdRef = React.forwardRef<HTMLImageElement, ISquareAsset>(
  ({
    size, generationId, requestId = '', children, onHoverContent,
    bottomLeftContent, bottomRightContent, onMouseLeave,
  },
  imgRef) => {
    const assetGeneration = useSelector(s => s.assetLab.assetGenerations[generationId])
    const assetReq = useSelector(state => state.assetLab.assetRequests[requestId])
    const classes = useStyles({size})

    const isLoading = assetReq?.status === 'REQUESTED' || assetReq?.status === 'PROCESSING'

    const [squareHovering, setSquareHovering] = React.useState(false)
    const imgSrc = useAssetGenImageUrl(assetGeneration)

    // TODO(dat): What about alt?
    const assetContent = assetGeneration
      ? (
      // NOTE(kyle): crossOrigin='use-credentials' is not required. CDN assets have a "*" policy
      // on allowed origins and including credentials on wildcard origins is not allowed.
      // See https://github.com/8thwall/code8/pull/1072 for reference.
        <img
          ref={imgRef}
          decoding='async'
          loading='lazy'
          src={imgSrc}
          alt=''
          crossOrigin='anonymous'
          className={classes.image}
        />
      )
    // When the asset generation is not found, display nothing so we can show the overlay on top.
      : null
    return (
      <div
        className={combine(classes.square, !isLoading && classes.border)}
        onMouseEnter={() => setSquareHovering(true)}
        onMouseLeave={() => {
          setSquareHovering(false)
          onMouseLeave?.()
        }}
      >
        {assetContent}
        {children}
        {squareHovering && onHoverContent?.(imgRef as React.MutableRefObject<HTMLImageElement>)}
        {bottomLeftContent && (
          <div className={classes.bottomLeft}>
            {bottomLeftContent(assetGeneration)}
          </div>
        )}
        {bottomRightContent && (
          <div className={classes.bottomRight}>
            {bottomRightContent(assetGeneration)}
          </div>
        )}
      </div>
    )
  }
)

const SquareAsset: React.FC<ISquareAsset> = (props) => {
  const imgRef = React.useRef<HTMLImageElement | null>(null)
  return <SquareAssetFwdRef {...props} ref={imgRef} />
}

const SquareAssetWithIcon: React.FC<Omit<ISquareAsset, 'bottomRightContent'>> = props => (
  <SquareAsset {...props} bottomRightContent={ag => <AssetGenTypeIcon ag={ag} />} />
)

type ISquareImage = {
  size: number
  srcId: string
}

// NOTE(coco): Used for displaying images directly from a source ID,
// which is needed for displaying user uploaded images that don't have
// an asset generation associated with them.
const SquareImage: React.FC<ISquareImage> = ({size, srcId}) => {
  const classes = useStyles({size})
  const currentAccount = useCurrentAccount()

  return (
    <div
      className={combine(classes.square, classes.border)}
    >
      <img
        decoding='async'
        loading='lazy'
        src={getImageUrl(currentAccount?.uuid, srcId)}
        alt=''
        crossOrigin='anonymous'
        className={classes.image}
      />
    </div>
  )
}

export {SquareAsset, SquareAssetFwdRef, SquareAssetWithIcon, SquareImage}
