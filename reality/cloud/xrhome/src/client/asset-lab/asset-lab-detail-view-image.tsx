import React from 'react'
import {useTranslation} from 'react-i18next'

import {Icon} from '../ui/components/icon'
import {backToLibraryClear, useAssetLabStateContext} from './asset-lab-context'
import {useSelector} from '../hooks'
import {getAssetGenUrl} from './urls'
import {DetailViewLeftSnackbar} from './widgets/asset-lab-detail-view-snackbar'
import {
  AssetLabDownloadButton, AssetLabImportButton, DetailViewSendToMenu, AssetLabLibraryButton,
} from './widgets/detail-view-button-widgets'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  image: {
    maxWidth: '100%',
    maxHeight: '100%',
    objectFit: 'contain',
  },
  promptField: {
    'flex': '1 1 auto',
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
    'borderRadius': '.5em',
    'backgroundColor': theme.sfcBackgroundDefault,
    'color': theme.fgMuted,
    'fontSize': '14px',
    'height': '38px',
    'padding': '0 0.75rem',
    'overflow': 'hidden',
    'gap': '0.75rem',
  },
  prompt: {
    'fontFamily': 'inherit',
    'cursor': 'text',
    'width': '100%',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    'whiteSpace': 'nowrap',
    'overflow': 'hidden',
    'maxWidth': '100%',
    'overflowX': 'auto',
    '&::-webkit-scrollbar': {
      height: '3px',
      background: 'transparent',
    },
  },
  copyButton: {
    'border': 'none',
    'background': 'none',
    'cursor': 'pointer',
    '&:disabled': {
      cursor: 'default',
    },
  },
  copyIcon: {
    display: 'flex',
    color: theme.fgMuted,
  },
  imageContainer: {
    'flex': '1 1 auto',
    'borderRadius': '0.5rem',
    'backgroundColor': theme.sfcBackgroundDefault,
    'margin': '0.5rem 0',
    'display': 'flex',
    'overflow': 'hidden',
    'justifyContent': 'center',
  },
  row: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
  },
}))

const AssetLabDetailViewImage = () => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  const assetLabCtx = useAssetLabStateContext()
  const {assetGenerationUuid} = assetLabCtx.state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])
  // @ts-expect-error TODO(christoph): Clean up
  const genUser = useSelector(s => s.team.roles.find(e => e.uuid === assetGeneration.UserUuid))
  const [copiedPrompt, setCopiedPrompt] = React.useState(false)
  const timerRef = React.useRef<ReturnType<typeof setTimeout>>()
  const assetRef = React.useRef<HTMLImageElement>(null)

  React.useEffect(() => {
    if (!copiedPrompt) {
      return undefined
    }
    if (timerRef.current) {
      clearTimeout(timerRef.current)
    }
    timerRef.current = setTimeout(() => setCopiedPrompt(false), 3000)
    return () => clearTimeout(timerRef.current)
  }, [copiedPrompt])

  const copyPrompt = () => {
    navigator.clipboard.writeText(assetGeneration.prompt)
    setCopiedPrompt(true)
  }

  return (
    <>
      <div className={classes.row}>
        <AssetLabLibraryButton onClick={() => {
          backToLibraryClear(assetLabCtx, {scrollToUuid: assetGenerationUuid})
        }}
        />
        <div className={classes.promptField}>
          <div className={classes.prompt}>
            {assetGeneration.prompt}
          </div>
          <button
            type='button'
            onClick={copyPrompt}
            aria-label={t('button.copy', {ns: 'common'})}
            className={classes.copyButton}
            disabled={copiedPrompt}
          >
            <div className={classes.copyIcon}>
              {copiedPrompt ? <Icon stroke='checkmark' /> : <Icon stroke='copy' />}
            </div>
          </button>
        </div>
      </div>
      <div className={classes.imageContainer}>
        <img
          className={classes.image}
          crossOrigin='anonymous'
          ref={assetRef}
          decoding='async'
          loading='lazy'
          src={getAssetGenUrl(assetGeneration)}
          alt={assetGeneration.prompt}
        />
      </div>
      <div className={classes.row}>
        <DetailViewLeftSnackbar
          user={genUser ? {givenName: genUser.given_name, familyName: genUser.family_name} : null}
          assetGeneration={assetGeneration}
        />
        <div className={classes.row}>
          <DetailViewSendToMenu assetGenUuid={assetGenerationUuid} />
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
  AssetLabDetailViewImage,
}
