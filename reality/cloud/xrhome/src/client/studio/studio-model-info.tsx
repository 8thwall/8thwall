import React, {useState} from 'react'
import {useTranslation} from 'react-i18next'

import {AnimationMixer, AnimationClip, Clock, Object3D} from 'three'

import {BASIC, ENV_MAP_PRESETS} from '@ecs/shared/environment-maps'

import StudioModelPreviewWithLoader from './studio-model-preview-with-loader'
import {RowBooleanField, RowSelectField} from './configuration/row-fields'
import {useAppPreviewStyles} from '../editor/app-preview/app-preview-utils'
import {IconButton} from '../ui/components/icon-button'
import {Loader} from '../ui/components/loader'
import {formatBytesToText} from '../../shared/asset-size-limits'
import {AssetInfo} from './configuration/asset-info'
import {createThemedStyles} from '../ui/theme'
import {StaticBanner} from '../ui/components/banner'
import {useStudioModelPreviewContext} from './asset-previews/studio-model-preview-context'
import type {EditorFileLocation} from '../editor/editor-file-location'
import {editorFontSize} from '../static/styles/settings'

const AssetMeshConfigurator = React.lazy(() => import('./configuration/asset-mesh-configurator'))

const useStyles = createThemedStyles(theme => ({
  modelGrid: {
    display: 'flex',
    height: '40vh',
    position: 'relative',
  },
  assetModelInfo: {
    padding: '1em 1em',
    borderTop: theme.studioSectionBorder,
    borderBottom: theme.studioSectionBorder,
    fontSize: editorFontSize,
  },
  assetControlContainer: {
    flex: '1 1 0',
    overflowY: 'auto',
  },
  hidden: {
    display: 'none',
  },
  loader: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: '100%',
    width: '100%',
    position: 'absolute',
    backgroundColor: `${theme.sfcBackgroundDefault}bb`,
  },
  warningContainer: {
    position: 'absolute',
    width: '100%',
    padding: '1em',
  },
}))

const roundedCountText = (count: number): string => {
  if (!count && count !== 0) {
    return null
  }

  const k = count / 1000
  const m = k / 1000
  const b = m / 1000

  if (b > 1) {
    return `${b.toFixed(2)}b`
  } else if (m > 1) {
    return `${m.toFixed(2)}m`
  } else if (k > 1) {
    return `${k.toFixed(2)}k`
  } else {
    return `${count}`
  }
}

interface IStudioModelInfo {
  modelUrl: string
  defaultFileSize: number
  previewScene: Object3D
  mixer: AnimationMixer
  selectedAssetLocation: EditorFileLocation
}

