import React, {useEffect, useState} from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {
  IMAGE_FILES,
  SPECIAL_IMAGE_FILES,
  VIDEO_FILES,
  AUDIO_FILES,
  NO_PREVIEW_MODEL_FILES,
  NORMAL_MODEL_FILES,
  SPLAT_MODEL_FILES,
} from '../../common/editor-files'
import {fetchSize} from '../../common/fetch-utils'
import {fileExt} from '../../editor/editor-common'
import type {AdditionalAssetData, AssetDimension} from '../../editor/asset-preview-types'
import {EditorFileLocation, extractFilePath} from '../../editor/editor-file-location'
import {FileNameConfigurator} from './file-name-configurator'
import {useFileContent} from '../../git/hooks/use-file-content'
import {
  ImagePreview,
  VideoPreview,
  AudioPreview,
  CustomImagePreview,
} from '../asset-previews/studio-asset-previewers'
import {GltfAssetWrapper} from '../asset-previews/gltf-asset-wrapper'
import {useAbandonableEffect} from '../../hooks/abandonable-effect'
import {formatBytesToText} from '../../../shared/asset-size-limits'
import {AssetInfo} from './asset-info'
import {Loader} from '../../ui/components/loader'
import {SplatAssetWrapper} from './splat-asset-wrapper'
import {useResourceUrl} from '../hooks/resource-url'
import {StaticBanner} from '../../ui/components/banner'
import {ErrorBoundary} from '../../common/error-boundary'

const sizeCache: Record<string, number> = {}

const useStyles = createUseStyles({
  modelGrid: {
    display: 'flex',
    width: '100%',
    height: '40%',
  },
  resizeElement: {
    'justifyContent': 'center',
    'alignItems': 'center',
    'width': '100%',
    'height': 'auto',
    '& img, & video, & canvas': {
      maxWidth: '100%',
      height: 'auto',
      objectFit: 'contain',
      maxHeight: '70vh',
      borderRadius: '0.5rem',
    },
  },
  assetContainer: {
    padding: '1em 1em 0em',
    width: '100%',
    height: 'auto',
  },
  metaDataContainer: {
    padding: '1em 1em',
  },
  hidden: {
    visibility: 'hidden',
  },
  wrapper: {
    height: '100%',
  },
})

interface IAssetConfigurator {
  selectedAsset: EditorFileLocation
}

