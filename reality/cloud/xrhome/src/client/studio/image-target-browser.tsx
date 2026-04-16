import React, {useEffect} from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {useEnclosedApp} from '../apps/enclosed-app-context'
import {useSelector} from '../hooks'
import {ImageTargetListItem} from './image-target-list-item'
import imageTargetsActions from '../image-targets/actions'
import useActions from '../common/use-actions'
import {OptionalFilters, SearchBar, SearchToolbar} from './ui/search-bar'
import type {
  ImageTargetFilterFlag,
} from '../image-targets/types'
import {combine} from '../common/styles'
import {selectTargetsGalleryFilterOptions} from '../image-targets/state-selectors'
import {
  AddImageTargetButton, ImageTargetUploadInput, useImageTargetUpload,
} from './image-target-upload'
import {ImageTargetLoader} from '../image-targets/image-target-loader'
import {
  IMAGE_TARGET_BROWSER_GALLERY_ID as BROWSER_GALLERY_ID,
} from '../apps/image-targets/image-target-constants'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import FileUploadProgressBar from '../editor/file-upload-progress-bar'
import {useStudioStateContext} from './studio-state-context'
import {useGalleryTargets} from '../image-targets/use-image-targets'
import type {ImageTargetType} from '../common/types/db'

const useStyles = createUseStyles({
  targetsContainer: {
    display: 'flex',
    flexDirection: 'column',
    flexGrow: 1,
    height: '100%',
  },
  loadingInitial: {
    margin: 'auto',
  },
  searchBarContainer: {
    'padding': '0 0.75rem 0.5rem',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
  },
  targetResultsContainer: {
    overflowY: 'auto',
    flexGrow: 1,
    position: 'relative',
    display: 'flex',
    flexDirection: 'column',
  },
  disabled: {
    pointerEvents: 'none',
    opacity: 0.5,
  },
  progressContainer: {
    position: 'absolute',
    bottom: 0,
    left: 0,
    right: 0,
  },
})

type GeometryFilter = {type: ImageTargetType, label: string}
const GEOMETRY_FILTERS: GeometryFilter[] = [{
  type: 'PLANAR',
  label: 'file_browser.image_targets.flat',
}, {
  type: 'CYLINDER',
  label: 'file_browser.image_targets.cylindrical',
}, {
  type: 'CONICAL',
  label: 'file_browser.image_targets.conical',
}]

type FlagFilter = {
  type: ImageTargetFilterFlag
  label: string
}
const FLAG_FILTERS: FlagFilter[] = [{
  type: 'hasMetadata',
  label: 'file_browser.image_targets.metadata',
}]

const IMAGE_TARGET_FILTERS = [
  ...GEOMETRY_FILTERS,
  ...FLAG_FILTERS,
]

const StudioImageTargetBrowser: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const app = useEnclosedApp()
  const stateCtx = useStudioStateContext()
  const imageTargets = useGalleryTargets(BROWSER_GALLERY_ID)
  const loadingInitial = false  // TODO(christoph): Ensure suspense is handled
  const hasMoreTargets = false  // TODO(christoph): Clean up
  const classes = useStyles()
  const menuState = useContextMenuState()
  const {options, inputRefs} = useImageTargetUpload()
  const {
    fetchImageTargetsForApp, setGalleryFilterOptionsForApp, resetGalleryFilterOptionsForApp,
  } = useActions(imageTargetsActions)

  const galleryOptions = useSelector(
    state => selectTargetsGalleryFilterOptions(app.uuid, BROWSER_GALLERY_ID, state.imageTargets)
  )

  const searchValue = galleryOptions.nameLike || ''
  const filters = [
    ...galleryOptions.type,
    ...FLAG_FILTERS.map(({type}) => (
      galleryOptions[type] ? type : null
    )).filter(Boolean),
  ]

  const searching = searchValue.length > 0 || filters.length > 0

  const clearSearch = () => {
    resetGalleryFilterOptionsForApp(app.uuid, BROWSER_GALLERY_ID)
  }

  const setSearchValue = (value: string) => {
    setGalleryFilterOptionsForApp(app.uuid, BROWSER_GALLERY_ID, {
      nameLike: value.length > 0 ? value : null,
    })
  }

  const setFilters = (newFilters: string[]) => {
    setGalleryFilterOptionsForApp(app.uuid, BROWSER_GALLERY_ID, {
      type: GEOMETRY_FILTERS.filter(({type}) => newFilters.includes(type)).map(({type}) => type),
      ...Object.fromEntries(
        FLAG_FILTERS.map(({type}) => [
          type,
          newFilters.includes(type),
        ])
      ),
    })
  }

  useEffect(() => {
    resetGalleryFilterOptionsForApp(app.uuid, BROWSER_GALLERY_ID)
    fetchImageTargetsForApp(app.uuid, BROWSER_GALLERY_ID)
  }, [app.uuid])

  return (
    <div className={classes.targetsContainer}>
      <div className={classes.searchBarContainer}>
        <SearchBar
          searchText={searchValue}
          setSearchText={setSearchValue}
          filterOptions={collapse => (
            <OptionalFilters
              options={IMAGE_TARGET_FILTERS}
              noneOption={t('file_browser.image_targets.all_targets', {ns: 'cloud-studio-pages'})}
              filters={filters}
              setFilters={setFilters}
              collapse={collapse}
            />
          )}
        />
        <AddImageTargetButton options={options} />
      </div>
      {searching &&
        <SearchToolbar
          resultCount={imageTargets.length}
          clearSearch={clearSearch}
          showingPartialResult={hasMoreTargets}
        />
      }
      <div
        className={combine(classes.targetResultsContainer, loadingInitial && classes.disabled)}
        onContextMenu={menuState.handleContextMenu}
        {...menuState.getReferenceProps()}
      >
        {imageTargets?.map(target => (
          <ImageTargetListItem
            key={target.uuid}
            imageTarget={target}
            disabled={loadingInitial}
          />
        ))}
        <ImageTargetLoader galleryUuid={BROWSER_GALLERY_ID} />
      </div>
      <ContextMenu menuState={menuState} options={options} />
      {/* TODO(owenmmech): swap to drop target */}
      <ImageTargetUploadInput inputRefs={inputRefs} />
      <div className={classes.progressContainer}>
        <FileUploadProgressBar
          numFileUploading={1}
          totalNumFiles={stateCtx.state.imageTargetUploadProgress === undefined ? 0 : 1}
          bytesUploaded={stateCtx.state.imageTargetUploadProgress ?? 1}
          totalBytes={1}
        />
      </div>
    </div>
  )
}

export {
  StudioImageTargetBrowser,
}
