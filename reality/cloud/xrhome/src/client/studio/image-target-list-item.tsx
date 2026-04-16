import React from 'react'
import {useTranslation} from 'react-i18next'

import type {IImageTarget} from '../common/types/models'
import {createThemedStyles} from '../ui/theme'
import {Icon} from '../ui/components/icon'
import type {ImageTargetType} from '../common/types/db'
import {useStudioStateContext} from './studio-state-context'
import {combine} from '../common/styles'
import {SrOnly} from '../ui/components/sr-only'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import InlineTextInput from '../common/inline-text-input'
import {Loader} from '../ui/components/loader'
import {useTreeElementStyles} from './ui/tree-element-styles'
import {DeleteImageTargetModal} from './image-target-delete-modal'
import {useOtherImageNames, validateImageTargetName} from '../../shared/validate-image-target-name'
import {ImageTargetModal} from './image-target-modal'
import {useImageTargetActions} from '../image-targets/use-image-targets'

const useStyles = createThemedStyles(theme => ({
  listItem: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'padding': '5px 12px',
    'position': 'relative',
    'borderColor': 'transparent',
    'borderWidth': '1px 0 1px 0',
    'borderStyle': 'solid',
    '&:hover': {
      'backgroundColor': theme.studioTreeHoverBg,
      'borderStyle': 'solid',
      'borderColor': theme.sfcBorderFocus,
      'borderWidth': '1px 0 1px 0',
    },
  },
  imgContainer: {
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'width': '50px',
    'minWidth': '50px',
    'height': '66px',
    'boxSizing': 'border-box',
    'backgroundColor': theme.studioAssetBorder,
    'border': `2px solid ${theme.studioAssetBorder}`,
    'borderRadius': '4px',
    '& > img': {
      borderRadius: '3px',
      width: '100%',
      height: '100%',
      objectFit: 'cover',
    },
  },
  compactImgContainer: {
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'width': '24px',
    'minWidth': '24px',
    'height': '24px',
    'boxSizing': 'border-box',
    'backgroundColor': theme.studioAssetBorder,
    'border': `2px solid ${theme.studioAssetBorder}`,
    'borderRadius': '4px',
    '& > img': {
      borderRadius: '3px',
      width: '100%',
      height: '100%',
      objectFit: 'cover',
    },
  },
  badgeContainer: {
    position: 'absolute',
    top: '6px',
    left: '72px',
  },
  nameContainer: {
    flexGrow: 1,
    textOverflow: 'ellipsis',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
    padding: '1px',
    margin: '0 12px',
  },
  iconContainer: {
    width: '16px',
    minWidth: '16px',
    height: '16px',
  },
  input: {
    width: '100%',
  },
  loading: {
    opacity: 0.5,
  },
  modalTrigger: {
    display: 'none',
  },
  iconOptionContainer: {
    width: '16px',
    minWidth: '16px',
    height: '16px',
    marginRight: '0.5rem',
  },
}))

const typeToIcon = (type: ImageTargetType) => {
  switch (type) {
    case 'CYLINDER':
      return 'cylindricalTarget'
    case 'CONICAL':
      return 'conicalTarget'
    default:
      return 'flatTarget'
  }
}

const typeToLabel = (type: ImageTargetType) => {
  switch (type) {
    case 'CYLINDER':
      return 'file_browser.image_targets.cylindrical'
    case 'CONICAL':
      return 'file_browser.image_targets.conical'
    default:
      return 'file_browser.image_targets.flat'
  }
}

interface IImageTargetListItem {
  imageTarget: IImageTarget
  disabled?: boolean
}

