import React from 'react'
import * as ExifJs from 'exif-js'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {SelectMenu} from './ui/select-menu'
import {FloatingPanelIconButton} from '../ui/components/floating-panel-icon-button'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {MenuOption, MenuOptions} from './ui/option-menu'
import {Icon, type IconStroke} from '../ui/components/icon'
import {useCanvasPool, useImgPool} from '../common/resource-pool'
import {
  EXIF_ORIENTATION_TO_ROTATION, MINIMUM_LONG_LENGTH, MINIMUM_SHORT_LENGTH,
} from '../../shared/xrengine-config'
import {
  getDefaultBottomRadius,
  getDownScaledImage, getImageBlobFromUrl, getImageDataRotated, getImageDataWithBackground,
  getLuminosity, getMaximumCropAreaPixels, getUnconifiedHeight, isUsableDimensions, loadImage,
} from '../apps/image-targets/image-helpers'
import {
  IMAGE_TARGET_BROWSER_GALLERY_ID, IMAGE_TARGET_MAX_HEIGHT, IMAGE_TARGET_MAX_WIDTH,
} from '../apps/image-targets/image-target-constants'
import {makeFileName} from '../apps/image-targets/naming'
import useActions from '../common/use-actions'
import appsActions, {TargetGeometry} from '../apps/apps-actions'
import {useSelector} from '../hooks'
import {selectImageTargetsForApp} from '../image-targets/state-selectors'
import {useEnclosedApp} from '../apps/enclosed-app-context'
import type {IImageTarget} from '../common/types/models'
import {SubMenuHeading} from './ui/submenu-heading'
import imageTargetsActions from '../image-targets/actions'
import {useStudioStateContext} from './studio-state-context'

const DEFAULT_TOP_RADIUS = 4479
const UPLOAD_PROGRESS_EXIF_LOADED = 0.1
const UPLOAD_PROGRESS_IMAGE_LOADED = 0.2
const UPLOAD_PROGRESS_IMAGE_BLOB_LOADED = 0.3

const useStyles = createUseStyles({
  addOption: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'space-between',
    gap: '2rem',
    width: '100%',
  },
  subMenu: {
    padding: '0 0.5em',
  },
  nowrap: {
    whiteSpace: 'nowrap',
    display: 'flex',
    width: '100%',
  },
  select: {
    display: 'flex',
    flexDirection: 'column',
    paddingTop: '0.25em',
    gap: '0.5em',
  },
})

interface IImageTargetUploadInput {
  inputRefs: {
    'PLANAR': React.RefObject<HTMLInputElement>
    'CYLINDER': React.RefObject<HTMLInputElement>
    'CONICAL': React.RefObject<HTMLInputElement>
  }
  onUploadComplete?: (it: IImageTarget) => void
}