const StudioModelInfo: React.FC<IStudioModelInfo> = ({
  modelUrl, defaultFileSize, previewScene, mixer, selectedAssetLocation,
}) => {
  const classes = useStyles()
  const appPreviewStyles = useAppPreviewStyles()

  const {
    fileSize, modelInfo, showGlbComputingLoader, showSimplifyMeshWarning, loadingFileSize,
    loadingTriCount, setShowSimplifyMeshWarning,
  } = useStudioModelPreviewContext()

  const [wireframe, setWireframe] = useState(false)

  const {t} = useTranslation(['cloud-studio-pages', 'cloud-editor-pages'])

  const envOptions = [{
    value: 'none',
    content: t('asset_configurator.studio_model_info.env_map.none'),
  },
  ...Object.entries(ENV_MAP_PRESETS).map(([key, value]) => ({
    value: key,
    content: t(value),
  }))]

  // envName defaults to 'basic'
  const [envName, setEnvName] = React.useState<string>(BASIC)

  const {animations} = previewScene
  const clock = new Clock()

  const animOptions = [{
    value: '',
    content: t('asset_configurator.studio_model_info.animation_clip.none'),
  },
  ...animations.map(clip => ({value: clip.name, content: clip.name}))]

  const hasAnimations = animations.length > 0
  const defaultAnimName = animOptions[0].value

  const [animName, setAnimName] = React.useState<string>(defaultAnimName)
  const [pauseAnimation, setPauseAnimation] = React.useState<boolean>(true)
  const animationTime = React.useRef(0)

  let cancelAnimation: number | null = null

  React.useEffect(() => {
    const action = mixer.clipAction(AnimationClip.findByName(animations, animName))
    if (hasAnimations && animName !== '' && action) {
      action.play()
      action.time = animationTime.current
      mixer.update(0)

      const animate = () => {
        if (!pauseAnimation) {
          cancelAnimation = requestAnimationFrame(animate)
          const deltaTime = clock.getDelta()
          mixer.update(deltaTime)
          animationTime.current = action.time
        }
      }

      animate()
    }
    return () => {
      action?.stop()
      mixer.update(0)
      if (cancelAnimation !== null) {
        cancelAnimationFrame(cancelAnimation)
      }
    }
  }, [animName, hasAnimations, pauseAnimation])

  const handleAnimPause = () => {
    setPauseAnimation(!pauseAnimation)
  }

  const handleAnimSelect = (newAnimName: string) => {
    if (newAnimName === '') {
      setPauseAnimation(true)
    } else {
      animationTime.current = 0
    }
    setAnimName(newAnimName)
  }

  const animPauseButton = (
    <div className={appPreviewStyles.actionButton}>
      <IconButton
        stroke={pauseAnimation ? 'play' : 'pause'}
        onClick={handleAnimPause}
        text={t('asset_configurator.studio_model_info.animation_clip.play_button')}
        disabled={!animName}
      />
    </div>
  )

  const toggleWireframe = (isChecked: boolean) => {
    setWireframe(isChecked)
  }

  const handleEnvMapSelect = (newEnvName: string) => setEnvName(newEnvName)

  const animationSelection = hasAnimations
    ? (
      <RowSelectField
        id='model-anim'
        label={t('asset_configurator.studio_model_info.animation_clip.title')}
        options={animOptions}
        onChange={handleAnimSelect}
        value={animName}
        leftContent={animPauseButton}
        disabled={showGlbComputingLoader}
      />
    )
    : null

  const assetInfoChild = (
    <AssetInfo metadata={{
      size: {
        label: t('editor_page.asset_preview.metadata.size', {ns: 'cloud-editor-pages'}),
        value: formatBytesToText(fileSize || defaultFileSize),
        loading: loadingFileSize,
      },
      tris: {
        label: t('editor_page.asset_preview.metadata.tris', {ns: 'cloud-editor-pages'}),
        value: roundedCountText(modelInfo?.triangles),
        loading: loadingTriCount,
      },
      compression: {
        label:
          t('asset_configurator.studio_model_info.meta_data.compression',
            {ns: 'cloud-studio-pages'}),
        value:
          t('asset_configurator.studio_model_info.meta_data.compression_type.draco'),
      },
    }}
    />
  )

  return (
    <>
      <div className={classes.modelGrid}>
        <StudioModelPreviewWithLoader
          src={previewScene}
          wireframe={wireframe}
          envName={envName}
        />
        {showGlbComputingLoader &&
          <div className={classes.loader}>
            <Loader inline centered />
          </div>
        }
        {showSimplifyMeshWarning &&
          <div className={classes.warningContainer}>
            <StaticBanner
              type='warning'
              onClose={() => setShowSimplifyMeshWarning(false)}
              autoClose={5000}
            >
              <span>
                {t(
                  'asset_configurator.studio_model_info.simplify_mesh_warning',
                  {ns: 'cloud-studio-pages'}
                )}
              </span>
            </StaticBanner>
          </div>
        }
      </div>
      <div className={classes.assetControlContainer}>
        <div className={classes.assetModelInfo}>
          <RowBooleanField
            id='model-wireframe'
            label={t('asset_configurator.studio_model_info.wireframe', {ns: 'cloud-studio-pages'})}
            checked={wireframe}
            onChange={e => toggleWireframe(e.target.checked)}
            disabled={showGlbComputingLoader}
            checkBoxAlign='right'
          />
          {assetInfoChild}
          <RowSelectField
            id='model-env'
            label={t('asset_configurator.studio_model_info.env_map')}
            options={envOptions}
            onChange={handleEnvMapSelect}
            value={envName}
            disabled={showGlbComputingLoader}
          />
          {animationSelection}
        </div>
        <React.Suspense fallback={null}>
          <AssetMeshConfigurator
            previewScene={previewScene}
            selectedAssetLocation={selectedAssetLocation}
            modelUrl={modelUrl}
            mixer={mixer}
          />
        </React.Suspense>
      </div>
    </>
  )
}

export default StudioModelInfo
