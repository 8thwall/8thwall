import React from 'react'

import {createUseStyles} from 'react-jss'

import {useTranslation} from 'react-i18next'

import StudioThreeModelPreview from '../studio-model-preview'
import {AssetInfo} from './asset-info'
import {formatBytesToText} from '../../../shared/asset-size-limits'

const useStyles = createUseStyles({
  modelContainer: {
    position: 'relative',
    display: 'flex',
    height: '40vh',
  },
  configuratorContainer: {
    margin: '1em',
  },
})

interface ISplatAssetWrapper {
  url: string
  defaultFileSize: number
  onSplatLoad?: () => void
}

const SplatAssetWrapper: React.FC<ISplatAssetWrapper> = ({
  url, defaultFileSize, onSplatLoad,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages'])

  return (
    <>
      <div className={classes.modelContainer}>
        <StudioThreeModelPreview
          src={url}
          wireframe={false}
          onModelLoad={onSplatLoad}
          grid={false}
          gridColor='#ffffff'
        />
      </div>

      <div className={classes.configuratorContainer}>
        <AssetInfo
          metadata={{
            size: {
              label: t('editor_page.asset_preview.metadata.file_size'),
              value: formatBytesToText(defaultFileSize),
            },
          }}
        />
      </div>

    </>

  )
}

export {
  SplatAssetWrapper,
}
