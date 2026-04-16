import React from 'react'
import {useTranslation} from 'react-i18next'
import type {ImageTarget} from '@ecs/shared/scene-graph'

import type {IImageTarget} from '../../common/types/models'
import {combine} from '../../common/styles'
import {useSelector} from '../../hooks'
import {useEnclosedApp} from '../../apps/enclosed-app-context'
import useActions from '../../common/use-actions'
import imageTargetsActions from '../../image-targets/actions'
import {SearchBar} from '../ui/search-bar'
import {selectTargetsGalleryFilterOptions} from '../../image-targets/state-selectors'
import {useStyles as useRowFieldStyles} from './row-fields'
import {SelectMenu} from '../ui/select-menu'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {IconButton} from '../../ui/components/icon-button'
import {ImageTargetLoader} from '../../image-targets/image-target-loader'
import {
  ImageTargetMenuItem, ImageTargetNoneMenuItem, ImageTargetAddMenuItem,
} from '../image-target-menu-item'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {
  IMAGE_TARGET_ENTITY_GALLERY_ID as ENTITY_GALLERY_ID,
} from '../../apps/image-targets/image-target-constants'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {createThemedStyles} from '../../ui/theme'
import {
  AddImageTargetSubMenu, ImageTargetUploadInput, useImageTargetUpload,
} from '../image-target-upload'
import {useGalleryTargets} from '../../image-targets/use-image-targets'

const useStyles = createThemedStyles(theme => ({
  searchBarContainer: {
    'padding': '0.25rem 0.25rem 0.5rem',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
  },
  imageContainer: {
    'width': '22px',
    'height': '22px',
    'cursor': 'pointer',
    'overflow': 'hidden',
    'display': 'flex',
    '& > img': {
      borderRadius: '0.25em',
      width: '100%',
      height: '100%',
      objectFit: 'cover',
    },
  },
  targetResultsContainer: {
    overflowY: 'auto',
    flexGrow: 1,
    position: 'relative',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
    borderBottom: theme.studioSectionBorder,
    paddingBottom: '0.5em',
  },
  selectContainer: {
    width: '20rem',
    minHeight: '8rem',
    maxHeight: '18rem',
    overflowY: 'auto',
  },
  dropdownContainer: {
    width: '9.5em',
  },
  selectTiny: {
    padding: '0px',
    borderRight: 'none',
    display: 'block',
    width: '22px',
  },
  row: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
  },
  addTarget: {
    paddingTop: '0.3em',
  },
}))

interface IImageTargetThumbnail {
  imageTarget?: IImageTarget
}

const ImageTargetThumbnail: React.FC<IImageTargetThumbnail> = ({imageTarget}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()

  return (
    <div className={classes.selectTiny}>
      {imageTarget
        ? (
          <div className={classes.imageContainer}>
            <img
              src={imageTarget.thumbnailImageSrc}
              alt={imageTarget.thumbnailImageSrc}
            />
          </div>
        )
        : (
        // NOTE(chloe): StandardFieldContainer is used here as background for None stroke icon
          <StandardFieldContainer>
            <IconButton
              onClick={() => {}}
              text={t('image_target_configurator_menu.none_button.label')}
              stroke='image'
            />
          </StandardFieldContainer>
        )}
    </div>
  )
}

interface IImageTargetDropdownTrigger {
  imageTarget?: IImageTarget
}

const ImageTargetDropdownTrigger: React.FC<IImageTargetDropdownTrigger> = ({imageTarget}) => {
  const classes = useStyles()
  const rowFieldStyles = useRowFieldStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <div className={combine(classes.row, rowFieldStyles.flexItemGroup)}>
      <div className={classes.dropdownContainer}>
        <StandardFieldContainer>
          <div className={combine(rowFieldStyles.select, rowFieldStyles.preventOverflow)}>
            <div className={rowFieldStyles.selectText}>
              {imageTarget?.name ?? t('image_target_configurator_menu.none.label')}
            </div>
            <div className={rowFieldStyles.chevron} />
          </div>
        </StandardFieldContainer>
      </div>
      <ImageTargetThumbnail imageTarget={imageTarget} />
    </div>
  )
}

