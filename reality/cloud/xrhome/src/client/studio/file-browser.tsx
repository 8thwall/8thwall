import React from 'react'
import {useTranslation} from 'react-i18next'

import '../static/styles/code-editor.scss'

import {FileBrowserList, ItemType} from './file-browser-list'
import {ASSET_FOLDER_PREFIX} from '../common/editor-files'
import {extractProjectFileLists} from '../editor/files/extract-file-lists'
import {FileActionsContext} from '../editor/files/file-actions-context'
import {useCurrentGit} from '../git/hooks/use-current-git'
import type {UploadState} from '../editor/hooks/use-file-upload-state'
import FileUploadProgressBar from '../editor/file-upload-progress-bar'
import {FileTreeContainer} from '../editor/file-tree-container'
import {useTheme} from '../user/use-theme'
import {createThemedStyles} from '../ui/theme'
import {combine} from '../common/styles'
import type {ScopedFileLocation} from '../editor/editor-file-location'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {FileBrowserSearchResults, FILE_BROWSER_FILTERS} from './file-browser-search-results'
import {OptionalFilters, SearchBar} from './ui/search-bar'
import {FileBrowserCornerButton} from './file-browser-corner-button'
import {StudioImageTargetBrowser} from './image-target-browser'
import editorActions from '../editor/editor-actions'
import useActions from '../common/use-actions'
import {useSelector} from '../hooks'
import {PrefabsList} from './prefabs-list'
import {Loader} from '../ui/components/loader'
import {useStudioStateContext, type FileBrowserSection} from './studio-state-context'
import {useMaybeLocalSyncContext} from './local-sync-context'
import {FloatingPanelIconButton} from '../ui/components/floating-panel-icon-button'
import {openProject} from './local-sync-api'
import {useEnclosedAppKey} from '../apps/enclosed-app-context'

const AssetLabMiniLibrary = React.lazy(() => import('../asset-lab/asset-lab-mini-library'))

const useStyles = createThemedStyles(theme => ({
  fileBrowser: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
    flexGrow: 1,
  },
  sectionTitle: {
    'cursor': 'pointer',
    'userSelect': 'none',
    '&:disabled': {
      cursor: 'default',
    },
  },
  sectionTitleContainer: {
    'padding': '0.75em 0.75em 0.5em 0.75em',
    'display': 'flex',
    'flexDirection': 'row',
    'fontWeight': 700,
    'justifyContent': 'flex-start',
    'gap': '1em',
    'borderTop': theme.studioSectionBorder,
    'color': theme.fgMuted,
  },
  sectionTitleHorizontal: {
    'overflow-x': 'auto',
    'flex': '0 0 auto',  // Since we are scrolling horizontally, don't allow to be squished.
    '&::-webkit-scrollbar': {
      height: '6px !important',
    },
  },
  sectionActive: {
    color: theme.fgMain,
  },
  fileListContainer: {
    overflowY: 'auto',
    flexGrow: 1,
    position: 'relative',
    display: 'flex',
    flexDirection: 'column',
  },
  fileTreeContainer: {
    flexGrow: 1,
    display: 'flex',
    flexDirection: 'column',
    position: 'relative',
  },
  onDropHover: {
    backgroundColor: theme.studioTreeHoverBg,
  },
  importModuleButton: {
    margin: '0.5em 1em',
  },
  searchBarContainer: {
    'padding': '0 0.75rem 0.5rem',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
  },
  collapseToggle: {
    'flexGrow': 1,
    '&:disabled': {
      cursor: 'default',
    },
  },
}))

interface IFileBrowser {
  uploadDropRef: React.RefObject<HTMLInputElement>
  handleFileUpload: (event: React.ChangeEvent<HTMLInputElement>) => void
  fileUploadState: UploadState
  activeFileLocation: ScopedFileLocation
  isStudio?: boolean
}

