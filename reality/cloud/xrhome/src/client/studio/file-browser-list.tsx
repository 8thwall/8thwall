import React from 'react'
import {useTranslation} from 'react-i18next'

import type {ISplitFiles} from '../git/split-files-by-type'
import {FileBrowserListFile} from './file-browser-list-file'
import {FileBrowserListFolder} from './file-browser-list-folder'
import {useNewFileOptions} from './hooks/use-new-file-options'
import {NewFileItem} from './file-browser-new-file-item'
import type {IOptionItem} from '../editor/files/option-item'
import {STUDIO_HIDDEN_FILE_PATHS} from './common/studio-files'
import type {NewItem} from '../editor/files/file-actions-context'
import {useTreeElementStyles} from './ui/tree-element-styles'
import {createThemedStyles} from '../ui/theme'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {combine} from '../common/styles'
import type {ScopedFileLocation} from '../editor/editor-file-location'
import {useFileSelection} from './hooks/use-file-selection'
import {openProject} from './local-sync-api'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {useEnclosedAppKey} from '../apps/enclosed-app-context'

const useStyles = createThemedStyles(theme => ({
  fileListGrow: {
    flexGrow: 1,
    display: 'flex',
    flexDirection: 'column',
  },
  fileListBar: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: '0.5em 1em',
  },
  fileListTitle: {
    margin: 0,
    fontWeight: 'bold',
    fontSize: '1em',
  },
  fileListSubtitle: {
    color: theme.fgMuted,
    marginLeft: '0.5em',
  },
  extraSpace: {
    minHeight: '1em',
  },
  extraSpaceGrow: {
    flexGrow: 1,
  },
  openButton: {
    display: 'flex',
    justifyContent: 'center',
    padding: '0.5em 0.75em',
  },
}))

type ItemType = 'file' | 'component' | 'folder'

interface IFileBrowserList {
  title?: string
  subtitle?: string
  onFileSelect?: Function
  paths: ISplitFiles
  currentEditorFileLocation: ScopedFileLocation
  onCreateItem?: (newItem: {name: string, isFile: boolean}) => void
  onCreateBackendConfig?: () => void
  disableNewFiles?: boolean
  additionalOptions?: IOptionItem[]
  productTourId?: string
  externalAdding?: boolean
  setExternalAdding?: (adding: boolean) => void
  externalItemType?: ItemType
  setExternalItemType?: (itemType: ItemType) => void
  grow?: boolean
}

const FileBrowserList: React.FC<IFileBrowserList> = ({
  title, subtitle, onFileSelect, paths, currentEditorFileLocation,
  disableNewFiles, onCreateItem, additionalOptions = [], productTourId, onCreateBackendConfig,
  externalAdding, setExternalAdding, externalItemType, setExternalItemType, grow,
}) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const classes = useStyles()
  const appKey = useEnclosedAppKey()

  const menuState = useContextMenuState()

  const [adding, setAdding] = React.useState(false)
  const [itemType, setItemType] = React.useState<ItemType>(null)
  const elementClasses = useTreeElementStyles()

  const {onFileClick} = useFileSelection(paths)

  const isAdding = externalAdding || adding
  const showOpenExternally = Build8.PLATFORM_TARGET === 'desktop' && !title && !isAdding &&
  paths.filePaths.filter(f => !STUDIO_HIDDEN_FILE_PATHS.includes(f)).length === 0

  const startNewItem = (newItem: ItemType) => {
    setItemType(newItem)
    setAdding(true)
  }

  const cancelNewItem = () => {
    setAdding(false)
    if (setExternalAdding) { setExternalAdding(false) }
    setItemType(null)
    setExternalItemType(null)
  }

  const handleCreateItem = (newItem: NewItem) => {
    setAdding(false)
    if (setExternalAdding) { setExternalAdding(false) }
    onCreateItem(newItem)
    setItemType(null)
    setExternalItemType(null)
  }

  const onCreateBackendConfigExists = !!onCreateBackendConfig

  const fileOptions = useNewFileOptions({
    startNewItem,
    supportsNewFile: !!onCreateItem && !disableNewFiles,
    supportsNewComponentFile: !!onCreateItem && !disableNewFiles,
    supportsNewFolder: !!onCreateItem,
    supportsAssetBundle: false,
  })

  const options = [
    !!onFileSelect && {
      content: t('file_list.option.upload'),
      onClick: onFileSelect,
      a8: 'click;cloud-editor-asset-management;upload-assets-button',
    },
    onCreateBackendConfigExists && {
      content: t('file_list.option.new_backend_config'),
      onClick: onCreateBackendConfig,
      a8: 'click;cloud-editor-asset-management;new-backend-proxy-button',
    },
    ...fileOptions,
    ...additionalOptions,
  ].filter(Boolean)

  return (
    <div className={grow && classes.fileListGrow} {...menuState.getReferenceProps()}>
      {title &&
        <div
          className={classes.fileListBar}
          onContextMenu={menuState.handleContextMenu}
        >
          <h4 className={classes.fileListTitle}>
            {title}
            {subtitle && (
              <span className={classes.fileListSubtitle}>
                {subtitle}
              </span>
            )}
          </h4>
        </div>
      }
      <div className={elementClasses.treeElementGroup} id={productTourId}>
        {showOpenExternally &&
          <div className={classes.openButton}>
            <FloatingPanelButton
              onClick={() => openProject(appKey)}
              spacing='full'
            >
              {t('file_configurator.button.open_externally', {ns: 'cloud-studio-pages'})}
            </FloatingPanelButton>
          </div>
        }
        {isAdding &&
          <NewFileItem
            itemType={externalItemType || itemType}
            onSubmit={handleCreateItem}
            onCancel={cancelNewItem}
          />
        }
        {paths.folderPaths.map(filePath => (
          <FileBrowserListFolder
            key={filePath}
            filePath={filePath}
            currentEditorFileLocation={currentEditorFileLocation}
            onFileClick={onFileClick}
          />
        ))}
        {paths.filePaths.map((filePath) => {
          if (STUDIO_HIDDEN_FILE_PATHS.includes(filePath)) {
            return null
          }

          return (
            <FileBrowserListFile
              key={filePath}
              fileLocation={filePath}
              onFileClick={onFileClick}
            />
          )
        })}
      </div>
      {options.length > 0 &&
        <div
          className={combine(classes.extraSpace, grow && classes.extraSpaceGrow)}
          onContextMenu={menuState.handleContextMenu}
        />
      }
      <ContextMenu
        menuState={menuState}
        options={options}
      />
    </div>
  )
}

export type {
  ItemType,
}

export {
  FileBrowserList,
}