type MenuState = 'selecting' | 'adding'

interface IImageTargetConfiguratorMenu {
  onChange: (it: ImageTarget) => void
  imageTarget?: IImageTarget
}

const ImageTargetConfiguratorMenu: React.FC<IImageTargetConfiguratorMenu> = (
  {imageTarget, onChange}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const rowFieldStyles = useRowFieldStyles()
  const [menuState, setMenuState] = React.useState<MenuState>('selecting')
  const {options, inputRefs} = useImageTargetUpload()

  const app = useEnclosedApp()
  const imageTargets = useGalleryTargets(ENTITY_GALLERY_ID)
  const {
    resetGalleryFilterOptionsForApp, setGalleryFilterOptionsForApp,
  } = useActions(imageTargetsActions)

  const galleryOptions = useSelector(
    state => selectTargetsGalleryFilterOptions(app.uuid, ENTITY_GALLERY_ID, state.imageTargets)
  )

  const searchValue = galleryOptions.nameLike || ''

  const setSearchValue = (value: string) => {
    setGalleryFilterOptionsForApp(app.uuid, ENTITY_GALLERY_ID, {
      nameLike: value.length > 0 ? value : null,
    })
  }

  const clearSearch = () => {
    resetGalleryFilterOptionsForApp(app.uuid, ENTITY_GALLERY_ID)
  }

  React.useEffect(() => {
    clearSearch()
  }, [app.uuid])

  return (
    <label htmlFor='image-target-target-menu.selector' className={rowFieldStyles.row}>
      <div className={rowFieldStyles.flexItem}>
        <StandardFieldLabel
          label={t('image_target_configurator_menu.selector.label')}
          mutedColor
        />
      </div>
      <div className={rowFieldStyles.flexItem}>
        <SelectMenu
          id='image-target-configurator-select-menu'
          menuWrapperClassName={combine(menuStyles.studioMenu, classes.selectContainer)}
          trigger={<ImageTargetDropdownTrigger imageTarget={imageTarget} />}
          minTriggerWidth
          margin={4}
          placement='top-end'
          onDismiss={() => {
            clearSearch()
            setMenuState('selecting')
          }}
        >
          {(collapse) => {
            if (menuState === 'selecting') {
              return (
                <>
                  <div className={classes.searchBarContainer}>
                    <SearchBar
                      searchText={searchValue}
                      setSearchText={setSearchValue}
                    />
                  </div>
                  <div className={classes.targetResultsContainer}>
                    {!searchValue &&
                      <ImageTargetNoneMenuItem
                        onClick={() => {
                          onChange({name: ''})
                          setMenuState('selecting')
                          collapse()
                        }}
                        showCheckmark={!imageTarget}
                      />
                    }
                    {imageTargets?.map(target => (
                      <ImageTargetMenuItem
                        key={target.uuid}
                        showCheckmark={target.uuid === imageTarget?.uuid}
                        imageTarget={target}
                        onClick={() => {
                          onChange(
                            {
                              name: target.name,
                              staticOrientation: JSON.parse(target.metadata)
                                ?.staticOrientation,
                            }
                          )
                          setMenuState('selecting')
                          clearSearch()
                          collapse()
                        }}
                      />
                    ))}
                    <ImageTargetLoader galleryUuid={ENTITY_GALLERY_ID} />
                  </div>
                  <div className={classes.addTarget}>
                    <ImageTargetAddMenuItem onClick={() => setMenuState('adding')} />
                  </div>
                </>
              )
            } else {
              return (
                <AddImageTargetSubMenu
                  collapse={collapse}
                  onBackClick={() => setMenuState('selecting')}
                  options={options}
                />
              )
            }
          }
        }
        </SelectMenu>
      </div>
      <ImageTargetUploadInput
        inputRefs={inputRefs}
        onUploadComplete={target => onChange(({name: target.name}))}
      />
    </label>
  )
}

export {
  ImageTargetConfiguratorMenu,
}
