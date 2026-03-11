import React, {useEffect, useState} from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import type {IImageTarget} from '../../common/types/models'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {Icon} from '../../ui/components/icon'
import {FloatingPanelIconButton} from '../../ui/components/floating-panel-icon-button'
import {JointToggleButton} from '../../ui/components/joint-toggle-button'
import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import CylindricalImageTargetGeometryVisualizer
  from '../../apps/image-targets/cylindrical-image-target-geometry-visualizer'
import {
  getConinessForRadii, MIN_CIRCUMFERENCE, MIN_CYLINDER_SIDE_LENGTH,
} from '../../apps/image-targets/curved-geometry'
import {RangeSliderInput} from '../../ui/components/range-slider-input'
import {ImageTargetArcVisualizer} from './image-target-arc-visualizer'
import {combine} from '../../common/styles'
import {brandBlack} from '../../static/styles/settings'
import {recalculateMetadata, type ImageTargetMetadata} from './image-target-geometry-configurator'
import {Loader} from '../../ui/components/loader'
import UnconifyWorker from '../../apps/image-targets/conical/unconify.worker'
import {computePixelPointsFromRadius} from '../../apps/image-targets/conical/unconify'
import {useCanvasPool, useImgPool} from '../../common/resource-pool'
import {
  getImageFromTag, processImageData, getMaximumCropAreaPixels, loadImage, getMaxZoom,
  getUnconifiedHeight, getBottomRadius, getMinimumTopRadius,
} from '../../apps/image-targets/image-helpers'
import {ImageTargetTrackingRegion, TrackingRegion} from './image-target-tracking-region'
import type {VisualizerFocus, VisualizerState} from './image-target-asset-configurator'
import {useUpdateEffect} from '../../hooks/use-change-effect'
import {MINIMUM_LONG_LENGTH, MINIMUM_SHORT_LENGTH} from '../../../shared/xrengine-config'
import {RowContent} from './row-content'

type ArcCurveFocus = 'top' | 'bottom'

const useStyles = createThemedStyles(theme => ({
  visualizerContainer: {
    backgroundColor: theme.studioAssetBg,
    border: `3px solid ${theme.studioAssetBorder}`,
    borderRadius: '1em',
    display: 'flex',
    flexDirection: 'column',
    overflow: 'clip',
  },
  content: {
    aspectRatio: '3 / 4',
    width: '100%',
    height: '100%',
  },
  canvas: {
    backgroundColor: theme.studioAssetBg,
  },
  img: {
    verticalAlign: 'top',
  },
  empty: {
    backgroundColor: theme.sfcDisabledColor,
    color: brandBlack,
    textAlign: 'center',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    padding: '15%',
  },
  iconHeader: {
    'display': 'flex',
    'flexDirection': 'row',
    'justifyContent': 'center',
    'alignItems': 'center',
    'gap': '0.5em',
    '& h4': {
      'marginTop': '0',
    },
    'marginBottom': '1em',
  },
  warning: {
    backgroundColor: theme.sfcDisabledColor,
    color: brandBlack,
    textAlign: 'center',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    padding: '12%',
  },
}))

const useCustomStyles = createCustomUseStyles<{focus?: VisualizerFocus}>()(() => ({
  innerCanvasContainer: {
    position: 'relative',
  },
  bottomBar: {
    width: '100%',
    height: 'fit-content',
    padding: '4px',
    gap: '4px',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    flexDirection: ({focus}) => (focus === 'arcCurves' ? 'column' : 'row'),
  },
  iconContainer: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    gap: '4px',
  },
  backButtonContainer: {
    position: 'absolute',
    top: 0,
    left: 0,
    padding: '12px',
    zIndex: 10,
  },
  arcCurveInnerContainer: {
    display: 'flex',
    flexDirection: 'row',
    gap: '2px',
    width: '100%',
    padding: '0px 8px 2px',
  },
  flippedHorizontally: {
    '& svg': {
      transform: 'scaleX(-1)',
    },
  },
}))

interface IImageTargetVisualizerFrame {
  children?: React.ReactNode
}

const ImageTargetVisualizerFrame: React.FC<IImageTargetVisualizerFrame> = ({children}) => {
  const classes = useStyles()

  return (
    <RowContent>
      <div className={classes.visualizerContainer}>
        {children}
      </div>
    </RowContent>
  )
}

