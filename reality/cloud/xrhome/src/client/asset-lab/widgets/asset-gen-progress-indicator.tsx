import React from 'react'
import {useTranslation} from 'react-i18next'

import {Loader} from '../../ui/components/loader'
import {useSelector} from '../../hooks'
import type {AssetRequest} from '../../common/types/db'
import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  'overlay': {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    color: theme.overlayFg,
  },
  'placeholder': {
    color: theme.overlayBg,
  },
  'animatedBorder': {
    border: '2px solid #BA4FFF',
    borderRadius: '0.75rem',
    boxSizing: 'border-box',
    animation: '$borderColorCycle 4s linear infinite',
  },
  '@keyframes borderColorCycle': {
    '0%': {borderColor: '#BA4FFF'},
    '33%': {borderColor: '#FF4713'},
    '66%': {borderColor: '#FFC828'},
    '100%': {borderColor: '#BA4FFF'},
  },
  'loaderText': {
    userSelect: 'none',
  },
}))

interface IAssetGenProgressIndicator {
  id: string
  // Show this when asset request is not found for this id.
  placeholder?: React.ReactNode
}

const AssetGenProgressIndicator: React.FC<IAssetGenProgressIndicator> = ({id, placeholder}) => {
  const assetReq = useSelector(state => state.assetLab.assetRequests[id])
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()

  const isLoading = assetReq?.status === 'REQUESTED' || assetReq?.status === 'PROCESSING'
  const isFailed = assetReq?.status === 'FAILED'
  const isSuccess = assetReq?.status === 'SUCCESS'

  const getLoader = (status: AssetRequest['status']) => {
    if (!status) {
      return (
        <div className={classes.placeholder}>
          {placeholder || t('asset_lab.generate.ready')}
        </div>
      )
    }

    if (isLoading) {
      return (
        <Loader animateSpinnerColor>
          <span className={classes.loaderText}>{t('asset_lab.generate.generating')}</span>
        </Loader>
      )
    }
    if (isFailed) {
      return <div>{t('asset_lab.generate.failed')}</div>
    }
    return null
  }

  if (isSuccess) {
    return null  // No overlay when the asset generation is complete.
  }

  return (
    <div className={combine(classes.overlay, isLoading && classes.animatedBorder)}>
      {getLoader(assetReq?.status)}
    </div>
  )
}

export {
  AssetGenProgressIndicator,
}
