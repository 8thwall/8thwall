import React from 'react'
import {useTranslation} from 'react-i18next'
import {BASIC} from '@ecs/shared/environment-maps'

import {backToLibraryClear, useAssetLabStateContext} from './asset-lab-context'
import {useSelector} from '../hooks'
import {getAssetGenUrl} from './urls'
import {DetailViewLeftSnackbar} from './widgets/asset-lab-detail-view-snackbar'
import StudioModelPreviewWithLoader from '../studio/studio-model-preview-with-loader'
import {createThemedStyles} from '../ui/theme'
import {urlToUuid} from './generate-request'
import {SquareImage} from './widgets/square-asset'
import {
  AssetLabDownloadButton, DetailViewButton, AssetLabImportButton, DetailViewSendToMenu,
  AssetLabLibraryButton,
} from './widgets/detail-view-button-widgets'

const useStyles = createThemedStyles(theme => ({
  imagePromptContainer: {
    'minWidth': '150px',
    'maxWidth': '15%',
    'maxHeight': '80vh',
    'overflowY': 'auto',
    '&::-webkit-scrollbar': {
      width: '3px',
      background: 'transparent',
    },
    'paddingRight': '5px',
    'display': 'flex',
    'flexDirection': 'column',
    'gap': '0.62rem',
  },
  imageContainer: {
    'borderRadius': '0.5rem',
    'backgroundColor': theme.sfcBackgroundDefault,
    'display': 'flex',
    'overflow': 'hidden',
    'justifyContent': 'center',
    'width': '13rem',
    'height': '13rem',
  },
  assetInfoContainer: {
    display: 'flex',
    flexDirection: 'row',
    gap: '.5rem',
    margin: '.5rem 0',
    overflow: 'hidden',
    minWidth: 0,
    height: '100%',
  },
  modelContainer: {
    flex: '1 0',
    borderRadius: '0.5rem',
    backgroundColor: theme.sfcBackgroundDefault,
    display: 'flex',
    overflow: 'hidden',
    justifyContent: 'center',
    height: '100%',
    width: '100%',
    position: 'relative',
  },
  row: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
  },
}))

const AssetLabDetailViewMesh = () => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()

  const assetLabCtx = useAssetLabStateContext()
  const {assetGenerationUuid} = assetLabCtx.state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration.RequestUuid])
  // @ts-expect-error TODO(christoph): Clean up
  const genUser = useSelector(s => s.team.roles.find(e => e.uuid === assetGeneration.UserUuid))
  // NOTE(dat): An asset might have been optimized so it might be available at a slightly different
  // URL.
  const assetGenFilePath = getAssetGenUrl(assetGeneration)

  const promptUrls = assetRequest.input?.imageUrls

  return (
    <>
      <div className={classes.row}>
        <AssetLabLibraryButton onClick={() => {
          backToLibraryClear(assetLabCtx, {scrollToUuid: assetGenerationUuid})
        }}
        />
      </div>
      <div className={classes.assetInfoContainer}>
        <div className={classes.imagePromptContainer}>
          {((promptUrls && Array.isArray(promptUrls))
            ? promptUrls
            : [])
            .map((imagePrompt: string, index: number) => (
              <SquareImage
                key={imagePrompt || index}
                srcId={urlToUuid(imagePrompt)}
                size={240}
              />
            ))
          }
        </div>
        <div className={classes.modelContainer}>
          <StudioModelPreviewWithLoader
            src={assetGenFilePath}
            srcExt='glb'
            envName={BASIC}
          />
        </div>
      </div>
      <div className={classes.row}>
        <DetailViewLeftSnackbar
          user={genUser ? {givenName: genUser.given_name, familyName: genUser.family_name} : null}
          assetGeneration={assetGeneration}
        />
        <div className={classes.row}>
          <DetailViewSendToMenu assetGenUuid={assetGenerationUuid} />
          <DetailViewButton
            onClick={() => {
              assetLabCtx.setState({
                mode: 'optimizer',
                prevMode: assetLabCtx.state.mode,
              })
            }}
            iconStroke='wrench'
            text={t('asset_lab.open_in_3d_model_optimizer')}
          />
          <AssetLabDownloadButton assetGenUuid={assetGenerationUuid} />
          <AssetLabImportButton
            assetGenUuid={assetGenerationUuid}
            type={assetGeneration.assetType}
          />
        </div>
      </div>
    </>
  )
}

export {
  AssetLabDetailViewMesh,
}