const ImageTargetListItem: React.FC<IImageTargetListItem> = ({
  imageTarget, disabled = false,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const stateCtx = useStudioStateContext()
  const classes = useStyles()
  const elementClasses = useTreeElementStyles()
  const menuState = useContextMenuState()
  const {updateImageTarget} = useImageTargetActions()
  const otherImageNames = useOtherImageNames(imageTarget.uuid)

  const [loading, setLoading] = React.useState(false)
  const [renaming, setRenaming] = React.useState(false)
  const [newName, setNewName] = React.useState('')
  const [nameError, setNameError] = React.useState<string | null>(null)
  const deleteModalTriggerRef = React.useRef<HTMLButtonElement>(null)
  const renameModalTriggerRef = React.useRef<HTMLButtonElement>(null)

  const selected = stateCtx.state.selectedImageTarget === imageTarget.uuid

  const onRenameModalSubmit = () => {
    setRenaming(true)
    setNewName(imageTarget.name)
  }

  const onRenameClicked = () => {
    renameModalTriggerRef.current?.click()
  }

  const onRenameSubmit = async () => {
    setRenaming(false)
    if (nameError) {
      setNewName('')
      return
    }

    setLoading(true)
    try {
      await updateImageTarget(imageTarget.name, {
        name: newName,
      })
    } catch (e) {
      // TODO(owenmech): J8W-4698 parse error and convert to translated strings from UX
      stateCtx.update(p => ({...p, errorMsg: e.message}))
    }
    setLoading(false)
  }

  const onDeleteClicked = () => {
    deleteModalTriggerRef.current?.click()
  }

  const options = [
    {content: t('file_browser.image_targets.context_menu.rename'), onClick: onRenameClicked},
    {content: t('file_browser.image_targets.context_menu.delete'), onClick: onDeleteClicked},
  ]

  const deleteModalTrigger = (
    <button
      type='button'
      className={classes.modalTrigger}
      ref={deleteModalTriggerRef}
      aria-label={t('file_browser.image_targets.context_menu.delete')}
    />
  )

  const renameModalTrigger = (
    <button
      type='button'
      className={classes.modalTrigger}
      ref={renameModalTriggerRef}
      aria-label={t('file_browser.image_targets.context_menu.rename')}
    />
  )

  return (
    <>
      <button
        className={combine('style-reset', classes.listItem, loading && classes.loading,
          selected && elementClasses.selectedButton)}
        type='button'
        disabled={disabled}
        draggable={!renaming && !disabled}
        onClick={() => {
          stateCtx.selectImageTarget(imageTarget.uuid)
        }}
        onContextMenu={menuState.handleContextMenu}
        onDragStart={(e) => {
          if (BuildIf.STATIC_IMAGE_TARGETS_20250721) {
            e.dataTransfer.setData('imageTargetMetadata', imageTarget.metadata)
          }
          e.dataTransfer.setData('imageTargetName', imageTarget.name)
        }}
        {...menuState.getReferenceProps()}
      >
        <div className={classes.imgContainer}>
          <img
            src={imageTarget.thumbnailImageSrc}
            alt={imageTarget.name}
          />
        </div>
        <div className={classes.nameContainer}>
          {renaming
            ? <InlineTextInput
                value={newName}
                onChange={(e) => {
                  setNewName(e.target.value)
                  const error = validateImageTargetName(e.target.value, {otherImageNames})
                  setNameError(error)
                  // TODO(owenmech): J8W-4774 replace with inline error state
                  stateCtx.update(p => ({...p, errorMsg: error}))
                }}
                onCancel={() => {
                  setRenaming(false)
                  setNewName('')
                }}
                onSubmit={onRenameSubmit}
                onFocus={e => e.target.select()}
                inputClassName={combine('style-reset', elementClasses.renaming, classes.input)}
            />
            : (loading && newName) || imageTarget.name}
        </div>
        <div className={classes.iconContainer}>
          <SrOnly>{t(typeToLabel(imageTarget.type))}</SrOnly>
          <Icon stroke={typeToIcon(imageTarget.type)} />
        </div>
        {loading && <Loader />}
        <ContextMenu
          menuState={menuState}
          options={options}
        />
      </button>
      <DeleteImageTargetModal
        trigger={deleteModalTrigger}
        imageTarget={imageTarget}
      />
      <ImageTargetModal
        trigger={renameModalTrigger}
        onSubmit={onRenameModalSubmit}
        heading={t('file_browser.image_targets.edit_modal.header')}
        body={t('file_browser.image_targets.edit_modal.body')}
        submitLabel={t('file_browser.image_targets.edit_modal.confirm')}
        cancelLabel={t('file_browser.image_targets.edit_modal.cancel')}
      />
    </>
  )
}

export {
  ImageTargetListItem,
  typeToIcon,
  typeToLabel,
}
