import React from 'react'

import type {Resource} from '@ecs/shared/scene-graph'
import {inferResourceObject} from '@ecs/shared/resource'

import {useTranslation} from 'react-i18next'

import {StandardFieldContainer} from '../../ui/components/standard-field-container'

import {combine} from '../../common/styles'
import {SelectMenu} from './select-menu'
import {useStudioMenuStyles} from './studio-menu-styles'
import {Icon} from '../../ui/components/icon'
import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {IconButton} from '../../ui/components/icon-button'
import {useResourceUrl} from '../hooks/resource-url'
import {SrOnly} from '../../ui/components/sr-only'
import {useStyles as useRowStyles} from '../configuration/row-fields'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {createThemedStyles} from '../../ui/theme'
import {DropTarget} from './drop-target'
import {basename, fileExt} from '../../editor/editor-common'
import {isAssetPath} from '../../common/editor-files'
import {ASSET_EXT_TO_KIND, Kind, getFilesByKind} from '../common/studio-files'
import {useEphemeralEditState} from '../configuration/ephemeral-edit-state'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {useScopedGit} from '../../git/hooks/use-current-git'
import {compareResources} from './asset-selector'
import {useVideoThumbnail} from '../hooks/use-video-thumbnail'

const useStyles = createThemedStyles(theme => ({
  selectTiny: {
    padding: '2px',
    borderRight: 'none',
    display: 'block',
    color: theme.fgMuted,
  },
  imageContainer: {
    'width': '26px',
    'height': '26px',
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'cursor': 'pointer',
    'overflow': 'hidden',
    'borderRadius': '0.25em',
    // Note(Dale) make outline outside of child
    '&:focus': {
      outline: 'none',
      boxShadow: '0 0 0px 1px #007BFF',
    },
  },
  image: {
    width: '100%',
    height: '100%',
    objectFit: 'cover',
  },
  hovered: {
    outline: '2px solid #44a !important',
  },
}))

const validateAssetKind = (filePath: string, assetKind: Kind | Kind[]) => {
  const kind = ASSET_EXT_TO_KIND[fileExt(filePath)]
  if (Array.isArray(assetKind)) {
    return assetKind.some(k => k === kind)
  }
  return kind === assetKind
}

interface IImageOption {
  text: string
  onClick: () => void
  showCheckmark: boolean
  children?: React.ReactNode
}

const ImageOption: React.FC<IImageOption> = ({onClick, text, showCheckmark, children, ...rest}) => {
  const rowClasses = useRowStyles()
  return (
    <FloatingMenuButton
      onClick={onClick}
      {...rest}
    >
      <div className={rowClasses.selectOption}>
        <div className={rowClasses.selectText}>{text}</div>
        {showCheckmark &&
          <div className={rowClasses.checkmark}>
            <Icon stroke='checkmark' color='highlight' block />
          </div>
        }
        {children}
      </div>
    </FloatingMenuButton>
  )
}

interface ICompactImagePicker {
  resource: string | Resource
  onChange: (resource: Resource) => void
  assetKind?: Kind | Kind[]
  ariaLabel: string
}