const FileBrowser: React.FC<IFileBrowser> = ({
  uploadDropRef, handleFileUpload, fileUploadState, activeFileLocation,
  isStudio,
}) => {
  const classes = useStyles()
  const studioStyles = useStudioMenuStyles()
  const {t} = useTranslation(['cloud-editor-pages', 'cloud-studio-pages'])

  const [searchValue, setSearchValue] = React.useState('')
  const [filters, setFilters] = React.useState<string[]>([])
  const [externalAdding, setExternalAdding] = React.useState(false)
  const [externalAddingAssets, setExternalAddingAssets] = React.useState(false)
  const [externalItemType, setExternalItemType] = React.useState<ItemType>(null)

  const appKey = useEnclosedAppKey()
  const {saveFileBrowserHeightPercent} = useActions(editorActions)
  const isFileBrowserCollapsed = useSelector(
    s => s.editor.byKey[appKey]?.fileBrowserHeightPercent === 0
  )

  const localSyncContext = useMaybeLocalSyncContext()
  const filesLoading = localSyncContext && localSyncContext.fileSyncStatus !== 'active'

  const stateCtx = useStudioStateContext()
  const {currentBrowserSection: currentSection} = stateCtx.state

  const handleExternalAdding = (adding: boolean, isAssetPath?: boolean) => {
    setExternalAddingAssets(isAssetPath && adding)
    setExternalAdding(!isAssetPath && adding)
  }

  const handleToggleCollapsed = () => {
    if (!isFileBrowserCollapsed) {
      saveFileBrowserHeightPercent(appKey, 0)
    } else {
      saveFileBrowserHeightPercent(appKey)
    }
  }

  const handleSectionClick = (section: FileBrowserSection) => {
    if (isFileBrowserCollapsed) {
      handleToggleCollapsed()
    }

    stateCtx.update(p => ({...p, currentBrowserSection: section}))
  }

  const git = useCurrentGit()
  const files = extractProjectFileLists(git, () => false)

  const {onCreate, onUploadStart} = React.useContext(FileActionsContext)

  const {
    uploadTotalNumFiles, uploadFiles, uploadTotalBytes, uploadBytesUploaded,
  } = fileUploadState

  const themeName = useTheme()

  const filesList = filesLoading
    ? <Loader size='small' />
    : (
      <FileTreeContainer
        themeName={themeName}
        hoverClassName={classes.onDropHover}
        classNameOverride={classes.fileTreeContainer}
      >
        {files.main.filePaths.length > 0 && files.main.folderPaths.length > 0 &&
          <FileBrowserList
            paths={files.main}
            currentEditorFileLocation={activeFileLocation}
          />
      }
        <FileBrowserList
          onCreateItem={onCreate}
          onFileSelect={onUploadStart}
          paths={files.text}
          currentEditorFileLocation={activeFileLocation}
          externalAdding={externalAdding}
          setExternalAdding={handleExternalAdding}
          externalItemType={externalItemType}
          setExternalItemType={setExternalItemType}
        />
        <FileBrowserList
          title={t('editor_page.file_list.assets')}
          subtitle={ASSET_FOLDER_PREFIX}
          onCreateItem={newItem => onCreate(newItem, 'assets')}
          disableNewFiles
          onFileSelect={onUploadStart}
          setExternalAdding={handleExternalAdding}
          externalAdding={externalAddingAssets}
          externalItemType={externalItemType}
          setExternalItemType={setExternalItemType}
          paths={files.assets}
          currentEditorFileLocation={activeFileLocation}
          grow
        />
        <input
          className='hidden-input'
          type='file'
          accept='*'
          ref={uploadDropRef}
          onChange={handleFileUpload}
          value=''
          multiple
        />
        <FileUploadProgressBar
          numFileUploading={uploadFiles.length}
          totalNumFiles={uploadTotalNumFiles}
          bytesUploaded={uploadBytesUploaded}
          totalBytes={uploadTotalBytes}
        />
      </FileTreeContainer>
    )

  const searchResults = (
    <FileBrowserSearchResults
      searchValue={searchValue}
      filters={filters}
      clearSearch={() => {
        setSearchValue('')
        setFilters([])
      }}
    />
  )

  return (
    <div className={combine(classes.fileBrowser, isStudio && studioStyles.studioFont)}>
      <div className={combine(classes.sectionTitleContainer, classes.sectionTitleHorizontal)}>
        <button
          a8='click;studio;file-browser-files-tab'
          type='button'
          onClick={() => handleSectionClick('files')}
          className={combine(
            'style-reset', classes.sectionTitle,
            currentSection === 'files' && classes.sectionActive
          )}
          disabled={!isFileBrowserCollapsed && currentSection === 'files'}
        >
          {t('editor_page.file_list.files')}
        </button>
        {BuildIf.STUDIO_ASSET_LAB_20260209 && (
          <button
            a8='click;studio;file-browser-asset-lab-tab'
            type='button'
            onClick={() => {
              handleSectionClick('assetLab')
              setSearchValue('')
            }}
            className={combine(
              'style-reset', classes.sectionTitle,
              currentSection === 'assetLab' && classes.sectionActive
            )}
          >
            {t('file_browser.asset_lab.label', {ns: 'cloud-studio-pages'})}
          </button>
        )}
        <button
          type='button'
          onClick={() => {
            handleSectionClick('prefabs')
            setSearchValue('')
          }}
          className={combine(
            'style-reset', classes.sectionTitle,
            currentSection === 'prefabs' && classes.sectionActive
          )}
        >
          {t('file_browser.prefabs.label', {ns: 'cloud-studio-pages'})}
        </button>
        {BuildIf.STUDIO_IMAGE_TARGETS_20260210 && (
          <button
            type='button'
            onClick={() => {
              handleSectionClick('imageTargets')
              setSearchValue('')
              setFilters([])
            }}
            className={combine(
              'style-reset', classes.sectionTitle,
              currentSection === 'imageTargets' && classes.sectionActive
            )}
            disabled={!isFileBrowserCollapsed && currentSection === 'imageTargets'}
          >
            {t('file_browser.image_targets.short_label', {ns: 'cloud-studio-pages'})}
          </button>
        )}
      </div>
      {currentSection === 'files' &&
        <div className={classes.searchBarContainer}>
          <SearchBar
            searchText={searchValue}
            setSearchText={setSearchValue}
            filterOptions={collapse => (
              <OptionalFilters
                options={FILE_BROWSER_FILTERS}
                noneOption={t('file_browser.search_bar.option.all', {ns: 'cloud-studio-pages'})}
                filters={filters}
                setFilters={setFilters}
                collapse={collapse}
              />
            )}
            a8='click;studio-bottom-left-pane;file-browser-click'
          />
          {Build8.PLATFORM_TARGET === 'desktop' &&
            <FloatingPanelIconButton
              text={t('file_configurator.button.open_externally', {ns: 'cloud-studio-pages'})}
              stroke='openFolder'
              onClick={() => openProject(appKey)}
            />
          }
          <FileBrowserCornerButton
            onFileSelect={onUploadStart}
            onCreateItem={onCreate}
            setExternalAdding={handleExternalAdding}
            setExternalItemType={setExternalItemType}
          />
        </div>
      }
      {((!searchValue && filters.length === 0) || externalItemType)
        ? (
          <div className={classes.fileListContainer}>
            {currentSection === 'files' && filesList}
            {currentSection === 'prefabs' && <PrefabsList />}
            {BuildIf.STUDIO_IMAGE_TARGETS_20260210 && currentSection === 'imageTargets' &&
              <StudioImageTargetBrowser />
            }
            {BuildIf.STUDIO_ASSET_LAB_20260209 && currentSection === 'assetLab' &&
              <React.Suspense fallback={<div><Loader animateSpinnerColor /></div>}>
                <AssetLabMiniLibrary />
              </React.Suspense>
            }
          </div>
        )
        : searchResults
      }
    </div>
  )
}

export {
  FileBrowser,
  useStyles as useFileBrowserStyles,
}
