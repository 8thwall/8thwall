import React from 'react'
import {useTranslation} from 'react-i18next'

import {backToLibraryClear, useAssetLabStateContext} from './asset-lab-context'
import {useSelector} from '../hooks'
import {AssetLabDetailViewImage} from './asset-lab-detail-view-image'
import {AssetLabDetailViewMesh} from './asset-lab-detail-view-mesh'
import {IconButton} from '../ui/components/icon-button'
import {AssetLabDetailViewAnimation} from './asset-lab-detail-view-animation'
import {createThemedStyles} from '../ui/theme'
import {AssetLabLibraryButton} from './widgets/detail-view-button-widgets'

const useStyles = createThemedStyles(theme => ({
  row: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
  },
  detailViewContainer: {
    'display': 'flex',
    'flexDirection': 'column',
    'flex': '1 1 auto',
    'height': 'calc(92vh - 60px)',
    'overflow': 'hidden',
  },
  button: {
    'color': theme.fgMuted,
    '& > *:hover': {
      color: theme.fgMain,
    },
    '& > *:disabled': {
      color: theme.fgMuted,
      cursor: 'default',
    },
  },
}))

const AssetLabDetailView = () => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  const assetLabCtx = useAssetLabStateContext()
  const {librarySearchResults} = assetLabCtx
  const {assetGenerationUuid} = assetLabCtx.state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])

  const [assetGenerationIndex, setAssetGenerationIndex] =
    React.useState<number | undefined>(-1)

  React.useEffect(() => {
    setAssetGenerationIndex(librarySearchResults.findIndex(e => e === assetGenerationUuid))
  }, [assetGenerationUuid, librarySearchResults])

  const prevAssetGenerationUuid = librarySearchResults[assetGenerationIndex - 1]
  const nextAssetGenerationUuid = librarySearchResults[assetGenerationIndex + 1]

  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration?.RequestUuid])
  const requestType = assetRequest?.type || assetGeneration?.metadata?.type

  if (!assetGeneration) {
    return (
      <div>
        <AssetLabLibraryButton onClick={() => {
          backToLibraryClear(assetLabCtx)
        }}
        />
        <p>{t('asset_lab.detail_view.no_asset_found')}</p>
      </div>
    )
  }
  return (
    <div className={classes.row}>
      <div className={classes.button}>
        <IconButton
          stroke='chevronLeft'
          text={t('asset_lab.detail_view.prev_asset')}
          onClick={() => {
            if (prevAssetGenerationUuid) {
              assetLabCtx.setState({assetGenerationUuid: prevAssetGenerationUuid})
            }
          }}
          disabled={assetGenerationIndex === -1 || !prevAssetGenerationUuid}
          aria-label={t('asset_lab.detail_view.close')}
        />
      </div>
      <div className={classes.detailViewContainer}>
        {assetGeneration.assetType === 'IMAGE' && <AssetLabDetailViewImage />}
        {requestType === 'IMAGE_TO_MESH' && <AssetLabDetailViewMesh />}
        {requestType === 'MESH_TO_ANIMATION' && <AssetLabDetailViewAnimation />}
      </div>
      <div className={classes.button}>
        <IconButton
          stroke='chevronRight'
          text={t('asset_lab.detail_view.next_asset')}
          onClick={() => {
            if (nextAssetGenerationUuid) {
              assetLabCtx.setState({assetGenerationUuid: nextAssetGenerationUuid})
            }
          }}
          disabled={assetGenerationIndex === -1 || !nextAssetGenerationUuid}
          aria-label={t('asset_lab.detail_view.close')}
        />
      </div>
    </div>
  )
}

export {
  AssetLabDetailView,
}
