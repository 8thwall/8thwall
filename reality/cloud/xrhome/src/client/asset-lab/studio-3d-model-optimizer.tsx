import React from 'react'
import {useTranslation} from 'react-i18next'

import type {Vec3Tuple} from '@ecs/shared/scene-graph'
import {BASIC} from '@ecs/shared/environment-maps'

import {combine} from '../common/styles'
import {useSelector} from '../hooks'
import {createThemedStyles} from '../ui/theme'
import {Icon} from '../ui/components/icon'
import StudioModelPreviewWithLoader from '../studio/studio-model-preview-with-loader'
import {useAssetLabStateContext} from './asset-lab-context'
import {getMeshUrl, getOptimizedMeshUrl} from '../../shared/genai/constants'
import {StandardCheckboxField} from '../ui/components/standard-checkbox-field'
import {SliderInputAxisField} from '../ui/components/slider-input-axis-field'
import {normalizeDegrees} from '../studio/quaternion-conversion'
import {RangeSliderInput} from '../ui/components/range-slider-input'
import {cherry, gray4, lightBlue, mint} from '../static/styles/settings'
import {
  getDocLastPositionArray, makeGlbFile, makeOptimizerMetadata,
  useGlbPostProcessing,
} from './hooks/use-request-model-post-processing'
import {loadGltfFromArrayBuffer, type GltfData} from '../studio/hooks/gltf'
import {useDebounce} from '../common/use-debounce'
import type {AssetGenerationMeshMetadata, PivotPointOptions, PostProcessingSettings} from './types'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {AssetLabButton} from './widgets/asset-lab-button'
import {useAssetLabStyles} from './asset-lab-styles'
import {getBoundingBox, getPivot} from './hooks/mesh-pivot-point'
import {IconButton} from '../ui/components/icon-button'
import useActions from '../common/use-actions'
import useCurrentAccount from '../common/use-current-account'
import assetLabActions from './asset-lab-actions'
import assetConverterActions from '../studio/actions/asset-converter-actions'
import {ASSET_OPTIMIZER_TEXTURE_SIZES, OptimizerTextureSizes} from './constants'
import {useArrayBufferFetch} from '../studio/hooks/use-fetch'
import {DetailViewButton} from './widgets/detail-view-button-widgets'

const useStyles = createThemedStyles(theme => ({
  optimizerOptionsContainer: {
    'overflowY': 'auto',
    '&::-webkit-scrollbar': {
      width: '3px',
      background: 'transparent',
    },
    'display': 'flex',
    'flexDirection': 'column',
    'gap': '0.62rem',
    'flex': '0 0 250px',
  },
  optimizerOptions: {
    'borderRadius': '0.5rem',
    'border': `1px solid ${theme.subtleBorder}`,
    'display': 'flex',
    'flexDirection': 'column',
    'padding': '0.625rem',
    'gap': '0.5rem',
    '& > label > span': {
      color: theme.fgMuted,
    },
  },
  optionGroupLabel: {
    fontWeight: 700,
    color: 'white',
    marginBottom: 0,
  },
  assetInfoContainer: {
    display: 'flex',
    flexDirection: 'row',
    gap: '.5rem',
    margin: '.5rem 0',
    overflow: 'hidden',
    minWidth: 0,
    alignItems: 'stretch',
    flex: '1 1 auto',
  },
  transformGrid: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0em',
  },
  transformButtonRow: {
    'display': 'flex',
    'gap': '0.5em',
    'color': theme.fgMuted,
    '& > *:hover': {
      color: theme.fgMain,
    },
    'alignItems': 'center',
  },
  transformInput: {
    display: 'flex',
    gap: '0.25em',
    flex: 2,
  },
  inputSliderCombo: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'gap': '0.5em',
    '& > span:first-child': {
      flex: '1 0 6em',
    },
  },
  title: {
    color: theme.fgMain,
    fontWeight: 600,
    fontFamily: 'Geist Mono, monospace',
  },
  beforeAfterBox: {
    position: 'absolute',
    top: '1rem',
    left: '1rem',
    fontFamily: 'Geist Mono, monospace',
  },
  modelContainer: {
    flex: '1 0 80vw',
    borderRadius: '0.5rem',
    backgroundColor: theme.sfcBackgroundDefault,
    display: 'flex',
    overflow: 'hidden',
    justifyContent: 'center',
    position: 'relative',
  },
  beforeText: {
    color: theme.fgMuted,
    textDecoration: 'line-through',
  },
  afterText: {
    color: theme.fgMain,
    fontWeight: 600,
  },
  pivotButtonsContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.3125rem',
  },
  pivotButtons: {
    gap: '0.625rem',
    display: 'flex',
    flexDirection: 'row',
    alignContent: 'center',
    justifyContent: 'center',
  },
  goodMint: {
    color: mint,
  },
  badCherry: {
    color: cherry,
  },
  okLightBlue: {
    color: lightBlue,
  },
  detailViewContainer: {
    'display': 'flex',
    'flexDirection': 'column',
    'flex': '1 1 auto',
    'height': 'calc(92vh - 60px)',
    'overflow': 'hidden',
  },
  row: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
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
  buttonBar: {
    'display': 'flex',
    'flexDirection': 'row',
    'gap': '0.625rem',
    '& > *': {
      marginLeft: 0,
      marginRight: 0,
    },
  },
}))

