import * as React from 'react'
import Cropper from 'react-easy-crop'
import {Input} from 'semantic-ui-react'
import {useTranslation} from 'react-i18next'

import {CanvasPool, ImgPool, ImgResource, useCanvasPool, useImgPool} from './resource-pool'
import '../static/styles/image-cropper.scss'
import {Icon} from '../ui/components/icon'

interface ImageSize {
  width: number
  height: number
}

interface CropArea {
  x: number
  y: number
  width: number
  height: number
}

interface CropAreaPixels {
  left: number
  top: number
  width: number
  height: number
}

interface ImageFile {
  file: any
  width: number
  height: number
}

type GenerateFunc = () => Promise<[ImageFile, ImageFile, CropArea]>

interface IImageCropper {
  file?: any
  minHeight: number
  minWidth: number
  onCropComplete(
    cropArea: CropArea, cropAreaPixels: CropArea, generateResultFunc: GenerateFunc
  ): void
  onError: Function
}

const getMaxZoom = (
  baseWidth: number,
  baseHeight: number,
  minWidth: number,
  minHeight: number
) => (
  Math.min(baseWidth / minWidth, baseHeight / minHeight)
)

const loadImageFile = (file: Blob): Promise<string> => new Promise((resolve, reject) => {
  const reader = new FileReader()
  reader.onload = (e) => {
    if (typeof e.target.result !== 'string') {
      reject(new Error('Failed to load image file'))
    } else {
      resolve(e.target.result)
    }
  }
  reader.readAsDataURL(file)
})

const loadImage = (
  url: string, imgPool: ImgPool
): Promise<ImgResource> => new Promise((resolve) => {
  const img = imgPool.get()
  img.onload = () => {
    resolve(img)
  }

  if (img.complete && img.src === url) {
    // This source has already been loaded on this element. Simply resolve.
    resolve(img)
    return
  }
  img.src = url
})

const imageToFile = (
  url: string, fileType: string, imgPool: ImgPool, canvasPool: CanvasPool
): Promise<ImageFile> => loadImage(url, imgPool).then((img) => {
  const canvas = canvasPool.get()
  const ctx = canvas.getContext('2d')

  canvas.width = img.naturalWidth
  canvas.height = img.naturalHeight

  ctx.drawImage(img, 0, 0)

  return new Promise((resolve) => {
    canvas.toBlob((file) => {
      canvas.release()
      img.release()
      resolve({
        file,
        width: canvas.width,
        height: canvas.height,
      })
    }, fileType)
  })
})

const croppedImageToFile = (
  url: string, fileType: string, imgPool: ImgPool, canvasPool: CanvasPool,
  croppedImagePixels: CropArea
): Promise<ImageFile> => loadImage(url, imgPool).then((img) => {
  const canvas = canvasPool.get()
  const ctx = canvas.getContext('2d')

  canvas.width = croppedImagePixels.width
  canvas.height = croppedImagePixels.height

  ctx.drawImage(
    img,
    croppedImagePixels.x,
    croppedImagePixels.y,
    croppedImagePixels.width,
    croppedImagePixels.height,
    0,
    0,
    croppedImagePixels.width,
    croppedImagePixels.height
  )

  return new Promise((resolve) => {
    canvas.toBlob((file) => {
      canvas.release()
      img.release()
      resolve({
        file,
        width: canvas.width,
        height: canvas.height,
      })
    }, fileType)
  })
})

const ImageCropper: React.FunctionComponent<IImageCropper> = (
  {file, minHeight, minWidth, onCropComplete, onError}
) => {
  const {t} = useTranslation('app-pages')
  const [crop, setCrop] = React.useState({x: 0, y: 0})
  const [zoom, setZoom] = React.useState(1)
  const [image, setImage] = React.useState({url: null, width: 0, height: 0, fileType: null})
  const imgPool = useImgPool()
  const canvasPool = useCanvasPool()

  const onCropCompleteInternal = (croppedArea: CropArea, croppedAreaPixels: CropArea) => {
    onCropComplete(
      croppedArea,
      croppedAreaPixels,
      () => Promise.all([
        imageToFile(image.url, image.fileType, imgPool, canvasPool),
        croppedImageToFile(image.url, image.fileType, imgPool, canvasPool, croppedAreaPixels),
        Promise.resolve(croppedAreaPixels),
      ])
    )
  }

  React.useEffect(() => {
    loadImageFile(file).then((url) => {
      const fileType = url.includes('data:image/png;') ? 'image/png' : 'image/jpeg'
      loadImage(url, imgPool).then((img) => {
        if (img.naturalWidth >= minWidth && img.naturalHeight >= minHeight) {
          setImage({url, width: img.naturalWidth, height: img.naturalHeight, fileType})
        } else {
          onError(t('project_settings_page.edit_basic_info_card.image_cropper.error',
            {minWidth, minHeight}))
        }
        img.release()
      })
    }).catch(e => onError(e.message))
  }, [])

  const maxZoom = Math.max(1, getMaxZoom(image.width, image.height, minWidth, minHeight))
  return (
    <div className='image-cropper-container'>
      <div className='image-cropper'>
        {image.url &&
          <Cropper
            image={image.url}
            crop={crop}
            zoom={zoom}
            maxZoom={maxZoom}
            aspect={minWidth / minHeight}
            onCropChange={setCrop}
            onCropComplete={onCropCompleteInternal}
            onZoomChange={setZoom}
          />
        }
      </div>
      <div className='slider-bar'>
        <Icon inline stroke='regionSelect' size={1.5} />
        <Input
          type='range'
          className='thumb-slider'
          value={zoom}
          min='1'
          step='0.001'
          max={maxZoom}
          onChange={e => setZoom(Number(e.target.value))}
        />
      </div>
    </div>
  )
}

export {
  ImageCropper as default,
  ImageSize,
  CropArea,
  CropAreaPixels,
  ImageFile,
  GenerateFunc,
  loadImage,
  loadImageFile,
}
