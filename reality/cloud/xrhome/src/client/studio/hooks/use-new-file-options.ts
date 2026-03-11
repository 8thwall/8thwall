import {useTranslation} from 'react-i18next'

type ItemType = 'file' | 'component' | 'folder'

type useNewFileOptionsArgs = {
  startNewItem: (itemType: ItemType) => void
  supportsNewFile: boolean
  supportsNewComponentFile: boolean
  supportsNewFolder: boolean
  // NOTE(christoph): supportsAssetBundle is being used as a proxy for being inside the assets
  // directory, but is only true for the top-level asset list as opposed to subfolders.
  // It only affects metrics.
  supportsAssetBundle?: boolean
}

const useNewFileOptions = ({
  startNewItem, supportsNewFile, supportsNewComponentFile, supportsNewFolder, supportsAssetBundle,
}: useNewFileOptionsArgs) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const res = []

  if (supportsNewFile && supportsNewComponentFile) {
    res.push({
      type: 'menu',
      content: t('file_list.option.new_file'),
      options: [
        {
          content: t('file_list.option.empty_file'),
          onClick: () => startNewItem('file'),
          a8: supportsAssetBundle
            ? 'click;cloud-editor-asset-management;new-asset-file-button'
            : undefined,
        },
        {
          content: t('file_list.option.new_component_file'),
          onClick: () => startNewItem('component'),
          a8: supportsAssetBundle
            ? 'click;cloud-editor-asset-management;new-asset-file-button'
            : undefined,
        },
      ],
    })
  } else {
    if (supportsNewFile) {
      res.push({
        content: t('file_list.option.new_file'),
        onClick: () => startNewItem('file'),
        a8: supportsAssetBundle
          ? 'click;cloud-editor-asset-management;new-asset-file-button'
          : undefined,
      })
    }

    if (supportsNewComponentFile) {
      res.push({
        content: t('file_list.option.new_component_file'),
        onClick: () => startNewItem('component'),
        a8: supportsAssetBundle
          ? 'click;cloud-editor-asset-management;new-asset-file-button'
          : undefined,
      })
    }
  }

  if (supportsNewFolder) {
    res.push({
      content: t('file_list.option.new_folder'),
      onClick: () => startNewItem('folder'),
      a8: supportsAssetBundle
        ? 'click;cloud-editor-asset-management;new-asset-folder-button'
        : undefined,
    })
  }

  return res
}

export {useNewFileOptions}

export type {useNewFileOptionsArgs, ItemType}
