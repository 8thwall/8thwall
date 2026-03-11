import React from 'react'
import {useTranslation} from 'react-i18next'
import {BASIC} from '@ecs/shared/environment-maps'

import {DropTarget} from '../../studio/ui/drop-target'
import type {UiTheme} from '../../ui/theme'
import {useAssetLabStateContext} from '../asset-lab-context'
import {ImagePromptPreview} from './image-prompt-preview'
import {Icon} from '../../ui/components/icon'
import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import {combine} from '../../common/styles'
import StudioModelPreviewWithLoader from '../../studio/studio-model-preview-with-loader'
import {useSelector} from '../../hooks'
import {getAssetGenUrl} from '../urls'
import {AssetLabButton} from './asset-lab-button'

const useStyles = createCustomUseStyles<{size: number}>()((theme: UiTheme) => ({
  imageDropFieldContainer: {
    maxWidth: '18rem',
  },
  imageDropHovered: {
    outline: '2px solid #44a !important',
  },
  imageDropContainer: {
    position: 'relative',
    maxWidth: ({size}) => `${size}px`,
    maxHeight: ({size}) => `${size}px`,
    minHeight: ({size}) => `${size}px`,
    height: '100%',
    width: '100%',
    borderRadius: ({size}) => (size > 200 ? '10px' : '3px'),
    border: `1px solid ${theme.studioAssetBorder}`,
    aspectRatio: '1 / 1',
    display: 'flex',
    justifyContent: 'center',
    overflow: 'hidden',
  },
  imageDropAsset: {
    'height': '100%',
    'width': '100%',
    'display': 'flex',
    'color': theme.fgMuted,
    'flexDirection': 'column',
    'justifyContent': 'center',
    'alignItems': 'center',
    '& img': {
      width: '100%',
      height: '100%',
      objectFit: 'cover',
    },
    'gap': '0.3rem',
  },
  imageDropButton: {
    background: 'none',
    border: 'none',
    color: theme.fgMuted,
    cursor: 'pointer',
    fontSize: 'inherit',
    display: 'inline',
  },
  imageDropLabel: {
    textAlign: 'center',
  },
  bottomLeft: {
    position: 'absolute',
    bottom: '0',
    left: '0',
    padding: '0.5rem',
  },
  topRight: {
    position: 'absolute',
    top: '0',
    right: '0',
    padding: '0.5rem',
    zIndex: 5,
  },
}))

interface IAssetGenImageDrop {
  size?: number
  bottomLeftContent?: React.ReactNode
  onDrop?: (file: File) => void
  onClear?: () => void
  uuidOrFile?: string | File
  onAddFromLibrary?: () => void
  id?: string
}
// Use AssetLab context to store the input image file. Currently only support 1 component of this
// in the entire context. Otherwise, they all edit the same state in the context.
const AssetGenImageDrop: React.FC<IAssetGenImageDrop> = ({
  size = 180, bottomLeftContent, onDrop, onClear, uuidOrFile,
  onAddFromLibrary, id = 'asset-gen-image-drop',
}) => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles({size})
  const [hovering, setHovering] = React.useState(false)
  const assetLabCtx = useAssetLabStateContext()
  const {mode} = assetLabCtx.state
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)

  const isImageInputMode = mode === 'image' || mode === 'model'
  const isMeshInputMode = mode === 'animation'

  return (
    <div className={classes.imageDropContainer}>
      <DropTarget
        onHoverStart={() => setHovering(true)}
        onHoverStop={() => setHovering(false)}
        onDrop={(e) => {
          e.preventDefault()
          e.stopPropagation()
          const file = e.dataTransfer?.files?.[0]
          setHovering(false)
          onDrop?.(file)
        }}
        className={combine(hovering ? classes.imageDropHovered : '', classes.imageDropAsset)}
      >
        {uuidOrFile
          ? (
            <>
              <button
                type='button'
                onClick={() => {
                  onClear?.()
                }}
                className={combine(classes.topRight, classes.imageDropButton)}
              >
                <Icon stroke='cancel' size={1.5} />
              </button>
              {isImageInputMode &&
                <ImagePromptPreview
                  imagePrompt={uuidOrFile}
                  alt={t('asset_lab.image_prompt_preview')}
                />
              }
              {isMeshInputMode && (
                <StudioModelPreviewWithLoader
                  src={
                    uuidOrFile instanceof File
                      ? URL.createObjectURL(uuidOrFile)
                      : getAssetGenUrl(assetGenerations[uuidOrFile])
                  }
                  envName={BASIC}
                />
              )}
            </>
          )
          : (
            <>
              <Icon stroke={isImageInputMode ? 'image' : 'meshCube'} size={2} />
              <span className={classes.imageDropLabel}>
                {t(`asset_lab.drag_and_drop_${isImageInputMode ? 'image' : 'model'}_prompt`)}&nbsp;
                <label
                  className={classes.imageDropButton}
                  htmlFor={`${id}-input`}
                >
                  <input
                    id={`${id}-input`}
                    type='file'
                    accept={isImageInputMode ? 'image/*' : 'model/*'}
                    style={{display: 'none'}}
                    onChange={(e) => {
                      e.preventDefault()
                      e.stopPropagation()
                      const file = e.target.files?.[0]
                      e.target.value = ''
                      onDrop?.(file)
                    }}
                  />
                  <u>{t('asset_lab.browse')}</u>
                </label>
              </span>
              <AssetLabButton onClick={onAddFromLibrary}>
                {t('asset_lab.add_from_library')}
              </AssetLabButton>
            </>
          )}
        {bottomLeftContent && <div className={classes.bottomLeft}>{bottomLeftContent}</div>}
      </DropTarget>
    </div>
  )
}

export {
  AssetGenImageDrop,
}
