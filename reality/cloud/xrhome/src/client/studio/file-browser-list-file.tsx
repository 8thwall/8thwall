import React from 'react'
import {useTranslation} from 'react-i18next'

import type {IFileListFile} from '../editor/files/file-list-file'
import {
  isAssetPath,
} from '../common/editor-files'
import {FileActionsContext} from '../editor/files/file-actions-context'
import {basename} from '../editor/editor-common'
import FileListIcon from '../editor/file-list-icon'
import InlineTextInput from '../common/inline-text-input'
import {combine} from '../common/styles'
import {useTreeElementStyles, IndentFileBrowserCssProperties} from './ui/tree-element-styles'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {extractFilePath, type EditorFileLocation} from '../editor/editor-file-location'
import {useCurrentRepoId} from '../git/repo-id-context'
import {useStudioStateContext} from './studio-state-context'
import {useResourceUrl} from './hooks/resource-url'
import {useShowLocalFile} from './hooks/show-local-file'
import {openFile} from './local-sync-api'
import useCurrentApp from '../common/use-current-app'

interface IFileBrowserListFile{
  fileLocation: EditorFileLocation
  level?: number
  onFileClick?: (path: string, e: React.MouseEvent) => void
}

const FileBrowserListFile: React.FC<IFileBrowserListFile> = (
  {fileLocation, level = 0, onFileClick}
) => {
  const elementClasses = useTreeElementStyles()
  const [renaming, setRenaming] = React.useState(false)
  const [newName, setNewName] = React.useState('')
  const extractedFilePath = extractFilePath(fileLocation)
  const repoId = useCurrentRepoId()
  const stateCtx = useStudioStateContext()
  const {selectedFiles, fileBrowserScroller} = stateCtx.state
  const selected = selectedFiles?.includes(extractedFilePath) ?? false
  const app = useCurrentApp()

  const {t} = useTranslation(['cloud-editor-pages'])

  const {
    onSelect, onDelete, onRename, isProtectedFile,
  } = React.useContext(FileActionsContext)

  const fileBasename = basename(extractedFilePath)

  const name = fileBasename

  const resourceUrl = useResourceUrl(extractedFilePath)
  const allowDeleteRename = !isProtectedFile(extractedFilePath)

  const menuState = useContextMenuState()

  const startRename = () => {
    setNewName(name)
    setRenaming(true)
  }

  const cancelRename = () => {
    setRenaming(false)
  }

  const handleRenameSubmit = () => {
    setRenaming(false)
    onRename(newName, fileLocation)
  }

  const handleClick = (e: React.MouseEvent) => {
    if (renaming) {
      return
    }
    e.stopPropagation()
    if (!e.shiftKey) {
      onSelect(fileLocation)
    }
    onFileClick?.(extractedFilePath, e)
  }

  const handleDragStart = (e: React.DragEvent) => {
    if (selected) {
      e.dataTransfer.setData('filePaths', JSON.stringify(selectedFiles))
      e.dataTransfer.setData('filePath', extractedFilePath)
    } else {
      e.dataTransfer.setData('filePaths', JSON.stringify([extractedFilePath]))
      e.dataTransfer.setData('filePath', extractedFilePath)
    }
    e.dataTransfer.setData('repoId', repoId)
    e.dataTransfer.setData('resourceUrl', resourceUrl)
  }

  const handleRenameFocus = (e: {target: HTMLInputElement}) => {
    // automatically select the file name without the extension when renaming
    const firstDotIndex = newName.indexOf('.')
    if (firstDotIndex === -1) return  // no need to select since it'll be the same
    e.target.setSelectionRange(0, firstDotIndex)
  }

  const options = [
    allowDeleteRename && {
      content: t('file_list.option.rename'),
      onClick: startRename,
    },
    allowDeleteRename && {
      content: t('file_list.option.delete'),
      onClick: () => onDelete(fileLocation),
    },
    useShowLocalFile(fileLocation),
  ].filter(Boolean)

  return (
    <div className={combine(elementClasses.treeElementButton,
      selected && elementClasses.selectedButton,
      menuState.contextMenuOpen && elementClasses.activeButton)}
    >
      <button
        draggable={!renaming && allowDeleteRename}
        type='button'
        className={combine('style-reset', elementClasses.treeElementNameRow,
          elementClasses.treeElementBtnText)}
        onClick={handleClick}
        onDoubleClick={() => {
          if (Build8.PLATFORM_TARGET === 'desktop' && !isAssetPath(extractedFilePath)) {
            openFile(app.appKey, extractedFilePath)
          }
        }}
        onDragStart={handleDragStart}
        onContextMenu={menuState.handleContextMenu}
        ref={el => fileBrowserScroller.setRef(extractedFilePath, el)}
        {...menuState.getReferenceProps()}
      >
        <div
          style={{'--file-level': level} as IndentFileBrowserCssProperties}
          className={elementClasses.fileName}
        >
          <FileListIcon
            filename={renaming ? newName : fileBasename}
          />
          {renaming
            ? (
              <InlineTextInput
                value={newName}
                onChange={e => setNewName(e.target.value)}
                onCancel={cancelRename}
                onSubmit={handleRenameSubmit}
                onFocus={handleRenameFocus}
                inputClassName={combine('style-reset', elementClasses.renaming)}
              />
            )
            : <div className={elementClasses.name} title={name}>{name}</div>
        }
        </div>
      </button>
      <ContextMenu
        menuState={menuState}
        options={options}
      />
    </div>
  )
}

export {FileBrowserListFile}

export type {IFileListFile}
