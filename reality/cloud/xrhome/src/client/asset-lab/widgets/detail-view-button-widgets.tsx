import React from 'react'
import {useTranslation} from 'react-i18next'
import type {Placement} from '@floating-ui/react'

import {Icon, IconStroke} from '../../ui/components/icon'
import {combine} from '../../common/styles'
import {LoadableButtonChildren} from '../../ui/components/loadable-button-children'
import {useAssetLabStateContext} from '../asset-lab-context'
import {useStudioStateContext} from '../../studio/studio-state-context'
import {extractProjectFileLists} from '../../editor/files/extract-file-lists'
import {useFileSelection} from '../../studio/hooks/use-file-selection'
import {useCurrentGit, useScopedGitFile} from '../../git/hooks/use-current-git'
import {createThemedStyles} from '../../ui/theme'
import {AssetLabButton, AssetLabButtonWithCost} from './asset-lab-button'
import {getAssetGenUrl} from '../urls'
import {useSelector} from '../../hooks'
import {useDownloadAssets} from '../hooks/use-download-assets'
import {MenuOptions} from '../../studio/ui/option-menu'
import type {AssetGenerationAssetType} from '../../common/types/db'
import {getMenuOptions} from './asset-lab-asset-context-menu'
import {SelectMenu} from '../../studio/ui/select-menu'
import {useStudioMenuStyles} from '../../studio/ui/studio-menu-styles'
import {useImportIntoProject} from '../hooks/use-import-into-project'
import {useEnclosedApp} from '../../apps/enclosed-app-context'

const useStyles = createThemedStyles(theme => ({
  buttonContainer: {
    'position': 'relative',
    'color': theme.fgMain,
    'background': theme.studioPanelBtnBg,
    'borderRadius': '0.5rem',
    'padding': '0 0.75rem',
    'margin': '0 0.25rem',
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'height': '32px',
    'gap': '8px',
    'cursor': 'pointer',
    '&:disabled': {
      cursor: 'not-allowed',
      opacity: 0.5,
    },
    '&:hover:not(:disabled)': {
      background: theme.studioPanelBtnHoverBg,
    },
  },
  libraryButton: {
    color: theme.fgMuted,
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '8px',
    cursor: 'pointer',
    margin: '0 0.75rem',
    flex: '0 0 auto',
    whiteSpace: 'nowrap',
  },
  buttonPrimary: {
    'background': theme.primaryBtnBg,
    '&:hover': {
      color: theme.primaryBtnHoverBg,
    },
  },
  contextMenu: {
    zIndex: 200,
    backgroundColor: theme.studioBgMain,
  },
}))

interface IDetailViewButton{
  onClick: React.MouseEventHandler<HTMLButtonElement>
  text: string
  iconStroke?: IconStroke
  disabled?: boolean
  loading?: boolean
  primary?: boolean
}

const DetailViewButton: React.FC<IDetailViewButton> = ({
  onClick, text, iconStroke, disabled, loading, primary,
}) => {
  const classes = useStyles()

  return (
    <button
      type='button'
      className={
        combine('style-reset', classes.buttonContainer,
          primary ? classes.buttonPrimary : '')}
      onClick={onClick}
      disabled={disabled || loading}
    >
      <LoadableButtonChildren loading={loading}>
        {iconStroke && <Icon stroke={iconStroke} />}
      </LoadableButtonChildren>
      {text}
    </button>
  )
}

const AssetLabLibraryButton: React.FC<{ onClick: () => void }> = ({onClick}) => {
  const classes = useStyles()
  const {t} = useTranslation('asset-lab')

  return (
    <button
      type='button'
      className={combine('style-reset', classes.libraryButton)}
      onClick={onClick}
    >
      <Icon stroke='chevronLeft' />
      {t('asset_lab.detail_view.back_to_library')}
    </button>
  )
}