const ImageTargetVisualizerMessage: React.FC<{showWarning: boolean}> = ({showWarning = false}) => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')

  if (showWarning) {
    return (
      <div className={combine(classes.content, classes.warning)}>
        <div className={classes.iconHeader}>
          <Icon stroke='warning' color='warning' />
          <h4>{t('image_target_configurator.visualizer.warning.header')}</h4>
        </div>
        <p>{t('image_target_configurator.visualizer.warning.missing_reference')}</p>
        <p>{t('image_target_configurator.visualizer.warning.resolve')}</p>
      </div>
    )
  } else {
    return (
      <div className={combine(classes.content, classes.empty)}>
        <h4>{t('image_target_configurator.visualizer.empty.header')}</h4>
        <p>{t('image_target_configurator.visualizer.empty.body')}</p>
      </div>
    )
  }
}

interface IImageTargetVisualizer {
  imageTarget: IImageTarget
  metadataOverrides?: Partial<ImageTargetMetadata>
  originalImageOverride?: string
  croppedImageOverride?: string
  highlightCircumferenceTop?: boolean
  highlightTargetArcLength?: boolean
  showWarning?: boolean
  loading?: boolean
}

const ImageTargetVisualizer: React.FC<IImageTargetVisualizer> = ({
  imageTarget, metadataOverrides = {}, croppedImageOverride, originalImageOverride,
  highlightCircumferenceTop = false, highlightTargetArcLength = false, showWarning = false,
  loading = false,
}) => {
  const classes = useStyles()
  const curved = imageTarget?.type === 'CYLINDER' || imageTarget?.type === 'CONICAL'
  const metadata = React.useMemo<ImageTargetMetadata>(() => (imageTarget && {
    ...JSON.parse(imageTarget.metadata), ...metadataOverrides,
  }), [imageTarget?.metadata, JSON.stringify(metadataOverrides)])

  if (loading) {
    return (
      <div className={classes.content}>
        <Loader />
      </div>
    )
  }

  if (!imageTarget) {
    return (
      <ImageTargetVisualizerMessage showWarning={showWarning} />
    )
  }

  return curved
    ? <CylindricalImageTargetGeometryVisualizer
        className={combine(classes.content, classes.canvas)}
        cylinderCircumferenceTop={metadata.cylinderCircumferenceTop || MIN_CIRCUMFERENCE}
        cylinderCircumferenceBottom={metadata.cylinderCircumferenceBottom || MIN_CIRCUMFERENCE}
        cylinderSideLength={metadata.cylinderSideLength || MIN_CYLINDER_SIDE_LENGTH}
        targetCircumferenceTop={metadata.targetCircumferenceTop || MIN_CIRCUMFERENCE}
        originalImagePath={originalImageOverride ||
          imageTarget.geometryTextureImageSrc || imageTarget.originalImageSrc}
        imagePath={croppedImageOverride || imageTarget.imageSrc}
        imageLandscape={metadata.isRotated}
        x={metadata.left}
        y={metadata.top}
        width={metadata.width}
        height={metadata.height}
        originalWidth={metadata.originalWidth}
        originalHeight={metadata.originalHeight}
        highlightCircumferenceTop={highlightCircumferenceTop}
        highlightCircumferenceBottom={false}
        highlightTargetArcLength={highlightTargetArcLength}
        isAutoPan={false}
    />
    : <img
        alt={imageTarget?.name}
        src={croppedImageOverride || imageTarget?.imageSrc}
        className={combine(classes.content, classes.img)}
    />
}

interface IImageTargetTrackingConfigurator {
  imageTarget: IImageTarget
  metadata: ImageTargetMetadata
  onMetadataChange: (metadata: Partial<ImageTargetMetadata>) => void
  visualizerState: VisualizerState
  onVisualizerStateChange: (state: Partial<VisualizerState>) => void
  onTrackingRegionChange(region: TrackingRegion): void
}