const CompactImagePicker: React.FC<ICompactImagePicker> = ({
  resource, onChange, assetKind, ariaLabel,
}) => {
  const rowClasses = useRowStyles()
  const classes = useStyles()
  const [selectOpen, setSelectOpen] = React.useState(false)
  const [showUrlInput, setShowUrlInput] = React.useState(false)

  const [hovering, setHovering] = React.useState(false)
  const menuStyles = useStudioMenuStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const repoId = useCurrentRepoId()
  const filesByPath = useScopedGit(repoId, git => git.filesByPath)

  const currentResource: Resource | null = inferResourceObject(resource)
  const url = useResourceUrl(currentResource)
  const assetFiles = getFilesByKind(filesByPath, assetKind)

  const thumbnail = useVideoThumbnail(url)
  const isVideo = ASSET_EXT_TO_KIND[fileExt(url)] === 'video'
  const isImageReady = isVideo ? !!thumbnail : true

  const isValueSelected = (value: string) => {
    if (currentResource?.type === 'asset' && currentResource.asset === value) {
      return true
    }
    return false
  }

  const urlInputState = useEphemeralEditState<Resource | null, string>({
    value: currentResource,
    deriveEditValue: (value) => {
      if (value?.type === 'url') {
        return value.url
      }
      return ''
    },
    compareValue: compareResources,
    // NOTE(christoph): We don't trigger changes from the change handler, we use a save button
    parseEditValue: () => [false] as const,
    onChange: () => {},
  })

  const selectId = `urls-${React.useId()}`

  const handleAssetSelect = (filePath: string) => {
    onChange({type: 'asset', asset: filePath})
  }

  const handleDrop = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()
    const filePath = e.dataTransfer.getData('filePath')
    if (!filePath || !isAssetPath(filePath)) {
      return
    }
    if (!validateAssetKind(filePath, assetKind)) {
      return
    }
    handleAssetSelect(filePath)
  }

  return (
    <DropTarget
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e) => {
        handleDrop(e)
        setHovering(false)
      }}
    >
      <label htmlFor={selectId}>
        <div className={combine(
          rowClasses.flexItem, rowClasses.flexItemGroup, hovering && classes.hovered
        )}
        >
          <StandardFieldContainer>
            <SelectMenu
              id={selectId}
              menuWrapperClassName={combine(menuStyles.studioMenu, rowClasses.selectMenuContainer)}
              trigger={(
                <div aria-label={ariaLabel}>
                  {url && isImageReady
                    ? (
                      <div className={combine('style-reset', classes.imageContainer)}>
                        <img
                          src={thumbnail?.image?.toDataURL() ?? url}
                          alt={thumbnail?.image?.toDataURL() ?? url}
                          className={classes.image}
                        />
                      </div>
                    )
                    : (
                      <div
                        className={combine('style-reset', rowClasses.select,
                          rowClasses.preventOverflow, classes.selectTiny)}
                      >
                        <IconButton
                          onClick={() => setSelectOpen(!selectOpen)}
                          text={t('asset_selector.select_option')}
                          stroke='image'
                        />
                      </div>
                    )}
                </div>
              )}
              margin={4}
              placement='bottom-end'
            >
              {collapse => (
                <>
                  <ImageOption
                    text={t('asset_selector.option.none')}
                    onClick={() => {
                      onChange(null)
                      collapse()
                    }}
                    showCheckmark={!currentResource}
                  />
                  {showUrlInput
                    ? (
                      <form
                        onSubmit={(e) => {
                          e.preventDefault()
                          onChange({type: 'url', url: urlInputState.editValue})
                          collapse()
                        }}
                      >
                        <StandardTextField
                          id='image-url'
                          label={<SrOnly>{t('asset_selector.url.label')}</SrOnly>}
                          height='tiny'
                          value={urlInputState.editValue}
                          placeholder={t('asset_selector.url.placeholder')}
                          onChange={e => urlInputState.setEditValue(e.target.value)}
                          onBlur={() => {
                            setShowUrlInput(false)
                          }}
                          ref={element => element && element.focus()}
                        />
                      </form>
                    )
                    : (
                      <ImageOption
                        text='URL'
                        onClick={() => {
                          setShowUrlInput(true)
                        }}
                        showCheckmark={currentResource?.type === 'url' && !showUrlInput}
                      />
                    )}
                  {assetFiles.map(file => (
                    <ImageOption
                      key={file}
                      text={basename(file)}
                      onClick={() => {
                        onChange({type: 'asset', asset: file})
                        collapse()
                      }}
                      showCheckmark={isValueSelected(file)}
                    />
                  ))}
                </>
              )}
            </SelectMenu>
          </StandardFieldContainer>
        </div>
      </label>
    </DropTarget>
  )
}

export {
  CompactImagePicker,
  useStyles,
}
