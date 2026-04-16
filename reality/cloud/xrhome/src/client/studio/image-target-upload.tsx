import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import type {CropResult} from '@repo/reality/shared/desktop/image-target-api'

import {SelectMenu} from './ui/select-menu'
import {FloatingPanelIconButton} from '../ui/components/floating-panel-icon-button'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {MenuOption, MenuOptions} from './ui/option-menu'
import {Icon, type IconStroke} from '../ui/components/icon'
import {
  MINIMUM_LONG_LENGTH, MINIMUM_SHORT_LENGTH,
} from '../../shared/xrengine-config'
import {
  getDefaultBottomRadius,
  getMaximumCropAreaPixels, getUnconifiedHeight, isUsableDimensions,
} from '../apps/image-targets/image-helpers'
import {makeFileName} from '../apps/image-targets/naming'
import type {IImageTarget} from '../common/types/models'
import {SubMenuHeading} from './ui/submenu-heading'
import {useStudioStateContext} from './studio-state-context'
import {useImageTargetActions, useImageTargets} from '../image-targets/use-image-targets'
import {
  getCircumferenceRatio, getConinessForRadii, getTargetCircumferenceBottom,
} from '../apps/image-targets/curved-geometry'

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
  const {uploadImageTarget} = useImageTargetActions()
  const imageTargets = useImageTargets()
  const stateCtx = useStudioStateContext()

  const onUploadFail = (error: string) => {
    stateCtx.update({imageTargetUploadProgress: undefined, errorMsg: error})
  }

  const processFile = async (
    file: File, type: IImageTarget['type']
  ) => {
    stateCtx.update({imageTargetUploadProgress: 0})
    const validName = makeFileName(file.name, imageTargets.map(it => it.name))
    const img = await createImageBitmap(file)
    const imgValid = isUsableDimensions(img.width, img.height)
    if (!imgValid) {
      onUploadFail(t('image_target_page.edit_image_target.error_invalid_image', {
        min_long_length: MINIMUM_LONG_LENGTH, min_short_length: MINIMUM_SHORT_LENGTH,
      }))
      return
    }

    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_EXIF_LOADED})

    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_IMAGE_LOADED})

    stateCtx.update({imageTargetUploadProgress: UPLOAD_PROGRESS_IMAGE_BLOB_LOADED})

    let crop: CropResult
    if (type === 'CONICAL') {
      const topRadius = DEFAULT_TOP_RADIUS
      const bottomRadius = getDefaultBottomRadius(
        img.width, img.height, topRadius
      )
      const unconifiedWidth = img.width
      const unconifiedHeight = getUnconifiedHeight(topRadius, bottomRadius, unconifiedWidth)
      const isRotated = unconifiedWidth > unconifiedHeight
      const originalWidth = isRotated ? unconifiedHeight : unconifiedWidth
      const originalHeight = isRotated ? unconifiedWidth : unconifiedHeight
      const targetCircumferenceTop = 100
      const cylinderCircumferenceTop = 100
      const arcAngle = (targetCircumferenceTop / cylinderCircumferenceTop) * 360

      const circumferenceRatio = getCircumferenceRatio(topRadius, bottomRadius)
      const cylinderCircumferenceBottom = cylinderCircumferenceTop / circumferenceRatio

      const widerTargetCircumference = Math.max(
        targetCircumferenceTop,
        getTargetCircumferenceBottom(
          targetCircumferenceTop,
          cylinderCircumferenceTop,
          cylinderCircumferenceBottom
        )
      )

      const cylinderSideLength = widerTargetCircumference *
          (unconifiedHeight / unconifiedWidth)

      const coniness = getConinessForRadii(topRadius, bottomRadius)

      crop = {
        type,
        properties: {
          arcAngle,
          bottomRadius,
          coniness,
          cylinderCircumferenceBottom,
          cylinderCircumferenceTop,
          cylinderSideLength,
          inputMode: 'ADVANCED',
          isRotated,
          originalHeight,
          originalWidth,
          targetCircumferenceTop,
          topRadius,
          unit: 'mm',
          ...getMaximumCropAreaPixels(originalWidth, originalHeight, 3 / 4),
        },
      }
    } else if (type === 'CYLINDER') {
      const isRotated = img.width > img.height
      const originalWidth = isRotated ? img.height : img.width
      const originalHeight = isRotated ? img.width : img.height
      const cylinderCircumferenceTop = 100
      const cylinderCircumferenceBottom = 100
      const targetCircumferenceTop = 50
      const arcAngle = (targetCircumferenceTop / cylinderCircumferenceTop) * 360
      const cylinderSideLength = (originalHeight / originalWidth) * 50

      crop = {
        type,
        properties: {
          arcAngle,
          coniness: 0,
          cylinderCircumferenceBottom,
          cylinderCircumferenceTop,
          cylinderSideLength,
          inputMode: 'ADVANCED',
          isRotated,
          originalHeight,
          originalWidth,
          targetCircumferenceTop,
          unit: 'mm',
          ...getMaximumCropAreaPixels(originalWidth, originalHeight, 3 / 4),
        },
      }
    } else {
      const isRotated = img.width > img.height
      const originalWidth = isRotated ? img.height : img.width
      const originalHeight = isRotated ? img.width : img.height
      crop = {
        type,
        properties: {
          isRotated,
          originalWidth,
          originalHeight,
          ...getMaximumCropAreaPixels(originalWidth, originalHeight, 3 / 4),
        },
      }
    }

    try {
      const target = await uploadImageTarget(file, validName, crop)
      stateCtx.update({imageTargetUploadProgress: undefined})
      if (target) {
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
    processFile(event.target.files[0], type)
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