const DetailViewSendToMenu: React.FC<{
  assetGenUuid: string
  placement?: Placement
}> = ({assetGenUuid, placement = 'bottom-end'}) => {
  const {t} = useTranslation('asset-lab')
  const menuStyles = useStudioMenuStyles()
  const classes = useStyles()
  const assetLabCtx = useAssetLabStateContext()
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenUuid])
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration?.RequestUuid])
  const menuOptions = getMenuOptions(t, assetLabCtx, assetGeneration, assetRequest, true)

  return (
    <SelectMenu
      id='asset-lab-send-to-dropdown'
      menuWrapperClassName={combine(menuStyles.studioMenu, classes.contextMenu)}
      trigger={(
        <div className={classes.buttonContainer}>
          {t('asset_lab.detail_view.send_to')} <Icon stroke='chevronDown' />
        </div>
      )}
      placement={placement}
      margin={5}
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
interface IGoToAssetButton {
  disabled?: boolean
  fullAssetGenFilePath: string
}

const GoToAssetButton: React.FC<IGoToAssetButton> = ({disabled, fullAssetGenFilePath}) => {
  const assetLabCtx = useAssetLabStateContext()
  const stateCtx = useStudioStateContext()
  const git = useCurrentGit()
  const files = extractProjectFileLists(git, () => false)
  const {onFileClick} = useFileSelection(files.assets)
  const {t} = useTranslation('asset-lab')

  return (
    <DetailViewButton
      disabled={disabled}
      onClick={async (e: React.MouseEvent<HTMLButtonElement>) => {
        onFileClick(fullAssetGenFilePath, e)
        assetLabCtx.setState({open: false, assetRequestUuid: undefined})
        stateCtx.update(p => ({
          ...p, selectedAsset: fullAssetGenFilePath, currentBrowserSection: 'files',
        }))
        stateCtx.state.fileBrowserScroller.scrollTo(fullAssetGenFilePath)
      }}
      text={t('asset_lab.generate.go_to_asset')}
    />
  )
}

const AssetLabDownloadButton: React.FC<{
  assetGenUuid: string
}> = ({assetGenUuid}) => {
  const {t} = useTranslation('asset-lab')
  const {downloadFromUrl, isDownloading} = useDownloadAssets()
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenUuid])
  const assetGenFilePath = getAssetGenUrl(assetGeneration)

  return (
    <AssetLabButton
      onClick={async () => {
        await downloadFromUrl(assetGenFilePath, assetGeneration.uuid)
      }}
      loading={isDownloading}
    >
      <Icon inline stroke='download' />&nbsp;{t('asset_lab.detail_view.download_asset')}
    </AssetLabButton>
  )
}

const AssetLabImportButton: React.FC<{
  assetGenUuid: string
  type: AssetGenerationAssetType
}> = ({assetGenUuid, type}) => {
  const {t} = useTranslation('asset-lab')
  const [isImporting, setIsImporting] = React.useState(false)

  const isImage = type === 'IMAGE'
  const assetExtension = isImage ? '.png' : '.glb'
  const assetFileType = isImage ? 'image/png' : 'model/gltf-binary'

  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenUuid])
  const app = useEnclosedApp()
  const fullAssetGenFilePath = `assets/${assetGenUuid}${assetExtension}`
  const alreadyImported = !!useScopedGitFile(app?.repoId, fullAssetGenFilePath)

  const {importFromUrl, hasApp} = useImportIntoProject()

  return (
    alreadyImported
      ? <GoToAssetButton disabled={isImporting} fullAssetGenFilePath={fullAssetGenFilePath} />
      : hasApp && (
        <DetailViewButton
          onClick={async () => {
            setIsImporting(true)
            await importFromUrl(
              getAssetGenUrl(assetGeneration),
              `${assetGeneration.uuid}${assetExtension}`,
              assetFileType
            )
            setIsImporting(false)
          }}
          text={t('asset_lab.generate.import_into_project')}
          iconStroke='downloadFile'
          disabled={isImporting || alreadyImported}
          loading={isImporting}
        />
      )
  )
}
const AssetLabRerollButton: React.FC<{
  generateReroll: (inputIndex: number) => Promise<void>
  creditStatus: 'success' | 'error' | 'pending'
  isGenerating: boolean
  isSubmitting: boolean
  inputIndex: number
}> = ({generateReroll, creditStatus, isGenerating, isSubmitting, inputIndex}) => {
  const {t} = useTranslation('asset-lab')
  return (
    <AssetLabButtonWithCost
      a8='click;studio;reroll-multiview-button'
      onClick={() => generateReroll(inputIndex)}
      disabled={creditStatus !== 'success' || isGenerating || isSubmitting}
      label={t('asset_lab.image_gen.reroll')}
      type='TO_IMAGE'
      modelId='gpt-image-1'
    />
  )
}

export {
  DetailViewButton,
  AssetLabLibraryButton,
  GoToAssetButton,
  DetailViewSendToMenu,
  AssetLabDownloadButton,
  AssetLabImportButton,
  AssetLabRerollButton,
}