const ImageTargetTrackingConfigurator: React.FC<IImageTargetTrackingConfigurator> = ({
  imageTarget, metadata, onMetadataChange, visualizerState, onVisualizerStateChange,
  onTrackingRegionChange,
}) => {
  const {originalImageSrc, geometryTextureImageSrc, type} = imageTarget
  const {originalWidth, originalHeight, isRotated, topRadius, bottomRadius, arcAngle} = metadata
  const {
    focus, conicalOriginalImage, fullImageUnrotated, fullImageRotated, unconifying, prevOriginalUrl,
    prevTopRadius, prevBottomRadius, croppedImg, prevFullUrl, prevCropArea,
  } = visualizerState

  const {t} = useTranslation('cloud-studio-pages')
  const classes = useCustomStyles({focus})

  const [zoom, setZoom] = React.useState(1)
  const [maxZoom, setMaxZoom] = useState(getMaxZoom(originalWidth, originalHeight, isRotated))
  const [crop, setCrop] = React.useState({x: 0, y: 0})
  const [arcCurveFocus, setArcCurveFocus] = React.useState<ArcCurveFocus>('top')
  const [userIsRotated, setUserIsRotated] = useState(isRotated)
  const canChangeOrientation = getMaxZoom(originalHeight, originalWidth, !isRotated) >= 1

  const handleOrientationChange = () => {
    setUserIsRotated(!isRotated)
    onMetadataChange({
      isRotated: !isRotated,
      originalWidth: originalHeight,
      originalHeight: originalWidth,
    })
  }

  const fullImage = isRotated ? fullImageRotated : fullImageUnrotated
  const needsUniconify = type === 'CONICAL' && conicalOriginalImage && (
    conicalOriginalImage.url !== prevOriginalUrl ||
    topRadius !== prevTopRadius ||
    bottomRadius !== prevBottomRadius)
  const needsCrop = fullImage && !unconifying && !needsUniconify && (
    fullImage.url !== prevFullUrl ||
    prevCropArea.left !== metadata.left ||
    prevCropArea.top !== metadata.top ||
    prevCropArea.width !== metadata.width ||
    prevCropArea.height !== metadata.height)

  const imgPool = useImgPool()
  const canvasPool = useCanvasPool()
  const worker = React.useRef<UnconifyWorker>()

  useEffect(() => {
    worker.current = new UnconifyWorker()
    worker.current.onmessage = (event) => {
      onVisualizerStateChange({
        fullImageUnrotated: processImageData(event.data, canvasPool, 0),
        fullImageRotated: processImageData(event.data, canvasPool, 1),
        unconifying: false,
      })
    }
  }, [])

  const loadConicalOriginalImage = async (url: string) => {
    if (conicalOriginalImage) return
    const imgTag = await loadImage(url, imgPool)
    imgTag.crossOrigin = 'anonymous'
    await imgTag.decode()
    const img = getImageFromTag(imgTag, canvasPool)
    onVisualizerStateChange({conicalOriginalImage: {...img, url}})
  }

  const loadFullImage = async (url: string, alreadyRotated: boolean) => {
    if (fullImageRotated && fullImageUnrotated) return
    const imgTag = await loadImage(url, imgPool)
    imgTag.crossOrigin = 'anonymous'
    await imgTag.decode()
    const img = getImageFromTag(imgTag, canvasPool)
    if (alreadyRotated) {
      onVisualizerStateChange({
        fullImageRotated: {...img, url},
        fullImageUnrotated: processImageData(img.data, canvasPool, -1),
      })
    } else {
      onVisualizerStateChange({
        fullImageUnrotated: {...img, url},
        fullImageRotated: processImageData(img.data, canvasPool, 1),
      })
    }
  }

  useEffect(() => {
    if (type === 'CONICAL') {
      loadConicalOriginalImage(originalImageSrc)
      loadFullImage(geometryTextureImageSrc, isRotated)
    } else {
      loadFullImage(originalImageSrc, isRotated)
    }
  }, [type, originalImageSrc, geometryTextureImageSrc])

  useEffect(() => {
    // re-unconify when the user leaves the arc editor with new curves
    if (focus !== 'arcCurves' && needsUniconify) {
      onVisualizerStateChange({
        unconifying: true,
        prevTopRadius: topRadius,
        prevBottomRadius: bottomRadius,
        prevOriginalUrl: conicalOriginalImage.url,
      })
      const pixelWidth = isRotated ? originalHeight : originalWidth
      worker.current.postMessage({
        imgData: conicalOriginalImage.data,
        pixelPoints: computePixelPointsFromRadius(topRadius, bottomRadius, pixelWidth),
        outputWidth: pixelWidth,
      })
    }
  }, [focus, needsUniconify, topRadius, bottomRadius, isRotated, originalHeight,
    originalWidth, conicalOriginalImage])

  useEffect(() => {
    // re-crop when the user leaves the tracking region
    if (focus !== 'trackingRegion' && needsCrop) {
      onVisualizerStateChange({
        prevCropArea: {...metadata},
        prevFullUrl: fullImage.url,
        croppedImg: processImageData(fullImage.data, canvasPool, 0, metadata),
      })
    }
  }, [focus, needsCrop, metadata, fullImage, canvasPool])

  const [radiusDiff, setRadiusDiff] = useState(Math.abs(topRadius) - bottomRadius)

  useUpdateEffect(() => {
    setRadiusDiff(Math.abs(topRadius) - bottomRadius)
  }, [arcCurveFocus])

  const conicalOriginalWidth = conicalOriginalImage?.data?.width ?? 0
  const conicalOriginalHeight = conicalOriginalImage?.data?.height ?? 0
  const minimumOutputHeight = conicalOriginalWidth < MINIMUM_LONG_LENGTH
    ? MINIMUM_LONG_LENGTH
    : MINIMUM_SHORT_LENGTH

  const maxBottomRadius = getBottomRadius(topRadius, conicalOriginalWidth, minimumOutputHeight)
  const minBottomRadius = Math.max(Math.abs(topRadius) - conicalOriginalHeight, 1)

  const maxAbsTopRadius = conicalOriginalHeight * 10
  const minAbsTopRadius = getMinimumTopRadius(conicalOriginalWidth, minimumOutputHeight)
  const topRadiusRange = maxAbsTopRadius - minAbsTopRadius

  // remapping the input so the radius flips at the halfway point
  // also rescaling [-1, 1] with x^2 to give more granularity at smaller arcs
  const inputToTopRadius = (input: number) => {
    const scaleAdjusted = Math.sign(input) * input ** 2
    return scaleAdjusted * topRadiusRange + (input < 0 ? -1 : 1) * minAbsTopRadius
  }

  const topRadiusToInput = (radius) => {
    const remapped = (radius - Math.sign(radius) * minAbsTopRadius) / topRadiusRange
    return Math.sign(radius) * Math.sqrt(Math.abs(remapped))
  }

  const radiusSliderMin = arcCurveFocus === 'top' ? -1 : minBottomRadius
  const radiusSliderMax = arcCurveFocus === 'top' ? 1 : maxBottomRadius

  const onRadiusChange = (inputRadius: number) => {
    const newTopRadius = arcCurveFocus === 'top'
      ? inputToTopRadius(inputRadius)
      : topRadius
    const newBottomRadius = arcCurveFocus === 'bottom'
      ? inputRadius
      : Math.min(Math.max(Math.abs(newTopRadius) - radiusDiff, 1),
        getBottomRadius(newTopRadius, conicalOriginalWidth, minimumOutputHeight))
    const newConiness = getConinessForRadii(newTopRadius, newBottomRadius)

    const unconifiedWidth = isRotated ? originalHeight : originalWidth
    const unconifiedHeight = getUnconifiedHeight(newTopRadius, newBottomRadius, unconifiedWidth)

    const forceRotate = (!userIsRotated && unconifiedHeight < MINIMUM_LONG_LENGTH) ||
                        (userIsRotated && unconifiedWidth < MINIMUM_LONG_LENGTH)
    const newIsRotated = forceRotate ? !userIsRotated : userIsRotated

    const newOriginalWidth = newIsRotated ? unconifiedHeight : unconifiedWidth
    const newOriginalHeight = newIsRotated ? unconifiedWidth : unconifiedHeight
    const unrotatedCrop = getMaximumCropAreaPixels(newOriginalWidth, newOriginalHeight, 3 / 4)

    onMetadataChange(recalculateMetadata(type, {
      arcAngle,
      coniness: newConiness,
      isRotated: newIsRotated,
      originalWidth: newOriginalWidth,
      originalHeight: newOriginalHeight,
      topRadius: newTopRadius,
      bottomRadius: newBottomRadius,
      left: unrotatedCrop.left,
      top: unrotatedCrop.top,
      width: unrotatedCrop.width,
      height: unrotatedCrop.height,
    }))
  }

  return (
    <ImageTargetVisualizerFrame>
      <div className={classes.innerCanvasContainer}>
        {focus !== 'main' &&
          <div className={classes.backButtonContainer}>
            <FloatingPanelIconButton
              text={t('asset_configurator.image_target_configurator.visualizer.back')}
              stroke='backArrow'
              onClick={() => onVisualizerStateChange({focus: 'main'})}
            />
          </div>
        }
        {focus === 'main' &&
          <ImageTargetVisualizer
            imageTarget={imageTarget}
            metadataOverrides={metadata}
            originalImageOverride={fullImage?.url}
            croppedImageOverride={croppedImg?.url}
            loading={unconifying}
          />
        }
        {focus === 'arcCurves' &&
          <ImageTargetArcVisualizer
            imageTarget={imageTarget}
            topRadius={topRadius}
            bottomRadius={bottomRadius}
            arcCurveFocus={arcCurveFocus}
          />
        }
        {focus === 'trackingRegion' &&
          <ImageTargetTrackingRegion
            imageTarget={imageTarget}
            crop={crop}
            zoom={zoom}
            maxZoom={maxZoom}
            isRotated={isRotated}
            fullImageRotated={fullImageRotated}
            fullImageUnrotated={fullImageUnrotated}
            onCropChange={setCrop}
            onZoomChange={setZoom}
            onMaxZoomChange={setMaxZoom}
            onTrackingRegionChange={onTrackingRegionChange}
          />
        }
      </div>
      <div className={classes.bottomBar}>
        {focus === 'main' && (
          <>
            {type === 'CONICAL' && (
              <FloatingPanelButton
                grow
                onClick={() => onVisualizerStateChange({focus: 'arcCurves'})}
              >
                <div className={classes.iconContainer}>
                  <Icon stroke='arcCurve' inline size={0.85} />
                  {t('asset_configurator.image_target_configurator.visualizer.arc_curves')}
                </div>
              </FloatingPanelButton>
            )}
            <FloatingPanelButton
              grow={type === 'CONICAL'}
              onClick={() => onVisualizerStateChange({focus: 'trackingRegion'})}
            >
              <div className={classes.iconContainer}>
                <Icon stroke='regionSelect' inline size={0.75} />
                {t('asset_configurator.image_target_configurator.visualizer.tracking_region')}
              </div>
            </FloatingPanelButton>
          </>
        )}
        {focus === 'trackingRegion' && (
          <>
            <Icon stroke='regionSelect' inline size={1.25} />
            <RangeSliderInput
              id='zoom-tracking-region'
              value={zoom}
              min={1}
              step={0.002}
              max={Math.max(1, maxZoom)}
              onChange={num => setZoom(num)}
              trackColor={theme => theme.fgMuted}
              color={theme => theme.fgMain}
            />
            <div className={isRotated ? classes.flippedHorizontally : undefined}>
              <FloatingPanelIconButton
                // eslint-disable-next-line max-len
                text={t('asset_configurator.image_target_configurator.visualizer.tracking_regions.swap_orientations')}
                stroke='swapOrientation'
                buttonSize='tiny'
                onClick={handleOrientationChange}
                disabled={!canChangeOrientation}
              />
            </div>
          </>
        )}
        {focus === 'arcCurves' && (
          <>
            {/* TODO (jeffha): support bolding on `top` / `bottom` */ }
            {t('asset_configurator.image_target_configurator.visualizer.arc_curves.title', {
              focus: arcCurveFocus,
            })}
            <div className={classes.arcCurveInnerContainer}>
              <JointToggleButton
                options={[
                  {
                    value: 'top',
                    content: (
                      <div className={classes.iconContainer}>
                        <Icon stroke='topArcCurve' />
                      </div>
                    ),
                  },
                  {
                    value: 'bottom',
                    content: (
                      <div className={classes.iconContainer}>
                        <Icon stroke='bottomArcCurve' />
                      </div>
                    ),
                  },
                ]}
                value={arcCurveFocus}
                onChange={value => setArcCurveFocus(value)}
              />
              <RangeSliderInput
                id='arc-curves-focus'
                value={arcCurveFocus === 'top' ? topRadiusToInput(topRadius) : bottomRadius}
                min={radiusSliderMin}
                step={(radiusSliderMax - radiusSliderMin) / 1000}
                max={radiusSliderMax}
                onChange={onRadiusChange}
                trackColor={theme => theme.fgMuted}
                color={theme => theme.fgMain}
                centerMark={arcCurveFocus === 'top'}
              />
            </div>
          </>
        )}
      </div>
    </ImageTargetVisualizerFrame>
  )
}

export type {
  ArcCurveFocus,
}

export {
  ImageTargetVisualizerFrame,
  ImageTargetVisualizer,
  ImageTargetTrackingConfigurator,
}
