import React from 'react'
import Cropper from 'react-easy-crop'
import {createUseStyles} from 'react-jss'

import type {IImageTarget} from '../../common/types/models'
import type {CropArea} from '../../common/image-cropper'
import {getMaxZoom, ImageInfo, isValidDimensions} from '../../apps/image-targets/image-helpers'
import {MINIMUM_LONG_LENGTH, MINIMUM_SHORT_LENGTH} from '../../../shared/xrengine-config'
import type {ImageTargetMetadata} from './image-target-geometry-configurator'
import {useEvent} from '../../hooks/use-event'

interface TrackingRegion {
  top: number
  left: number
  width: number
  height: number
}

const useStyles = createUseStyles({
  imageCrop: {
    position: 'relative',
    aspectRatio: '3 / 4',
  },
})

const isFlatCropPixelsValid = (width: number, height: number) => (
  width >= MINIMUM_SHORT_LENGTH && height >= MINIMUM_LONG_LENGTH
)

interface IImageTargetTrackingRegion {
  imageTarget: IImageTarget
  crop: {x: number, y: number}
  zoom: number
  maxZoom: number
  isRotated: boolean
  fullImageUnrotated?: ImageInfo
  fullImageRotated?: ImageInfo
  onMaxZoomChange: (maxZoom: number) => void
  onCropChange: (crop: {x: number, y: number}) => void
  onZoomChange: (zoom: number) => void
  onTrackingRegionChange: (region: TrackingRegion) => void
}

const ImageTargetTrackingRegion: React.FC<IImageTargetTrackingRegion> = ({
  imageTarget, crop, zoom, maxZoom, isRotated, fullImageUnrotated, fullImageRotated, onCropChange,
  onZoomChange, onTrackingRegionChange, onMaxZoomChange,
}) => {
  const classes = useStyles()
  const fullImage = isRotated ? fullImageRotated : fullImageUnrotated

  const metadata: ImageTargetMetadata =
    React.useMemo(() => JSON.parse(imageTarget.metadata), [imageTarget.metadata])

  React.useEffect(() => {
    const newMaxZoom = getMaxZoom(
      fullImageUnrotated?.data?.width || metadata.originalWidth,
      fullImageUnrotated?.data?.height || metadata.originalHeight,
      isRotated
    )
    onMaxZoomChange(newMaxZoom)
    onZoomChange(Math.max(Math.min(newMaxZoom, zoom), 1))
    onCropChange({x: 0, y: 0})
  }, [isRotated, fullImageUnrotated])

  const getAspect = (): number => {
    if (imageTarget.type === 'CYLINDER' || imageTarget.type === 'CONICAL') {
      return isRotated ? 4 / 3 : 3 / 4
    }

    return 3 / 4
  }

  const getImageUrl = (): string | undefined => {
    if (imageTarget.type === 'CYLINDER' || imageTarget.type === 'CONICAL') {
      return fullImageUnrotated?.url
    }
    return fullImage?.url
  }

  const onCropComplete = useEvent((areaPixels: CropArea) => {
    const initialImageIsValid = isValidDimensions(
      fullImageUnrotated.data.width, fullImageUnrotated.data.height, isRotated
    )
    const croppedAreaIsValid = isValidDimensions(
      areaPixels.width,
      areaPixels.height,
      (imageTarget.type === 'CONICAL' || imageTarget.type === 'CYLINDER') ? isRotated : false
    )

    // Rounding issues from Cropper can cause us to get an image cropped to 479x639 for example.
    // This fixes that by setting the crop dimensions back up to the minimum size, since we know
    // that the initial image is valid.
    if (!croppedAreaIsValid && initialImageIsValid) {
      // Prevent extra added pixels to be off the edge of the image
      areaPixels.x = Math.min(areaPixels.x, fullImage.data.width - MINIMUM_SHORT_LENGTH)
      areaPixels.y = Math.min(areaPixels.y, fullImage.data.height - MINIMUM_LONG_LENGTH)

      areaPixels.height = MINIMUM_LONG_LENGTH
      areaPixels.width = MINIMUM_SHORT_LENGTH
    }

    switch (imageTarget.type) {
      case 'CONICAL':
      case 'CYLINDER':
        onTrackingRegionChange({
          top: isRotated ? Math.floor(areaPixels.x) : Math.floor(areaPixels.y),
          left: isRotated
            ? fullImageUnrotated.data.height - Math.floor(areaPixels.y) -
            Math.floor(areaPixels.height)
            : Math.floor(areaPixels.x),
          width: isRotated
            ? Math.floor(areaPixels.height)
            : Math.floor(areaPixels.width),
          height: isRotated
            ? Math.floor(areaPixels.width)
            : Math.floor(areaPixels.height),
        })
        break
      default:
        onTrackingRegionChange({
          left: Math.floor(areaPixels.x),
          top: Math.floor(areaPixels.y),
          width: Math.floor(areaPixels.width),
          height: Math.floor(areaPixels.height),
        })
    }
  })

  const cropperImage = getImageUrl()
  return (
    <div className={classes.imageCrop}>
      {cropperImage && (
        <Cropper
          image={cropperImage}
          crop={crop}
          zoom={zoom}
          maxZoom={maxZoom}
          aspect={getAspect()}
          onCropChange={onCropChange}
          onZoomChange={onZoomChange}
          onCropComplete={(_, croppedAreaPixels: CropArea) => {
            onCropComplete(croppedAreaPixels)
          }}
        />
      )}
    </div>
  )
}

export {
  ImageTargetTrackingRegion,
  TrackingRegion,
  isFlatCropPixelsValid,
}