interface BeforeAfterPair {
  before: string
  after: string
  afterClassName?: string
}

interface GlbDiff {
  size?: BeforeAfterPair
  vertices?: BeforeAfterPair
  tris?: BeforeAfterPair
  maxTexture?: BeforeAfterPair
  compression?: BeforeAfterPair
}

const toHumanReadableBytes = (bytes: number): string => {
  if (!Number.isFinite(bytes) || bytes < 0) {
    return '0B'
  }

  if (bytes < 1024) {
    return `${bytes}B`
  } else if (bytes < 1024 * 1024) {
    return `${(bytes / 1024).toFixed(2)}KB`
  } else if (bytes < 1024 * 1024 * 1024) {
    return `${(bytes / 1024 / 1024).toFixed(2)}MB`
  } else {
    return `${(bytes / 1024 / 1024 / 1024).toFixed(2)}GB`
  }
}

interface IBeforeAfterBox {
  before: GltfData['metadata']
  after: GltfData['metadata']
}

const BeforeAfterBox: React.FC<IBeforeAfterBox> = ({before, after}) => {
  const classes = useStyles()
  const goodIfLessThan = (left: number, right: number): string => {
    if (left < right) {
      return classes.goodMint
    }
    if (left > right) {
      return classes.badCherry
    }
    return ''
  }

  const {t} = useTranslation('asset-lab')
  const diff: GlbDiff = {}
  if (before && after) {
    if (before.vertices !== after.vertices) {
      diff.vertices = {
        before: `${before.vertices}`,
        after: `${after.vertices}`,
        afterClassName: goodIfLessThan(after.vertices, before.vertices),
      }
    }
    if (before.triangles !== after.triangles) {
      diff.tris = {
        before: `${before.triangles}`,
        after: `${after.triangles}`,
        afterClassName: goodIfLessThan(after.triangles, before.triangles),
      }
    }
    if (before.isDraco !== after.isDraco) {
      diff.compression = {
        before: before.isDraco
          ? t('asset_lab.model_optimizer.draco_yes')
          : t('asset_lab.model_optimizer.draco_no'),
        after: after.isDraco
          ? t('asset_lab.model_optimizer.draco_yes')
          : t('asset_lab.model_optimizer.draco_no'),
        afterClassName: after.isDraco ? classes.okLightBlue : classes.badCherry,
      }
    }
    if (before.maxTextureSize !== after.maxTextureSize) {
      diff.maxTexture = {
        before: `${before.maxTextureSize}px`,
        after: `${after.maxTextureSize}px`,
        afterClassName: goodIfLessThan(after.maxTextureSize, before.maxTextureSize),
      }
    }
    if (before.sizeInBytes !== after.sizeInBytes) {
      diff.size = {
        before: toHumanReadableBytes(before.sizeInBytes),
        after: toHumanReadableBytes(after.sizeInBytes),
        afterClassName: goodIfLessThan(after.sizeInBytes, before.sizeInBytes),
      }
    }
  }
  const renderBeforeAfterRow = (tLabel: string, beforeAfter: BeforeAfterPair) => beforeAfter && (
    <div>
      {t(tLabel)}:&nbsp;
      <span className={classes.beforeText}>{beforeAfter.before}</span>&nbsp;
      <span className={combine(classes.afterText, beforeAfter.afterClassName)}>
        {beforeAfter.after}
      </span>
    </div>
  )
  return (
    <>
      {renderBeforeAfterRow('asset_lab.model_optimizer.size', diff.size)}
      {renderBeforeAfterRow('asset_lab.model_optimizer.vertices', diff.vertices)}
      {renderBeforeAfterRow('asset_lab.model_optimizer.tris', diff.tris)}
      {renderBeforeAfterRow('asset_lab.model_optimizer.max_texture', diff.maxTexture)}
      {renderBeforeAfterRow('asset_lab.model_optimizer.compression', diff.compression)}
    </>

  )
}

