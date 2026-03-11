import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingPanelIconButton} from '../ui/components/floating-panel-icon-button'
import {useNewFileOptions} from './hooks/use-new-file-options'
import {SelectMenu} from './ui/select-menu'
import {MenuOptions} from './ui/option-menu'
import {useStudioMenuStyles} from './ui/studio-menu-styles'

type ItemType = 'file' | 'component' | 'folder'

interface IFileBrowserCornerButton {
  onFileSelect?: Function
  onCreateItem?: (newItem: {name: string, isFile: boolean}) => void
  disableNewFiles?: boolean
  setExternalAdding: (adding: boolean, isAssetPath?: boolean) => void
  setExternalItemType: (itemType: ItemType) => void
}

const FileBrowserCornerButton: React.FC<IFileBrowserCornerButton> = ({
  onFileSelect, disableNewFiles, onCreateItem,
  setExternalAdding, setExternalItemType,
}) => {
  const {t} = useTranslation(['cloud-editor-pages', 'cloud-studio-pages'])

  const menuStyles = useStudioMenuStyles()

  const startNewItem = (newItem: ItemType, isAssetPath?: boolean) => {
    setExternalItemType(newItem)
    setExternalAdding(true, isAssetPath)
  }

  const fileOptions = useNewFileOptions({
    startNewItem,
    supportsNewFile: !!onCreateItem && !disableNewFiles,
    supportsNewComponentFile: !!onCreateItem && !disableNewFiles,
    supportsNewFolder: false,
    supportsAssetBundle: false,
  })

  const options = [
    {
      content: t('file_list.option.upload'),
      onClick: onFileSelect,
      a8: 'click;cloud-editor-asset-management;upload-assets-button',
    },
    ...fileOptions,
    {
      type: 'menu',
      content: t('file_list.option.new_folder'),
      options: [
        {
          content: t('file_list.option.new_file_folder'),
          onClick: () => startNewItem('folder'),
        },
        {
          content: t('file_list.option.new_asset_folder'),
          onClick: () => startNewItem('folder', true),
        },
      ],
    },
  ]

  return (
    <SelectMenu
      id='file-browser-corner-dropdown'
      trigger={(
        <FloatingPanelIconButton
          a8='click;studio;file-browser-corner-menu-button'
          text={t('file_browser_corner_dropdown.label', {ns: 'cloud-studio-pages'})}
          stroke='plus'
        />
      )}
      menuWrapperClassName={menuStyles.studioMenu}
      placement='right-start'
      margin={16}
      minTriggerWidth
      returnFocus={false}
    >
      {collapse => (
        <MenuOptions
          collapse={collapse}
          options={options}
          returnFocus={false}
        />
      )}
    </SelectMenu>
  )
}

export {
  FileBrowserCornerButton,
}