const AssetConfigurator: React.FC<IAssetConfigurator> = ({
  selectedAsset,
}) => {
  const classes = useStyles()

  const {t} = useTranslation(['cloud-editor-pages', 'cloud-studio-pages'])

  const [fileSize, setFileSize] = useState<number>(null)
  const [dimensions, setDimensions] = useState<AssetDimension>(null)
  const [duration, setDuration] = useState<Number>(null)

  const selectedAssetPath = extractFilePath(selectedAsset)
  const assetContent = useFileContent(selectedAssetPath)
  const fullUrl = useResourceUrl(selectedAssetPath)
  const ext = selectedAssetPath && fileExt(selectedAssetPath)

  // handle preview for custom font conversion
  const customFontPreviewUrl = React.useMemo(() => null,
  // TODO(christoph): Bring back
  // if (!CUSTOM_FONT_FILES.includes(ext)) {
  //   return null
  // }

    // const pointer = parse(assetContent)
    // if (!isBundle(pointer)) {
    //   return null
    // }

    // const {files} = pointer
    // const fontFilePath = Object.keys(files)
    //   .find(filePath => CUSTOM_FONT_FILES.includes(fileExt(filePath)))
    // const imageFilePath = Object.keys(files).find(filePath => fileExt(filePath) === 'png')

    // if (fontFilePath && imageFilePath) {
    //   return getAssetUrl(pointer.path + imageFilePath)
    // }

    [assetContent])

  const [loadedFor, setLoadedFor] = useState<string>(null)
  const [showLoading, setShowLoading] = useState(false)
  const hasLoaded = loadedFor && loadedFor === fullUrl
  const loadTimeoutRef = React.useRef<ReturnType<typeof setTimeout>>()

  // for checking if video has audio
  let isVideo = false
  const [audioState, setAudioState] = useState({audioCheckDone: false, hasAudio: true})

  // Clear info on asset change.
  useEffect(() => {
    setFileSize(null)
    setDimensions(null)
    setDuration(null)
    setAudioState({audioCheckDone: false, hasAudio: true})

    setShowLoading(false)
    loadTimeoutRef.current = setTimeout(() => {
      setShowLoading(true)
    }, 750)

    return () => clearTimeout(loadTimeoutRef.current)
  }, [fullUrl])

  // Since the loading can complete after component unmounts, we use abandon to prevent
  // state updates.
  useAbandonableEffect(async (abandon) => {
    if (sizeCache[fullUrl]) {
      setFileSize(sizeCache[fullUrl])
    } else {
      const size = await abandon(fetchSize(fullUrl))
      sizeCache[fullUrl] = size
      setFileSize(size)
    }
  }, [fullUrl])

  const onMoreData = (data: Partial<AdditionalAssetData>) => {
    const {dimension, duration: updatedDuration} = data
    if (dimension) {
      setDimensions(dimension)
    }
    if (updatedDuration) {
      setDuration(updatedDuration)
    }
  }

  const onAssetLoad = () => {
    setLoadedFor(fullUrl)
    setShowLoading(false)
  }

  if (!selectedAsset) {
    return null
  }
  const ALL_MODEL_FILES = [...NORMAL_MODEL_FILES, ...SPLAT_MODEL_FILES]
  // TODO (Joshua G-K) handle other asset types
  if (ALL_MODEL_FILES.includes(ext) && !NO_PREVIEW_MODEL_FILES.includes(ext)) {
    return (
      <>
        <FileNameConfigurator location={selectedAsset} />
        <ErrorBoundary
          key={fullUrl}
          fallback={() => (
            <StaticBanner type='danger'>
              {t('asset_configurator.error.failed_to_load_model', {ns: 'cloud-studio-pages'})}
            </StaticBanner>
          )}
        >
          {showLoading && !hasLoaded && <Loader />}
          <React.Suspense fallback={null}>
            {NORMAL_MODEL_FILES.includes(ext) &&
              <div className={!hasLoaded ? classes.hidden : classes.wrapper}>
                <GltfAssetWrapper
                  selectedAssetLocation={selectedAsset}
                  modelUrl={fullUrl}
                  key={fullUrl}
                  onGltfLoad={onAssetLoad}
                  defaultFileSize={fileSize}
                />
              </div>}
            {SPLAT_MODEL_FILES.includes(ext) &&
              <div className={!hasLoaded ? classes.hidden : classes.wrapper}>
                <SplatAssetWrapper
                  url={fullUrl}
                  onSplatLoad={onAssetLoad}
                  defaultFileSize={fileSize}
                />
              </div>}
          </React.Suspense>
        </ErrorBoundary>
      </>
    )
  } else {
    let previewElement

    if (IMAGE_FILES.includes(ext)) {
      previewElement = (
        <div className={classes.resizeElement}>
          <ImagePreview
            key={fullUrl}
            src={fullUrl}
            alt={t('editor_page.asset_preview.img_alt.preview', {selectedAssetPath})}
            onMoreData={onMoreData}
            onAssetLoad={onAssetLoad}
          />
        </div>
      )
    } else if (SPECIAL_IMAGE_FILES.includes(ext)) {
      previewElement = (
        <div className={classes.resizeElement}>
          <CustomImagePreview
            key={fullUrl}
            src={fullUrl}
            onMoreData={onMoreData}
            onAssetLoad={onAssetLoad}
          />
        </div>
      )
    } else if (VIDEO_FILES.includes(ext)) {
      isVideo = true
      previewElement = (
        <div className={classes.resizeElement}>
          <VideoPreview
            key={fullUrl}
            src={fullUrl}
            onMoreData={onMoreData}
            onAssetLoad={onAssetLoad}
            audioState={audioState}
            setAudioState={setAudioState}
          />
        </div>
      )
    } else if (AUDIO_FILES.includes(ext)) {
      previewElement = (
        <AudioPreview
          key={fullUrl}
          src={fullUrl}
          onMoreData={onMoreData}
          onAssetLoad={onAssetLoad}
        />
      )
    } else if (customFontPreviewUrl) {
      previewElement = (
        <div className={classes.resizeElement}>
          <ImagePreview
            key={customFontPreviewUrl}
            src={customFontPreviewUrl}
            alt={t('editor_page.asset_preview.img_alt.preview', {selectedAssetPath})}
            onMoreData={() => {}}
            onAssetLoad={onAssetLoad}
          />
        </div>
      )
    } else {
      return (
        <div className='preview-not-available'>
          {t('editor_page.asset_preview.preview_not_available')}
        </div>
      )
    }

    const displayMetadata = fileSize || dimensions || duration
    const showAsset = hasLoaded && (!isVideo || audioState.audioCheckDone)

    const asset = (
      <div className={!showAsset ? classes.hidden : ''}>
        <div className={classes.assetContainer}>
          {previewElement}
        </div>
        {displayMetadata &&
          <div className={classes.metaDataContainer}>
            <AssetInfo
              metadata={{
                size: {
                  label: t('editor_page.asset_preview.metadata.file_size'),
                  value: formatBytesToText(fileSize),
                },
                dimensions: {
                  label: t('editor_page.asset_preview.metadata.dimensions'),
                  value: dimensions && `${dimensions.width} x ${dimensions.height}`,
                },
              }}
            />
          </div>
        }
      </div>
    )

    return (
      <>
        <FileNameConfigurator location={selectedAsset} />
        {showLoading && !showAsset && <Loader />}
        {asset}
      </>
    )
  }
}

export default AssetConfigurator
