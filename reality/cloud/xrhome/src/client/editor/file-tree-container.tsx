import React from 'react'

import {DropTarget} from './drop-target'
import {combine} from '../common/styles'
import {FileActionsContext} from './files/file-actions-context'

interface IFileTreeContainer {
  themeName: string
  classNameOverride?: string
  children?: React.ReactNode
  hoverClassName?: string
}

const FileTreeContainer: React.FC<IFileTreeContainer> = (
  {themeName, classNameOverride, children, hoverClassName}
) => {
  const [leftPaneActive, setLeftPaneActive] = React.useState(false)
  const {onDrop} = React.useContext(FileActionsContext)
  return (
    <DropTarget
      onDrop={(e) => {
        e.persist()
        setLeftPaneActive(false)
        onDrop(e)
      }}
      onHoverStart={() => setLeftPaneActive(true)}
      onHoverStop={() => setLeftPaneActive(false)}
      className={combine(
        classNameOverride,
        themeName,
        leftPaneActive ? hoverClassName || 'active' : '',
        classNameOverride || 'file-browser vertical'
      )}
    >
      {children}
    </DropTarget>
  )
}

export {
  FileTreeContainer,
}
