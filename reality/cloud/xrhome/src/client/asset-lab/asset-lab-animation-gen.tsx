import React from 'react'
import {useTranslation} from 'react-i18next'
import {BASIC} from '@ecs/shared/environment-maps'
import type {Object3D} from 'three'

import {useAssetLabStyles} from './asset-lab-styles'
import {AssetLabAnimModelDropdown} from './widgets/asset-lab-model-dropdown'
import {DEFAULT_ANIMATION_CLIP, type ToAnimModelIds} from './constants'
import {useAssetLabStateContext} from './asset-lab-context'
import {Icon} from '../ui/components/icon'
import {AssetRequestIntervalChecker} from './widgets/asset-request-interval-checker'
import {AssetGenProgressIndicator} from './widgets/asset-gen-progress-indicator'
import assetLabActions from './asset-lab-actions'
import useActions from '../common/use-actions'
import {useCreditQuery} from '../billing/use-credit-query'
import useCurrentAccount from '../common/use-current-account'
import type {MeshToAnimation} from '../../shared/genai/types/mesh-to-animation'
import StudioModelPreviewWithLoader from '../studio/studio-model-preview-with-loader'
import {getAssetGenUrl} from './urls'
import {useRequestResult} from './hooks/use-request-result'
import {SolidMessageBanner} from './widgets/solid-banner'
import {PlaceholderBox} from './widgets/square'
import {
  AssetLabButton, AssetLabButtonLink, AssetLabButtonWithCost,
} from './widgets/asset-lab-button'
import {useImportIntoProject} from './hooks/use-import-into-project'
import {makeUuidInput, makeFileInput, urlToUuid} from './generate-request'
import {useMeshGeneration} from './hooks/use-mesh-generation'
import {AssetLabInputError} from './widgets/asset-lab-input-error'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {
  useAnimationControls, useAnimationOptions, useAnimationsNoRaf,
} from '../studio/hooks/use-animations'
import {combine} from '../common/styles'
import {UseFrame} from '../studio/use-frame'
import {useChangeEffect} from '../hooks/use-change-effect'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {useMeshParentRequest} from './hooks/use-parent-request'
import {AssetGenImageDrop} from './widgets/asset-gen-image-drop'
import type {AssetInput} from '../../shared/genai/types/base'
import {isOptimizedMeshUrl} from '../../shared/genai/constants'
import AssetLabLibraryPicker from './widgets/asset-lab-library-picker'
import {useSelector} from '../hooks'