interface IStudio3dModelOptimizer {
}

const textureResizeOptions = ASSET_OPTIMIZER_TEXTURE_SIZES.map(size => ({
  value: `${size}`,
  content: `${size}x${size}`,
}))
const parseAsTextureSizeOr = (
  size: string, fallback: OptimizerTextureSizes = 512
): OptimizerTextureSizes => {
  const parsed = parseInt(size, 10) as OptimizerTextureSizes
  if (ASSET_OPTIMIZER_TEXTURE_SIZES.includes(parsed)) {
    return parsed
  }
  return fallback
}

const Studio3dModelOptimizer: React.FC<IStudio3dModelOptimizer> = () => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  const assetLabCtx = useAssetLabStateContext()
  const {prevMode, assetGenerationUuid} = assetLabCtx.state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])
  const currentOptimizerInfo =
    (assetGeneration?.metadata as AssetGenerationMeshMetadata)?.optimizer ||
    {
      completed: false,
      settings: {
        enableDraco: true,
        enableOptimizations: true,
        enableZip: false,
        enableSimplify: false,
        simplifyRatio: 0.5,
        enableTextureResize: true,
        textureSize: 1024,
        enableTextureBrightness: false,
        textureBrightness: 1.0,
        enablePBR: true,
        metalness: 0.0,
        roughness: 1.0,
        enableSmoothShading: true,

      },
      transform: {
        position: [0, 0, 0],
        rotation: [0, 0, 0],
        scale: [1, 1, 1],
      },
    }

  const [isDracoCompression, setIsDracoCompression] = React.useState(
    currentOptimizerInfo.settings.enableDraco ?? true
  )
  const [isOptimizations, setIsOptimizations] = React.useState(
    currentOptimizerInfo.settings.enableOptimizations ?? true
  )
  const [isSmoothShading, setIsSmoothShading] = React.useState(
    currentOptimizerInfo.settings.enableSmoothShading ?? true
  )
  const [isTextureResize, setIsTextureResize] = React.useState(
    currentOptimizerInfo.settings.enableTextureResize ?? true
  )
  const [textureSize, setTextureSize] = React.useState(
    currentOptimizerInfo.settings.textureSize?.toString() || '512'
  )
  const [isTextureBrightness, setIsTextureBrightness] = React.useState(
    currentOptimizerInfo.settings.enableTextureBrightness ?? true
  )
  const [isSimplifyGeometry, setIsSimplifyGeometry] = React.useState(
    currentOptimizerInfo.settings.enableSimplify ?? true
  )
  const [isPbrMaterial, setIsPbrMaterial] = React.useState(
    currentOptimizerInfo.settings.enablePBR ?? true
  )
  const [textureBrightnessValue, setTextureBrightnessValue] = React.useState(
    currentOptimizerInfo.settings.textureBrightness ?? 1
  )
  const [simplifyGeometryRatio, setSimplifyGeometryRatio] = React.useState(
    currentOptimizerInfo.settings.simplifyRatio ?? 0.5
  )
  const [pbrMetal, setPbrMetal] = React.useState(
    currentOptimizerInfo.settings.metalness ?? 0.5
  )
  const [pbrRoughness, setPbrRoughness] = React.useState(
    currentOptimizerInfo.settings.roughness ?? 0.5
  )
  const [posX, setPosX] = React.useState(
    currentOptimizerInfo.transform.position[0] ?? 0
  )
  const [posY, setPosY] = React.useState(
    currentOptimizerInfo.transform.position[1] ?? 0
  )
  const [posZ, setPosZ] = React.useState(
    currentOptimizerInfo.transform.position[2] ?? 0
  )
  const [rotX, setRotX] = React.useState(currentOptimizerInfo.transform.rotation[0] ?? 0)
  const [rotY, setRotY] = React.useState(currentOptimizerInfo.transform.rotation[1] ?? 0)
  const [rotZ, setRotZ] = React.useState(currentOptimizerInfo.transform.rotation[2] ?? 0)
  const [scaledLocked, setScaleLocked] = React.useState(true)
  const [scaleX, setScaleX] = React.useState(currentOptimizerInfo.transform.scale[0] ?? 1)
  const [scaleY, setScaleY] = React.useState(currentOptimizerInfo.transform.scale[1] ?? 1)
  const [scaleZ, setScaleZ] = React.useState(currentOptimizerInfo.transform.scale[2] ?? 1)

  // We will keep optimizedGlbRawArray and optimizedGlb in sync
  const [optimizedGlbRawArray, setOptimizedGlbRawArray] = React.useState<Uint8Array | null>(null)
  const [optimizedGlb, setOptimizedGlb] = React.useState<GltfData | null>(null)
  const [isApplyQueued, setIsApplyQueued] = React.useState(false)
  const rawMeshUrl = assetGeneration
    ? getMeshUrl(assetGeneration?.AccountUuid, assetGenerationUuid)
    : null
  const prevOptimizedMeshUrl = assetGeneration
    ? getOptimizedMeshUrl(
      assetGeneration?.AccountUuid,
      assetGenerationUuid,
      (assetGeneration?.metadata as AssetGenerationMeshMetadata)?.optimizer?.timestampMs
    )
    : rawMeshUrl

  const prevOptimizedGlbBuffer = useArrayBufferFetch(prevOptimizedMeshUrl)
  const [prevOptimizedGlb, setPrevOptimizedGlb] = React.useState<GltfData | null>(null)
  React.useEffect(() => {
    if (!prevOptimizedGlbBuffer?.data) {
      return
    }
    (async () => {
      const newRawGlb = await loadGltfFromArrayBuffer(prevOptimizedGlbBuffer.data as ArrayBuffer)
      setPrevOptimizedGlb(newRawGlb)
    })()
  }, [prevOptimizedGlbBuffer?.data])

  // State and action for running and saving optimized 3d model
  const glbPostProcessing = useGlbPostProcessing(rawMeshUrl)
  const applyPostProcessing = async (
    glbPP: ReturnType<typeof useGlbPostProcessing>,
    settings: PostProcessingSettings,
    position: Vec3Tuple,
    rotation: Vec3Tuple,
    scale: Vec3Tuple
  ) => {
    const glb = await glbPP.apply(
      settings, position, rotation, scale
    )

    // Since we loaded glb, we know that glbPostProcessing returns a clean buffer
    // that only contains the Uint8Array
    setOptimizedGlbRawArray(glb)
    setOptimizedGlb(await loadGltfFromArrayBuffer(glb.buffer as ArrayBuffer))
    setIsApplyQueued(false)
  }

  const getSettings = (): PostProcessingSettings => ({
    enableDraco: isDracoCompression,
    enableOptimizations: isOptimizations,
    enableSmoothShading: isSmoothShading,
    enableTextureResize: isTextureResize,
    textureSize: parseAsTextureSizeOr(textureSize),
    enableTextureBrightness: isTextureBrightness,
    textureBrightness: textureBrightnessValue,
    enableSimplify: isSimplifyGeometry,
    simplifyRatio: simplifyGeometryRatio,
    enablePBR: isPbrMaterial,
    metalness: pbrMetal,
    roughness: pbrRoughness,
    enableZip: false,  // TODO(dat): make this configurable
    // NOTE(dat): pivot point is built into position, rotation, scale below
  })
  const getPosition = (): Vec3Tuple => ([posX, posY, posZ])
  const getRotation = (): Vec3Tuple => ([rotX, rotY, rotZ])
  const getScale = (): Vec3Tuple => ([scaleX, scaleY, scaleZ])
  const debounceApplyPostProcessing = useDebounce(React.useRef(applyPostProcessing), 1000)
  React.useEffect(() => {
    if (!glbPostProcessing.readyToApply) {
      return
    }
    setIsApplyQueued(true)
    debounceApplyPostProcessing(
      glbPostProcessing,
      getSettings(),
      getPosition(),
      getRotation(),
      getScale()
    )
  }, [
    isDracoCompression,
    isOptimizations,
    isSmoothShading,
    isTextureResize, textureSize,
    isTextureBrightness,
    textureBrightnessValue,
    isSimplifyGeometry,
    simplifyGeometryRatio,
    isPbrMaterial,
    pbrMetal,
    pbrRoughness,
    posX, posY, posZ,
    rotX, rotY, rotZ,
    scaleX, scaleY, scaleZ,
    rawMeshUrl, glbPostProcessing.readyToApply,
  ])

  const {getMeshUploadSignedUrl, updateAssetGenerationMetadata} = useActions(assetLabActions)
  const {uploadFileToS3} = useActions(assetConverterActions)
  const {uuid: accountUuid} = useCurrentAccount()
  const [saveStatusIndicator, setSaveStatusIndicator] = React.useState<string | null>(null)
  const saveStatusUpdateTimeout = React.useRef<ReturnType<typeof setTimeout> | null>(null)
  const saveOptimized3DModel = async () => {
    if (!optimizedGlb) {
      // We should not get here since the button is disabled
      return
    }

    // Similar to the saving part in useRequestModelPostProcessing()
    setSaveStatusIndicator(t('asset_lab.model_optimizer.saving'))
    // We don't care if the signed URL is `already_exists` or `created`
    const signedUrlRes = await getMeshUploadSignedUrl(accountUuid, assetGenerationUuid, true)
    const processedGlbFile = makeGlbFile(optimizedGlbRawArray)
    await uploadFileToS3(processedGlbFile, signedUrlRes.signedUrl)
    await updateAssetGenerationMetadata(assetGenerationUuid, makeOptimizerMetadata(
      getSettings(),
      getPosition(),
      getRotation(),
      getScale()
    ))

    // We are done. Add some status indicator with timeout cleanup
    setSaveStatusIndicator(t('asset_lab.model_optimizer.saved'))
    saveStatusUpdateTimeout.current = setTimeout(() => {
      setSaveStatusIndicator(null)
    }, 3000)
  }
  // Clean up saveStatusTimeout if unmounted
  React.useEffect(() => () => {
    if (saveStatusUpdateTimeout.current) {
      clearTimeout(saveStatusUpdateTimeout.current)
      saveStatusUpdateTimeout.current = null
    }
  }, [])

  const resetToDefault = () => {
    setIsDracoCompression(currentOptimizerInfo.settings.enableDraco ?? true)
    setIsOptimizations(currentOptimizerInfo.settings.enableOptimizations ?? true)
    setIsSmoothShading(currentOptimizerInfo.settings.enableSmoothShading ?? true)
    setIsTextureResize(currentOptimizerInfo.settings.enableTextureResize ?? true)
    setTextureSize(currentOptimizerInfo.settings.textureSize?.toString() || '512')
    setIsTextureBrightness(currentOptimizerInfo.settings.enableTextureBrightness ?? true)
    setTextureBrightnessValue(currentOptimizerInfo.settings.textureBrightness ?? 1)
    setIsSimplifyGeometry(currentOptimizerInfo.settings.enableSimplify ?? true)
    setSimplifyGeometryRatio(currentOptimizerInfo.settings.simplifyRatio ?? 0.5)
    setIsPbrMaterial(currentOptimizerInfo.settings.enablePBR ?? true)
    setPbrMetal(currentOptimizerInfo.settings.metalness ?? 0.5)
    setPbrRoughness(currentOptimizerInfo.settings.roughness ?? 0.5)
    setPosX(currentOptimizerInfo.transform.position[0] ?? 0)
    setPosY(currentOptimizerInfo.transform.position[1] ?? 0)
    setPosZ(currentOptimizerInfo.transform.position[2] ?? 0)
    setRotX(currentOptimizerInfo.transform.rotation[0] ?? 0)
    setRotY(currentOptimizerInfo.transform.rotation[1] ?? 0)
    setRotZ(currentOptimizerInfo.transform.rotation[2] ?? 0)
    setScaleLocked(true)
    setScaleX(currentOptimizerInfo.transform.scale[0] ?? 1)
    setScaleY(currentOptimizerInfo.transform.scale[1] ?? 1)
    setScaleZ(currentOptimizerInfo.transform.scale[2] ?? 1)
  }

  const assetLabStyles = useAssetLabStyles()

  const [pivotToPos, setPivotToPos] = React.useState<Record<PivotPointOptions, Vec3Tuple>>({
    center: [0, 0, 0],
    right: [0, 0, 0],
    left: [0, 0, 0],
    top: [0, 0, 0],
    bottom: [0, 0, 0],
    front: [0, 0, 0],
    back: [0, 0, 0],
  })
  React.useEffect(() => {
    const asyncFunc = async () => {
      if (!glbPostProcessing?.gltf) {
        return
      }
      const positionArray = await getDocLastPositionArray(glbPostProcessing?.gltf)
      if (!positionArray) {
        return
      }
      const boundingBox = getBoundingBox(positionArray)
      setPivotToPos({
        center: getPivot('center', boundingBox),
        right: getPivot('right', boundingBox),
        left: getPivot('left', boundingBox),
        top: getPivot('top', boundingBox),
        bottom: getPivot('bottom', boundingBox),
        front: getPivot('front', boundingBox),
        back: getPivot('back', boundingBox),
      })
    }
    asyncFunc()
  }, [
    glbPostProcessing?.gltf,
  ])
  const makePivotPointButton = (point: PivotPointOptions) => (
    <AssetLabButton
      onClick={() => {
        const pos = pivotToPos[point]
        setPosX(pos[0])
        setPosY(pos[1])
        setPosZ(pos[2])
      }}
      className={assetLabStyles.tertiaryButton}
    >
      {t(`asset_lab.model_optimizer.${point}`)}
    </AssetLabButton>
  )

  const setAllScale = (val: number) => {
    setScaleX(val)
    setScaleY(val)
    setScaleZ(val)
  }
  const setScaleXWithLock = scaledLocked ? setAllScale : setScaleX
  const setScaleYWithLock = scaledLocked ? setAllScale : setScaleY
  const setScaleZWithLock = scaledLocked ? setAllScale : setScaleZ
  return (
    <div className={classes.detailViewContainer}>
      <div className={classes.row}>
        <button
          type='button'
          className={combine('style-reset', classes.libraryButton)}
          onClick={() => {
            assetLabCtx.setState({mode: prevMode, prevMode: null})
          }}
        >
          <Icon stroke='chevronLeft' />
          {t('asset_lab.model_optimizer.back')}
        </button>
        <div className={classes.title}>{t('asset_lab.model_optimizer.title')}</div>
      </div>
      <div className={classes.assetInfoContainer}>
        <div className={classes.optimizerOptionsContainer}>
          <div className={classes.optimizerOptions}>
            <h4 className={classes.optionGroupLabel}>{t('asset_lab.model_optimizer.options')}</h4>
            <StandardCheckboxField
              id='optimizer-draco-compression'
              label={t('asset_lab.model_optimizer.draco_compression')}
              checked={isDracoCompression}
              onChange={e => setIsDracoCompression(e.target.checked)}
            />
            <StandardCheckboxField
              id='optimizer-optimizations'
              label={t('asset_lab.model_optimizer.optimizations')}
              checked={isOptimizations}
              onChange={e => setIsOptimizations(e.target.checked)}
            />
            <StandardCheckboxField
              id='optimizer-smooth-shading'
              label={t('asset_lab.model_optimizer.smooth_shading')}
              checked={isSmoothShading}
              onChange={e => setIsSmoothShading(e.target.checked)}
            />
            <StandardCheckboxField
              id='optimizer-texture-resize'
              label={t('asset_lab.model_optimizer.texture_resize')}
              checked={isTextureResize}
              onChange={e => setIsTextureResize(e.target.checked)}
            />
            {isTextureResize && (
              <div>
                <StandardDropdownField
                  id='mode-optimizer-texture-resize'
                  label={t('asset_lab.model_optimizer.texture_resize_size')}
                  height='auto'
                  options={textureResizeOptions}
                  value={textureSize}
                  onChange={value => setTextureSize(value)}
                />
              </div>
            )}
            <StandardCheckboxField
              id='optimizer-texture-brightness'
              label={t('asset_lab.model_optimizer.texture_brightness')}
              checked={isTextureBrightness}
              onChange={e => setIsTextureBrightness(e.target.checked)}
            />
            {isTextureBrightness && (
              <div className={classes.inputSliderCombo}>
                <span>{t('asset_lab.model_optimizer.value')}</span>
                <RangeSliderInput
                  id='model-optimizer-texture-value-slider'
                  value={textureBrightnessValue}
                  min={0}
                  step={0.1}
                  max={1}
                  onChange={value => setTextureBrightnessValue(value)}
                  color={gray4}
                />
                <span>{textureBrightnessValue.toFixed(2)}</span>
              </div>
            )}
            <StandardCheckboxField
              id='optimizer-simplify-geometry'
              label={t('asset_lab.model_optimizer.simplify_geometry')}
              checked={isSimplifyGeometry}
              onChange={e => setIsSimplifyGeometry(e.target.checked)}
            />
            {isSimplifyGeometry && (
              <div className={classes.inputSliderCombo}>
                <span>{t('asset_lab.model_optimizer.ratio')}</span>
                <RangeSliderInput
                  id='model-optimizer-texture-value-slider'
                  value={simplifyGeometryRatio}
                  min={0}
                  step={0.1}
                  max={1}
                  onChange={value => setSimplifyGeometryRatio(value)}
                  color={gray4}
                />
                <span>{simplifyGeometryRatio.toFixed(2)}</span>
              </div>
            )}
            <StandardCheckboxField
              id='optimizer-pbr-material'
              label={t('asset_lab.model_optimizer.pbr_material')}
              checked={isPbrMaterial}
              onChange={e => setIsPbrMaterial(e.target.checked)}
            />
            {isPbrMaterial && (
              <>
                <div className={classes.inputSliderCombo}>
                  <span>{t('asset_lab.model_optimizer.metal')}</span>
                  <RangeSliderInput
                    id='model-optimizer-texture-value-slider'
                    value={pbrMetal}
                    min={0}
                    step={0.1}
                    max={1}
                    onChange={value => setPbrMetal(value)}
                    color={gray4}
                  />
                  <span>{pbrMetal.toFixed(2)}</span>
                </div>
                <div className={classes.inputSliderCombo}>
                  <span>{t('asset_lab.model_optimizer.roughness')}</span>
                  <RangeSliderInput
                    id='model-optimizer-texture-value-slider'
                    value={pbrRoughness}
                    min={0}
                    step={0.1}
                    max={1}
                    onChange={value => setPbrRoughness(value)}
                    color={gray4}
                  />
                  <span>{pbrRoughness.toFixed(2)}</span>
                </div>
              </>
            )}
          </div>
          <div className={classes.optimizerOptions}>
            <h4 className={classes.optionGroupLabel}>
              {t('asset_lab.model_optimizer.transforms')}
            </h4>
            <div className={classes.transformGrid}>
              <div>
                <div>{t('asset_lab.model_optimizer.position')}</div>
                <div className={classes.transformInput}>
                  <SliderInputAxisField
                    id='position-x'
                    label='X'
                    step={0.1}
                    value={posX}
                    onChange={val => setPosX(val)}
                    onDrag={diff => setPosX(posX + diff)}
                  />
                  <SliderInputAxisField
                    id='position-y'
                    label='Y'
                    step={0.1}
                    value={posY}
                    onChange={val => setPosY(val)}
                    onDrag={diff => setPosY(posY + diff)}
                  />
                  <SliderInputAxisField
                    id='position-z'
                    label='Z'
                    step={0.1}
                    value={posZ}
                    onChange={val => setPosZ(val)}
                    onDrag={diff => setPosZ(posZ + diff)}
                  />
                </div>
              </div>
              <div>
                <div>{t('asset_lab.model_optimizer.rotation')}</div>
                <div className={classes.transformInput}>
                  <SliderInputAxisField
                    id='rotation-x'
                    label='X'
                    step={1}
                    value={rotX}
                    onChange={val => setRotX(val)}
                    onDrag={diff => setRotX(rotX + diff)}
                    normalizer={normalizeDegrees}
                  />
                  <SliderInputAxisField
                    id='rotation-y'
                    label='Y'
                    step={1}
                    value={rotY}
                    onChange={val => setRotY(val)}
                    onDrag={diff => setRotY(rotY + diff)}
                    normalizer={normalizeDegrees}
                  />
                  <SliderInputAxisField
                    id='rotation-z'
                    label='Z'
                    step={1}
                    value={rotZ}
                    onChange={val => setRotZ(val)}
                    onDrag={diff => setRotZ(rotZ + diff)}
                    normalizer={normalizeDegrees}
                  />
                </div>
              </div>
              <div>
                <div className={classes.transformButtonRow}>
                  {t('asset_lab.model_optimizer.scale')}
                  <IconButton
                    text={t('transform_configurator.button.lock_scale')}
                    onClick={() => {
                      setScaleLocked(!scaledLocked)
                      if (!scaledLocked) {
                        setAllScale(scaleX)
                      }
                    }}
                    stroke={scaledLocked ? 'chain' : 'chainBroken'}
                  />
                </div>
                <div className={classes.transformInput}>
                  <SliderInputAxisField
                    id='scale-x'
                    label='X'
                    min={0}
                    step={0.1}
                    value={scaleX}
                    onChange={val => setScaleXWithLock(val)}
                    onDrag={diff => setScaleXWithLock(scaleX + diff)}
                  />
                  <SliderInputAxisField
                    id='scale-y'
                    label='Y'
                    min={0}
                    step={0.1}
                    value={scaleY}
                    onChange={val => setScaleYWithLock(val)}
                    onDrag={diff => setScaleYWithLock(scaleY + diff)}
                  />
                  <SliderInputAxisField
                    id='scale-z'
                    label='Z'
                    min={0}
                    step={0.1}
                    value={scaleZ}
                    onChange={val => setScaleZWithLock(val)}
                    onDrag={diff => setScaleZWithLock(scaleZ + diff)}
                  />
                </div>
              </div>
              <div>
                <div>{t('asset_lab.model_optimizer.pivot_point')}</div>
                <div className={classes.pivotButtonsContainer}>
                  <div className={classes.pivotButtons}>
                    {makePivotPointButton('front')}
                    {makePivotPointButton('top')}
                  </div>
                  <div className={classes.pivotButtons}>
                    {makePivotPointButton('left')}
                    {makePivotPointButton('center')}
                    {makePivotPointButton('right')}
                  </div>
                  <div className={classes.pivotButtons}>
                    {makePivotPointButton('back')}
                    {makePivotPointButton('bottom')}
                  </div>
                </div>
              </div>
            </div>
          </div>
          <div>{
            isApplyQueued
              ? t('asset_lab.model_optimizer.processing')
              : (
                <AssetLabButton onClick={resetToDefault}>
                  {t('asset_lab.model_optimizer.reset_to_default')}
                </AssetLabButton>
              )
          }
          </div>
        </div>
        <div className={classes.modelContainer}>
          <div className={classes.beforeAfterBox}>
            <BeforeAfterBox
              before={prevOptimizedGlb?.metadata}
              after={optimizedGlb?.metadata}
            />
          </div>
          <StudioModelPreviewWithLoader
            src={optimizedGlb?.model}
            srcMetadata={optimizedGlb?.metadata}
            srcExt='glb'
            cameraSticky={optimizedGlb !== null}
            envName={BASIC}
          />
        </div>
      </div>
      <div className={classes.row}>
        <div className={classes.buttonBar}>
          &nbsp;
        </div>
        <div className={combine(classes.row, classes.buttonBar)}>
          {saveStatusIndicator}
          <DetailViewButton
            onClick={saveOptimized3DModel}
            disabled={!optimizedGlb?.model}
            primary
            iconStroke='wrench'
            text={t('asset_lab.model_optimizer.save_optimized_3d_model')}
          />
        </div>
      </div>
    </div>
  )
}

export {Studio3dModelOptimizer}
