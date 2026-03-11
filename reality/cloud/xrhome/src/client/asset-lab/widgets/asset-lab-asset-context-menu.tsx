import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'

import {MenuOptions} from '../../studio/ui/option-menu'
import {createThemedStyles} from '../../ui/theme'
import {getAssetGenUrl} from '../urls'
import {SelectMenu} from '../../studio/ui/select-menu'
import {useStudioMenuStyles} from '../../studio/ui/studio-menu-styles'
import {combine} from '../../common/styles'
import {IAssetLabStateContext, useAssetLabStateContext} from '../asset-lab-context'
import {useSelector} from '../../hooks'
import {useImportIntoProject} from '../hooks/use-import-into-project'
import {Icon} from '../../ui/components/icon'
import type {AssetGeneration, AssetRequest} from '../../common/types/db'

const useStyles = createThemedStyles(theme => ({
  button: {
    width: '25px',
    height: '25px',
    backgroundColor: theme.tertiaryBtnFg,
    borderRadius: '4px',
    border: `1px solid ${theme.secondaryBtnHoverBg}`,
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
  },
  contextMenu: {
    zIndex: 200,
  },
}))

const getImageMenuOptions = (
  t: (key: string) => string,
  assetLabCtx: IAssetLabStateContext,
  assetGeneration: DeepReadonly<AssetGeneration>,
  assetRequest: DeepReadonly<AssetRequest>,
  removeSendToPrefix?: boolean
) => [
  {
    content: removeSendToPrefix
      ? t('asset_lab.image')
      : t('asset_lab.image_gen.send_to_image'),
    onClick: async () => {
      assetLabCtx.setState({
        open: true,
        workflow: 'gen-image',
        mode: 'image',
        imageInputUuid: assetGeneration.uuid,
        assetRequestUuid: null,
        assetGenerationUuid: null,
      })
    },
  },
  {
    content: removeSendToPrefix
      ? t('asset_lab.3d_model')
      : t('asset_lab.image_gen.send_to_3d_model'),
    onClick: async () => {
      assetLabCtx.setState({
        open: true,
        workflow: 'gen-3d-model',
        mode: 'model',
        inputFblr: assetGeneration.uuid,
        assetRequestUuid: null,
        assetGenerationUuid: null,
      })
      assetLabCtx.setRequestLineageFromInput(assetRequest.uuid)
    },
  },
]

const getModelMenuOptions = (
  t: (key: string) => string,
  assetLabCtx: IAssetLabStateContext,
  assetRequest: DeepReadonly<AssetRequest>,
  removeSendToPrefix?: boolean
) => [
  {
    content: removeSendToPrefix
      ? t('asset_lab.animation')
      : t('asset_lab.model_gen.send_to_animation'),
    onClick: async () => {
      assetLabCtx.setState({
        open: true,
        workflow: 'gen-animated-char',
        mode: 'animation',
        assetRequestUuid: null,
        assetGenerationUuid: null,
      })
      assetLabCtx.setRequestLineageFromInput(assetRequest.uuid)
    },
  },
]

const getMenuOptions = (
  t: (key: string) => string,
  assetLabCtx: IAssetLabStateContext,
  assetGeneration: DeepReadonly<AssetGeneration>,
  assetRequest: DeepReadonly<AssetRequest>,
  removeSendToPrefix?: boolean
) => {
  const requestType = assetRequest?.type
  switch (requestType) {
    case 'TEXT_TO_IMAGE':
    case 'IMAGE_TO_IMAGE':
      return getImageMenuOptions(t, assetLabCtx, assetGeneration, assetRequest, removeSendToPrefix)
    case 'IMAGE_TO_MESH':
      return getModelMenuOptions(t, assetLabCtx, assetRequest, removeSendToPrefix)
    case 'MESH_TO_ANIMATION':
    default:
      return []
  }
}

const useAssetContextMenuOptions = (id: string) => {
  const {t} = useTranslation('asset-lab')
  const {importFromUrl} = useImportIntoProject()
  const assetLabCtx = useAssetLabStateContext()
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[id])
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration?.RequestUuid])
  const assetGenFilePath = getAssetGenUrl(assetGeneration)

  const isImage = assetGeneration.assetType === 'IMAGE'

  const filetype = isImage ? 'image/png' : 'model/gltf'
  const fileExt = isImage ? '.png' : '.glb'

  const menuOptions = [
    {
      content: t('asset_lab.ctx_menu.import_into_project'),
      onClick: async () => {
        await importFromUrl(assetGenFilePath, `${assetGeneration.uuid}${fileExt}`, filetype)
      },
    },
    {
      content: t('asset_lab.ctx_menu.download'),
      onClick: () => {
        const a = document.createElement('a')
        a.href = assetGenFilePath
        a.download = assetGenFilePath
        a.target = '_blank'
        a.rel = 'noopener noreferrer'
        document.body.appendChild(a)
        a.click()
        document.body.removeChild(a)
      },
    },
    ...getMenuOptions(t, assetLabCtx, assetGeneration, assetRequest),
  ]

  return menuOptions
}

interface IAssetContextMenu {
  id: string
  onOpenChange?: (open: boolean) => void
}

const AssetContextMenu: React.FC<IAssetContextMenu> = ({id, onOpenChange}) => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const menuOptions = useAssetContextMenuOptions(id)

  return (
    <SelectMenu
      id='asset-lab-filter-dropdown'
      trigger={(
        <div className={classes.button}>
          <Icon stroke='kebab' />
        </div>
      )}
      menuWrapperClassName={combine(menuStyles.studioMenu, classes.contextMenu)}
      placement='right-start'
      onOpenChange={onOpenChange}
    >
      {collapse => (
        <MenuOptions
          options={menuOptions}
          collapse={collapse}
        />
      )}
    </SelectMenu>
  )
}

export {
  AssetContextMenu,
  useAssetContextMenuOptions,
  getMenuOptions,
}
