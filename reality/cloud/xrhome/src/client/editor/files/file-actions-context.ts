import React from 'react'

import type {EditorFileLocation} from '../editor-file-location'
import type {SwitchTab} from '../hooks/use-tab-actions'

type NewItem = {
  name: string
  isFile: boolean
  fileContent?: string
}

interface IFileActionsContext {
  onCreate: (newItem: NewItem, withinPath?: EditorFileLocation) => void
  onDelete: (itemPath: EditorFileLocation) => void
  onDrop: (e: React.DragEvent, withinPath?: EditorFileLocation) => void
  onRename: (newPath: string, prevLocation: EditorFileLocation) => void
  onSelect: SwitchTab
  onUploadStart: (withinPath?: EditorFileLocation) => void

  // A protected file can't be deleted or renamed
  isProtectedFile: (filePath: string) => boolean

  onClose?: (tabId: number) => void
}

const FileActionsContext = React.createContext<IFileActionsContext>(null)

const useFileActionsContext = () => React.useContext(FileActionsContext)

export {
  FileActionsContext,
  useFileActionsContext,
}

export type {
  NewItem,
  IFileActionsContext,
}
