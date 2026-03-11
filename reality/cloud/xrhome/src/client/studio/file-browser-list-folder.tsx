import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import icons from '../apps/icons'
import {DropTarget} from './ui/drop-target'
import {useFolderChildren} from '../git/hooks/use-folder-children'
import {useLayoutChangeEffect} from '../hooks/use-change-effect'
import {basename} from '../editor/editor-common'
import {FileActionsContext, NewItem} from '../editor/files/file-actions-context'
import {isAssetPath} from '../common/editor-files'
import FileListIcon from '../editor/file-list-icon'
import {NewFileItem} from './file-browser-new-file-item'
import {FileBrowserListFile} from './file-browser-list-file'
import InlineTextInput from '../common/inline-text-input'
import {useNewFileOptions} from './hooks/use-new-file-options'
import {useCurrentRepoId} from '../git/repo-id-context'
import type {ItemType} from './file-browser-list'
import {combine} from '../common/styles'
import {useTreeElementStyles, IndentFileBrowserCssProperties} from './ui/tree-element-styles'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {editorFileLocationEqual, type ScopedFileLocation} from '../editor/editor-file-location'
import {useStudioStateContext} from './studio-state-context'
import {useShowLocalFile} from './hooks/show-local-file'

const useStyles = createUseStyles({
  folderContainer: {
    'display': 'flex',
    'flexDirection': 'column',
  },
})

interface IFileBrowserListFolder {
  filePath: string
  currentEditorFileLocation: ScopedFileLocation
  level?: number
  onFileClick?: (path: string, e: React.MouseEvent) => void
}

const FileBrowserListFolder: React.FC<IFileBrowserListFolder> = (
  {filePath, currentEditorFileLocation, level = 0, onFileClick}
) => {
  const [expanded, setExpanded] = React.useState(
    editorFileLocationEqual(currentEditorFileLocation, filePath)
  )
  const [adding, setAdding] = React.useState(false)
  const [itemType, setItemType] = React.useState<ItemType>(null)
  const [renaming, setRenaming] = React.useState(false)
  const [dragging, setDragging] = React.useState(false)
  const [newName, setNewName] = React.useState('')

  const classes = useStyles()
  const elementClasses = useTreeElementStyles()

  const name = basename(filePath)
  const {selectedFiles} = useStudioStateContext().state
  const selected = (selectedFiles?.includes(filePath)) ?? false

  const repoId = useCurrentRepoId()
  const {onDelete, onRename, onDrop, onCreate} = React.useContext(FileActionsContext)

  const {folderPaths, filePaths} = useFolderChildren(filePath)
  const isEmpty = !folderPaths.length && !filePaths.length

  const {t} = useTranslation(['cloud-editor-pages'])

  const menuState = useContextMenuState()

  useLayoutChangeEffect(([, previousEditorFileLocation]) => {
    const shouldExpand = !expanded &&
                         currentEditorFileLocation !== previousEditorFileLocation &&
                         editorFileLocationEqual(currentEditorFileLocation, filePath)
    if (shouldExpand) {
      setExpanded(true)
    }
  }, [expanded, currentEditorFileLocation, filePath] as const)

  const disableNewFiles = isAssetPath(filePath)

  const toggleExpanded = () => {
    setExpanded(!expanded)
  }

  const handleHoverStart = () => {
    setDragging(true)
  }

  const handleHoverEnd = () => {
    setDragging(false)
  }

  const handleDrop = (e: React.DragEvent) => {
    e.persist()
    setDragging(false)
    if (isEmpty) {
      setExpanded(true)
    }
    onDrop(e, filePath)
  }

  const handleDragStart = (e: React.DragEvent) => {
    if (selected) {
      e.dataTransfer.setData('filePaths', JSON.stringify(selectedFiles))
    } else {
      e.dataTransfer.setData('filePath', filePath)
    }
    e.dataTransfer.setData('repoId', repoId)
  }

  const handleClick = (e: React.MouseEvent) => {
    e.stopPropagation()
    if (!e.shiftKey) {
      toggleExpanded()
    }
    onFileClick?.(filePath, e)
  }

  const startNewItem = (newItemType: ItemType) => {
    setItemType(newItemType)
    setAdding(true)
  }

  const cancelNewItem = () => {
    setAdding(false)
  }

  const handleCreateItem = (newItem: NewItem) => {
    setAdding(false)
    setExpanded(true)
    onCreate(newItem, filePath)
  }

  const startRename = () => {
    setNewName(name)
    setRenaming(true)
  }

  const cancelRename = () => {
    setRenaming(false)
  }

  const handleRenameSubmit = () => {
    setRenaming(false)
    onRename(newName, filePath)
  }

  let folderIcon
  if (isEmpty) {
    folderIcon = icons.folder_empty
  } else if (expanded) {
    folderIcon = icons.folder_expanded
  } else {
    folderIcon = icons.folder_collapsed
  }

  const options = [
    ...useNewFileOptions({
      startNewItem,
      supportsNewFile: !disableNewFiles,
      supportsNewFolder: true,
      supportsNewComponentFile: false,
    }),
    {content: t('file_list.option.rename'), onClick: startRename},
    {content: t('file_list.option.delete'), onClick: () => onDelete(filePath)},
    useShowLocalFile(filePath),
  ]

  return (
    <DropTarget
      as='div'
      className={combine(classes.folderContainer, dragging && elementClasses.activeButton)}
      onDrop={handleDrop}
      onHoverStart={handleHoverStart}
      onHoverStop={handleHoverEnd}
    >
      <div className={combine(elementClasses.treeElementButton,
        selected && elementClasses.selectedButton)}
      >
        <button
          type='button'
          className={combine('style-reset', elementClasses.treeElementBtnText)}
          onClick={handleClick}
          draggable={!renaming}
          onDragStart={handleDragStart}
          onContextMenu={menuState.handleContextMenu}
          {...menuState.getReferenceProps()}
        >
          <div
            style={{'--file-level': level} as IndentFileBrowserCssProperties}
            className={elementClasses.fileName}
          >
            <FileListIcon icon={folderIcon} />
            {renaming
              ? (
                <InlineTextInput
                  value={newName}
                  onChange={e => setNewName(e.target.value)}
                  onCancel={cancelRename}
                  onSubmit={handleRenameSubmit}
              // eslint-disable-next-line local-rules/hardcoded-copy
                  inputClassName={combine('style-reset', elementClasses.renaming)}
                />
              )
              : <span title={name}>{name}</span>
          }
          </div>
        </button>
      </div>

      {(expanded || adding) &&
        <>
          {adding &&
            <NewFileItem
              itemType={itemType}
              onSubmit={handleCreateItem}
              onCancel={cancelNewItem}
              level={level + 1}
            />
          }
          {folderPaths.map(subfolder => (
            <FileBrowserListFolder
              key={subfolder}
              filePath={subfolder}
              currentEditorFileLocation={currentEditorFileLocation}
              level={level + 1}
              onFileClick={onFileClick}
            />
          ))}
          {filePaths.map(file => (
            // eslint-disable-next-line @typescript-eslint/no-use-before-define
            <FileBrowserListFile
              key={file}
              fileLocation={file}
              level={level + 1}
              onFileClick={onFileClick}
            />
          ))}
        </>
      }
      <ContextMenu
        menuState={menuState}
        options={options}
      />
    </DropTarget>
  )
}

export {FileBrowserListFolder}

export type {IFileBrowserListFolder}
