import React from 'react'

import {basename} from '../../editor/editor-common'
import {EditorFileLocation, extractFilePath} from '../../editor/editor-file-location'
import {FileActionsContext} from '../../editor/files/file-actions-context'
import {AssetNameConfigurator} from './asset-name-configurator'
import {useEphemeralEditStateWithSubmit} from './ephemeral-edit-state'
import FileListIcon from '../../editor/file-list-icon'

interface IFileNameConfigurator {
  location: EditorFileLocation
}

const FileNameConfigurator: React.FC<IFileNameConfigurator> = ({
  location,
}) => {
  const {onRename} = React.useContext(FileActionsContext)

  const {
    editValue: assetEditName,
    setEditValue: setAssetEditName,
    clear: clearAssetEditName,
    submit: submitAssetEditName,
  } = useEphemeralEditStateWithSubmit({
    value: basename(extractFilePath(location)),
    deriveEditValue: (v: string) => v,
    parseEditValue: (v: string) => {
      if (!v) {
        return [false]
      }

      return [true, v]
    },
    onSubmit: (v: string) => {
      onRename(v, location)
    },
  })

  return (
    <AssetNameConfigurator
      assetEditName={assetEditName}
      setAssetEditName={setAssetEditName}
      clearAssetEditName={clearAssetEditName}
      submitAssetEditName={submitAssetEditName}
      icon={<FileListIcon filename={assetEditName} />}
    />
  )
}

export {
  FileNameConfigurator,
}
