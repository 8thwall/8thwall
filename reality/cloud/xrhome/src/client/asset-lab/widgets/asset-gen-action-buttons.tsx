import React from 'react'

import {useSelector} from '../../hooks'
import {createThemedStyles} from '../../ui/theme'
import {
  AssetLabDownloadButton, AssetLabImportButton, DetailViewSendToMenu,
} from './detail-view-button-widgets'
import {hexColorWithAlpha} from '../../../shared/colors'
import {brandBlack} from '../../static/styles/settings'

const useStyles = createThemedStyles(() => ({
  overlay: {
    'position': 'absolute',
    'top': 0,
    'left': 0,
    'right': 0,
    'bottom': 0,
    'backgroundColor': hexColorWithAlpha(brandBlack, 0.6),
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'borderRadius': '6px',
  },
  buttonContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5rem',
    alignItems: 'center',
  },
  bottomRight: {
    position: 'absolute',
    bottom: '0.5rem',
    right: '0.5rem',
    zIndex: 1,
  },
}))

interface IAssetGenActionButtons {
  id: string
  bottomRight?: React.ReactNode
}

const AssetGenActionButtons: React.FC<IAssetGenActionButtons> = ({
  id, bottomRight,
}) => {
  const assetGen = useSelector(state => state.assetLab.assetGenerations[id])

  const classes = useStyles()
  return (
    <div className={classes.overlay}>
      <div className={classes.buttonContainer}>
        <DetailViewSendToMenu assetGenUuid={id} placement='bottom' />
        <AssetLabDownloadButton assetGenUuid={id} />
        <AssetLabImportButton assetGenUuid={id} type={assetGen.assetType} />
      </div>
      <div className={classes.bottomRight}>
        {bottomRight}
      </div>
    </div>
  )
}

export {
  AssetGenActionButtons,
}