const ImageTargetUploadInput: React.FC<IImageTargetUploadInput> = ({
  inputRefs, onUploadComplete,
}) => {
  const {t} = useTranslation(['app-pages', 'cloud-studio-pages'])
  const app = useEnclosedApp()
  const imgPool = useImgPool()
  const canvasPool = useCanvasPool()
  const {uploadImageTarget} = useActions(appsActions)
  const {fetchImageTargetsForApp} = useActions(imageTargetsActions)
  const imageTargets = useSelector(s => (selectImageTargetsForApp(app.uuid, s.imageTargets))) ?? []
  const stateCtx = useStudioStateContext()

  const onUploadFail = (error: string) => {
    stateCtx.update({imageTargetUploadProgress: undefined, errorMsg: error})
  }

  const processFile = async (
    name: string, data: string, type: IImageTarget['type'], appUuid: string
  ) => {
    stateCtx.update({imageTargetUploadProgress: 0})
    const validName = makeFileName(name, imageTargets.map(it => it.name))
    const fileType = data.includes('data:image/png;') ? 'image/png' : 'image/jpeg'
    const img = await loadImage(data, imgPool)
    const imgValid = isUsableDimensions(img.naturalWidth, img.naturalHeight)
    if (!imgValid) {
      onUploadFail(t('image_target_page.edit_image_target.error_invalid_image', {
        min_long_length: MINIMUM_LONG_LENGTH, min_short_length: MINIMUM_SHORT_LENGTH,
      }))
      return
    }

    img.exifdata = null
    await new Promise<void>(resolve => ExifJs.getData(img, resolve))
    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_EXIF_LOADED})
    const scaledImg = await getDownScaledImage(
      img, IMAGE_TARGET_MAX_WIDTH, IMAGE_TARGET_MAX_HEIGHT, fileType, canvasPool, imgPool
    )

    if (!isUsableDimensions(scaledImg.naturalWidth, scaledImg.naturalHeight)) {
      if (scaledImg.naturalWidth > scaledImg.naturalHeight) {
        onUploadFail(t('image_target_page.edit_image_target.error_image_width_too_big'))
      } else {
        onUploadFail(t('image_target_page.edit_image_target.error_image_height_too_big'))
      }
      return
    }

    let rotatedImg = scaledImg
    const rotation = scaledImg.exifdata &&
      EXIF_ORIENTATION_TO_ROTATION[scaledImg.exifdata.Orientation]
    if (rotation) {
      const rotatedData = getImageDataRotated(scaledImg, rotation, fileType, canvasPool)
      rotatedImg = await loadImage(rotatedData, imgPool)
    }
    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_IMAGE_LOADED})

    const fillColor = getLuminosity(rotatedImg, canvasPool) > 128 ? 'black' : 'white'
    const isRotated = rotatedImg.naturalWidth > rotatedImg.naturalHeight
    const {dataURL} = getImageDataWithBackground(
      rotatedImg, fillColor, type !== 'CONICAL' && isRotated, fileType, canvasPool
    )

    const imgTag = await loadImage(dataURL, imgPool)
    const imgBlob = await getImageBlobFromUrl(
      imgTag, imgTag.naturalWidth, imgTag.naturalHeight, fileType, canvasPool
    )
    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_IMAGE_BLOB_LOADED})

    let geometry: TargetGeometry
    if (type === 'CONICAL') {
      const topRadius = DEFAULT_TOP_RADIUS
      const bottomRadius = getDefaultBottomRadius(
        imgTag.naturalWidth, imgTag.naturalHeight, topRadius
      )
      const unconifiedWidth = imgTag.naturalWidth
      const unconifiedHeight = getUnconifiedHeight(topRadius, bottomRadius, unconifiedWidth)
      const originalWidth = isRotated ? unconifiedHeight : unconifiedWidth
      const originalHeight = isRotated ? unconifiedWidth : unconifiedHeight
      geometry = {isRotated, originalWidth, originalHeight, topRadius, bottomRadius}
    } else {
      const originalWidth = imgTag.naturalWidth
      const originalHeight = imgTag.naturalHeight
      geometry = {isRotated, originalWidth, originalHeight}
    }

    const crop = getMaximumCropAreaPixels(geometry.originalWidth, geometry.originalHeight, 3 / 4)

    try {
      const target = await uploadImageTarget(appUuid, imgBlob, validName, type, crop, geometry)
      stateCtx.update({imageTargetUploadProgress: undefined})
      if (target) {
        fetchImageTargetsForApp(app.uuid, IMAGE_TARGET_BROWSER_GALLERY_ID)
        onUploadComplete?.(target)
        stateCtx.selectImageTarget(target.uuid)
      } else {
        onUploadFail(t('file_browser.image_targets.add.error.unknown', {ns: 'cloud-studio-pages'}))
      }
    } catch (e) {
      if (e instanceof Error) {
        // TODO(owenmech) J8W-4698 parse errors into strings from UX
        onUploadFail(e.message)
      }
    }
  }

  const handleFileUpload = (type: IImageTarget['type']) => (
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    const file = event.target.files[0]
    const reader = new FileReader()
    reader.onload = (e) => {
      if (typeof e.target.result === 'string') {
        processFile(file.name, e.target.result, type, app.uuid)
      }
    }
    reader.readAsDataURL(file)
  }

  return (
    <>
      <input
        id='flat-file-upload'
        className='hidden-input'
        type='file'
        accept='.jpg,.jpeg,.png'
        ref={inputRefs.PLANAR}
        onChange={handleFileUpload('PLANAR')}
        onClick={(e) => { e.currentTarget.value = null }}
      />
      <input
        id='cylindrical-file-upload'
        className='hidden-input'
        type='file'
        accept='.jpg,.jpeg,.png'
        ref={inputRefs.CYLINDER}
        onChange={handleFileUpload('CYLINDER')}
        onClick={(e) => { e.currentTarget.value = null }}
      />
      <input
        id='conical-file-upload'
        className='hidden-input'
        type='file'
        accept='.jpg,.jpeg,.png'
        ref={inputRefs.CONICAL}
        onChange={handleFileUpload('CONICAL')}
        onClick={(e) => { e.currentTarget.value = null }}
      />
    </>
  )
}