const AssetLabAnimationGen = () => {
  const {t} = useTranslation('asset-lab')
  // Load up state from the context to prepopulate our form with previous values. Used for remix.
  const assetLabCtx = useAssetLabStateContext()
  const {assetRequestUuid, animInputUuid, requestLineage} = assetLabCtx.state
  const {hasResult, resultMeshUrl, isGenerating, assetRequest} = useRequestResult(assetRequestUuid)
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)

  const {importFromUrl, hasApp} = useImportIntoProject()
  const [isImporting, setIsImporting] = React.useState(false)

  // --- Form values that user can then override
  const [model, setModel] = React.useState<ToAnimModelIds>(
    assetRequest?.input?.modelId || 'meshy-ai'
  )
  const {generateAssets} = useActions(assetLabActions)
  const currentAccount = useCurrentAccount()
  const {refetch: creditRefetch, status: creditStatus} = useCreditQuery()

  const [isSubmitting, setIsSubmitting] = React.useState(false)
  const {meshGeneration: meshGenInput} = useMeshGeneration(animInputUuid)
  const parentRequest = useMeshParentRequest(animInputUuid)
  const [uuidOrFileInput, setUuidOrFileInput] = React.useState<string | File>(
    animInputUuid || urlToUuid(assetRequest?.input?.meshUrl) || ''
  )

  const [isSelecting, setIsSelecting] = React.useState(false)

  const handleGenerate = async () => {
    setIsSubmitting(true)
    assetLabCtx.setState({userInputError: null})
    let meshInput: AssetInput
    if (uuidOrFileInput instanceof File) {
      meshInput = makeFileInput(uuidOrFileInput)
    } else if (isOptimizedMeshUrl(getAssetGenUrl(meshGenInput))) {
      meshInput = makeUuidInput(`${animInputUuid}/optimized`)
    } else {
      meshInput = makeUuidInput(animInputUuid)
    }

    const request: MeshToAnimation = {
      modelId: model,
      type: 'MESH_TO_ANIMATION',
      mesh: meshInput,
      numRequests: 1,
      ParentRequestUuid: parentRequest?.uuid,
    }

    try {
      const {assetRequest: {uuid}} = await generateAssets(currentAccount.uuid, request)
      assetLabCtx.setState({assetRequestUuid: uuid, assetGenerationUuid: undefined})
    } catch (error) {
      if (error.status === 400) {
        // If the error is a user input error, we want to show it in the UI.
        assetLabCtx.setState({
          userInputError: error.message || t('asset_lab.anim_gen.invalid_input'),
        })
      }
      // rethrow for logging
      throw error
    } finally {
      setIsSubmitting(false)
    }
    creditRefetch()
  }

  const classes = useAssetLabStyles()

  const [resultGltf, setResultGltf] = React.useState<Object3D>(null)
  const {mixer, actions, clips} = useAnimationsNoRaf(resultGltf?.animations || [], resultGltf)
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
    <div className={classes.assetGenSplit}>
      <div className={combine(classes.genForm, classes.flex4)}>
        <AssetLabAnimModelDropdown
          label={t('asset_lab.anim_gen.rigging_model')}
          model={model}
          setModel={v => setModel(v as ToAnimModelIds)}
        />
        <SolidMessageBanner
          type='info'
          message={t('asset_lab.anim_gen.only_bipedal_humanoid_t_pose')}
        />
        <div>
          <StandardFieldLabel
            label={t('asset_lab.anim_gen.3d_character_model')}
            mutedColor
            starred
          />
          <div className={classes.modelContainer}>
            {isSelecting
              ? <AssetLabLibraryPicker
                  onSelect={(uuid) => {
                    setUuidOrFileInput(uuid)
                    assetLabCtx.setState({animInputUuid: uuid})
                    assetLabCtx.setRequestLineageFromInput(assetGenerations[uuid].RequestUuid)
                  }}
                  onClose={() => setIsSelecting(false)}
                  filters={['MESH']}
              />
              : <AssetGenImageDrop
                  size={360}
                  onDrop={(file) => {
                    setUuidOrFileInput(file)
                    assetLabCtx.setState({animInputUuid: undefined})
                  }}
                  onClear={() => {
                    setUuidOrFileInput(null)
                    assetLabCtx.setState({animInputUuid: undefined})
                  }}
                  uuidOrFile={uuidOrFileInput}
                  onAddFromLibrary={() => setIsSelecting(true)}
              />
            }
          </div>
        </div>
        <AssetLabInputError />
        <div className={classes.generateRow}>
          <div className={classes.generateRowLeft}>
            <AssetLabButton onClick={() => {
              assetLabCtx.setState({
                mode: 'model',
                assetRequestUuid: requestLineage.model,
              })
            }}
            >
              {t('asset_lab.anim_gen.back_to_model')}
            </AssetLabButton>
          </div>
          <div className={classes.generateButton}>
            <AssetLabButtonWithCost
              a8='click;studio;generate-animation-button'
              onClick={handleGenerate}
              disabled={creditStatus !== 'success' || isGenerating || isSubmitting}
              active
              label={t('asset_lab.anim_gen.rig_and_animate')}
              type='MESH_TO_ANIMATION'
              modelId={model}
            />
          </div>
        </div>
      </div>
      <div className={combine(classes.genResults, classes.flex6)}>
        {assetRequestUuid && <AssetRequestIntervalChecker id={assetRequestUuid} />}
        {hasResult && (
          <div className={classes.modelContainerOutput}>
            <StudioModelPreviewWithLoader
              src={resultMeshUrl}
              srcExt='glb'
              envName={BASIC}
              onGltfLoad={setResultGltf}
            >
              <UseFrame callback={animateFrame} />
            </StudioModelPreviewWithLoader>
          </div>
        )}
        {!hasResult && (
          <PlaceholderBox width='100%' height='60vh'>
            <AssetGenProgressIndicator id={assetRequestUuid} />
          </PlaceholderBox>
        )}
        <div className={classes.buttonBar}>
          {clips?.length > 0 && (
            <div className={combine(classes.generateRowLeft, classes.animationClipControls)}>
              <FloatingPanelButton
                a8='click;animation-gen;play-pause-animation'
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
          <AssetLabButtonLink
            href={resultMeshUrl}
            target='_blank'
            rel='noopener noreferrer'
          >
            <Icon inline stroke='download' />&nbsp;{t('asset_lab.detail_view.download_asset')}
          </AssetLabButtonLink>
          {hasApp && (
            <AssetLabButton
              loading={isImporting}
              onClick={async () => {
                setIsImporting(true)
                await importFromUrl(
                  resultMeshUrl, `${animInputUuid}.glb`, 'model/gltf-binary'
                )
                setIsImporting(false)
              }}
              disabled={!hasResult}
            >
              <Icon stroke='downloadFile' inline />
              {t('asset_lab.model_gen.import_into_project')}
            </AssetLabButton>
          )}
        </div>
      </div>
    </div>
  )
}

export {AssetLabAnimationGen}
