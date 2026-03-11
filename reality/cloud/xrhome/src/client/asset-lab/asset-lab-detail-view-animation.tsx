import React from 'react'
import {BASIC} from '@ecs/shared/environment-maps'
import type {Object3D} from 'three'

import {Icon} from '../ui/components/icon'
import {backToLibraryClear, useAssetLabStateContext} from './asset-lab-context'
import {useSelector} from '../hooks'
import {getAssetGenUrl} from './urls'
import {DetailViewLeftSnackbar} from './widgets/asset-lab-detail-view-snackbar'
import StudioModelPreviewWithLoader from '../studio/studio-model-preview-with-loader'
import {combine} from '../common/styles'
import {createThemedStyles} from '../ui/theme'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {
  useAnimationControls, useAnimationOptions, useAnimationsNoRaf,
} from '../studio/hooks/use-animations'
import {UseFrame} from '../studio/use-frame'
import {useAssetLabStyles} from './asset-lab-styles'
import {useChangeEffect} from '../hooks/use-change-effect'
import {DEFAULT_ANIMATION_CLIP} from './constants'
import {
  AssetLabDownloadButton, AssetLabImportButton, AssetLabLibraryButton,
} from './widgets/detail-view-button-widgets'

const useStyles = createThemedStyles(theme => ({
  imagePromptContainer: {
    'minWidth': '150px',
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
    borderRadius: '0.5rem',
    position: 'relative',
    height: '100%',
  },
  singlePromptContainer: {
    'position': 'absolute',
    'bottom': '10px',
    'left': '10px',
    'height': '20vh',
    'aspectRatio': '1/1',
    'backgroundColor': theme.sfcBackgroundDefault,
    'border': `1px solid ${theme.studioAssetBorder}`,
    'borderRadius': '0.5rem',
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
  floatingTopRight: {
    position: 'absolute',
    top: '0.5rem',
    right: '0.5rem',
    zIndex: 100,
  },
}))

const AssetLabDetailViewAnimation = () => {
  const classes = useStyles()
  const assetLabClasses = useAssetLabStyles()

  const assetLabCtx = useAssetLabStateContext()
  const {assetGenerationUuid} = assetLabCtx.state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration?.RequestUuid])
  // @ts-expect-error TODO(christoph): Clean up
  const genUser = useSelector(s => s.team.roles.find(e => e.uuid === assetGeneration.UserUuid))
  const unriggedMeshUrl = assetRequest?.input?.meshUrl
  // NOTE(dat): An asset might have been optimized so it might be available at a slightly different
  // URL.
  const assetGenFilePath = getAssetGenUrl(assetGeneration)

  // Allow the mesh to have its animation controlled
  const [assetGenGltf, setAssetGenGltf] = React.useState<Object3D>(null)
  const {mixer, actions, clips} = useAnimationsNoRaf(assetGenGltf?.animations || [], assetGenGltf)
  const {
    animationClipToPlay, isAnimationPaused, setIsAnimationPaused, setAnimationClip, animateFrame,
  } = useAnimationControls(mixer, actions)
  const {animationClipOptions} = useAnimationOptions(clips)
  // Make sure we start the animation with the default clip when it's first available
  useChangeEffect(([prev]) => {
    if ((!prev || prev.length === 0) && (clips?.length > 0)) {
      setAnimationClip(DEFAULT_ANIMATION_CLIP)
      setIsAnimationPaused(false)
    }
  }, [clips])
  return (
    <>
      <div className={classes.row}>
        <AssetLabLibraryButton onClick={() => {
          backToLibraryClear(assetLabCtx, {scrollToUuid: assetGenerationUuid})
        }}
        />
      </div>
      <div className={classes.assetInfoContainer}>
        <div className={classes.modelContainer}>
          <StudioModelPreviewWithLoader
            src={assetGenFilePath}
            srcExt='glb'
            envName={BASIC}
            onGltfLoad={setAssetGenGltf}
          >
            <UseFrame callback={animateFrame} />
          </StudioModelPreviewWithLoader>
          {clips?.length > 0 && (
            <div className={combine(
              classes.floatingTopRight, assetLabClasses.animationClipControls
            )}
            >
              <FloatingPanelButton
                a8='click;asset-lab-detail-view;play-pause-animation'
                height='small'
                onClick={() => {
                  setIsAnimationPaused(!isAnimationPaused)
                }}
              >
                <Icon stroke={isAnimationPaused ? 'play' : 'pause'} inline />
              </FloatingPanelButton>

              <StandardDropdownField
                id='anim-gen-model-animation-clips'
                label=''
                height='auto'
                options={animationClipOptions}
                value={animationClipToPlay}
                onChange={value => setAnimationClip(value)}
              />
            </div>
          )}
        </div>
        <div className={classes.singlePromptContainer}>
          <StudioModelPreviewWithLoader
            src={unriggedMeshUrl as string}
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
  AssetLabDetailViewAnimation,
}