const useImageTargetUpload = (): {
  options: {content: React.ReactNode, onClick: () => void}[]
  inputRefs: Record<'PLANAR' | 'CYLINDER' | 'CONICAL', React.RefObject<HTMLInputElement>>
} => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  const flatInputRef = React.useRef<HTMLInputElement>(null)
  const cylindricalInputRef = React.useRef<HTMLInputElement>(null)
  const conicalInputRef = React.useRef<HTMLInputElement>(null)

  const inputRefs = {
    'PLANAR': flatInputRef,
    'CYLINDER': cylindricalInputRef,
    'CONICAL': conicalInputRef,
  }

  const onAddClicked = (type: IImageTarget['type']) => {
    inputRefs[type]?.current?.click()
  }

  const createAddOption = (label: string, stroke: IconStroke) => (
    <div className={classes.addOption}>
      {t(label)}
      <Icon stroke={stroke} />
    </div>
  )

  const options = [
    {
      content: createAddOption('file_browser.image_targets.add.flat', 'flatTarget'),
      onClick: () => onAddClicked('PLANAR'),
    },
    {
      content: createAddOption('file_browser.image_targets.add.cylindrical', 'cylindricalTarget'),
      onClick: () => onAddClicked('CYLINDER'),
    },
    {
      content: createAddOption('file_browser.image_targets.add.conical', 'conicalTarget'),
      onClick: () => onAddClicked('CONICAL'),
    },
  ]

  return {options, inputRefs}
}

interface IAddImageTargetButton {
  options: MenuOption[]
}

const AddImageTargetButton: React.FC<IAddImageTargetButton> = ({options}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const menuStyles = useStudioMenuStyles()

  return (
    <SelectMenu
      id='image-target-browser-corner-dropdown'
      trigger={(
        <FloatingPanelIconButton
          text={t('file_browser.image_targets.add.label')}
          stroke='plus'
        />
      )}
      menuWrapperClassName={menuStyles.studioMenu}
      placement='right-start'
      margin={16}
      minTriggerWidth
    >
      {collapse => (
        <MenuOptions
          options={options}
          collapse={collapse}
        />
      )}
    </SelectMenu>
  )
}

interface IAddImageTargetSubMenu {
  onBackClick: () => void
  collapse: () => void
  options: MenuOption[]
}

const AddImageTargetSubMenu: React.FC<IAddImageTargetSubMenu> = ({
  onBackClick, collapse, options,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()

  return (
    <>
      <div className={classes.subMenu}>
        <SubMenuHeading
          title={t('image_target_configurator_menu.add_new_target.label')}
          onBackClick={onBackClick}
          compact
        />
      </div>
      <div className={classes.select}>
        <MenuOptions
          collapse={collapse}
          options={options}
        />
      </div>
    </>
  )
}

export {
  AddImageTargetButton,
  AddImageTargetSubMenu,
  DEFAULT_TOP_RADIUS,
  useImageTargetUpload,
  ImageTargetUploadInput,
}
