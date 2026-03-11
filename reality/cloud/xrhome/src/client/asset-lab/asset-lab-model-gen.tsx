import React from 'react'
import {useTranslation} from 'react-i18next'
import {BASIC} from '@ecs/shared/environment-maps'

import {useAssetLabStyles} from './asset-lab-styles'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {RangeSliderInput} from '../ui/components/range-slider-input'
import {AssetLab3dModelDropdown} from './widgets/asset-lab-model-dropdown'
import {
  MeshModelToMulti, WorkflowToParameters, type ToMeshModelIds,
  ModelAttribute, ModelAttributeDefaults, ModelToMeshParameters,
  ModelToMeshParametersExtra, SelectableToMeshModelIds,
} from './constants'
import {useAssetLabStateContext} from './asset-lab-context'
import {
  useRequestModelPostProcessing, getOptimizationSettings,
} from './hooks/use-request-model-post-processing'
import {JointToggleButton} from '../ui/components/joint-toggle-button'
import {combine} from '../common/styles'
import {Icon} from '../ui/components/icon'
import {AssetRequestIntervalChecker} from './widgets/asset-request-interval-checker'
import {GridOfSquares} from './widgets/grid-of-squares'
import {AssetGenProgressIndicator} from './widgets/asset-gen-progress-indicator'
import assetLabActions from './asset-lab-actions'
import useActions from '../common/use-actions'
import {useCreditQuery} from '../billing/use-credit-query'
import useCurrentAccount from '../common/use-current-account'
import type {AssetInput} from '../../shared/genai/types/base'
import type {
  ImageToMesh, ImageToMeshHunyuanBase, ImageToMeshTrellisBase, TrellisTextureSize,
} from '../../shared/genai/types/image-to-mesh'
import {extractFblr, promptInputsToInputs} from './generate-request'
import StudioModelPreviewWithLoader from '../studio/studio-model-preview-with-loader'
import {useImportIntoProject} from './hooks/use-import-into-project'
import {
  AssetLabButton, AssetLabButtonLink, AssetLabButtonWithCost, AssetLabNumberInput,
} from './widgets/asset-lab-button'
import {PlaceholderBox} from './widgets/square'
import {gray4} from '../static/styles/settings'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {useRequestResult} from './hooks/use-request-result'
import {AssetLabInputError} from './widgets/asset-lab-input-error'
import {useFblrParentRequest} from './hooks/use-parent-request'
import {AssetGenImageDrop} from './widgets/asset-gen-image-drop'
import {StandardChip} from '../ui/components/standard-chip'
import type {FblrAsset} from './types'
import AssetLabLibraryPicker from './widgets/asset-lab-library-picker'
import {useSelector} from '../hooks'

const MODEL_TYPE_PARTS = new Set(['multi-view', 'multi', 'mini', 'turbo'])
const extractNameAndMode = (modelName: string) => {
  if (!modelName) {
    return {modelName: null, multiMode: null, isMulti: false, isTurbo: false, isMini: false}
  }
  const parts = modelName.split('/')
  // e.g. fal-ai/hunyuan3d/v2/multi-view
  // e.g. fal-ai/hunyuan3d/v2/multi-view/turbo
  // e.g. fal-ai/hunyuan3d/v2/mini/turbo
  // e.g. fal-ai/trellis/multi

  const notModelNamePartIdx = parts.findIndex(p => MODEL_TYPE_PARTS.has(p))
  // Produce fal-ai/hunyuan3d/v2
  const parsedModelName = parts.slice(0,
    notModelNamePartIdx === -1 ? parts.length : notModelNamePartIdx).join('/')
  const indicatorParts = parts.slice(notModelNamePartIdx)

  const isMulti = indicatorParts.some(p => p === 'multi' || p === 'multi-view')
  const isTurbo = indicatorParts.some(p => p === 'turbo')
  const isMini = indicatorParts.some(p => p === 'mini')
  return {
    modelName: parsedModelName,
    multiMode: isMulti ? 'multiView' : 'singleView',
    isMulti,
    isTurbo,
    isMini,
  }
}

const onlyHasUuidInputs = (arr: (string | File)[]): arr is string[] => (
  arr.every(item => !(item instanceof File))
)

const getSubmittingModel = (
  modelName: string, isMulti: boolean, isTurbo: boolean
): ToMeshModelIds => {
  let submittingModel: string = modelName
  if (isMulti) {
    submittingModel = MeshModelToMulti[submittingModel]
  }
  // NOTE(dat): If the predicate below gets complicated, make it into MeshModelToTurbo
  if (isTurbo && !modelName.includes('trellis')) {
    submittingModel = `${submittingModel}/turbo`
  }
  return submittingModel as ToMeshModelIds
}

const AssetLabModelGen = () => {
  const {t} = useTranslation('asset-lab')
  // Load up state from the context to prepopulate our form with previous values. Used for remix.
  const assetLabCtx = useAssetLabStateContext()
  const {
    assetRequestUuid, workflow, inputFblr: inputFblrStr, inputMode: alInputMode, requestLineage,
  } = assetLabCtx.state
  const assetRequests = useSelector(s => s.assetLab.assetRequests)
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)
  const {
    hasResult, resultMeshUrl, resultMesh, isGenerating, resultAssetGenerationUuid, assetRequest,
  } = useRequestResult(assetRequestUuid)

  const {modelName, multiMode, isTurbo} = extractNameAndMode(assetRequest?.input?.modelId)

  // --- Form values that user can then override
  const [model, setModel] = React.useState<ToMeshModelIds>(
    modelName as ToMeshModelIds || 'fal-ai/trellis'
  )
  const recommendedInput: string = multiMode || (
    alInputMode === 'multiView' ? 'multiView' : 'singleView'
  )
  const [inputMode, setInputMode] = React.useState<'singleView' | 'multiView'>(
    recommendedInput as 'singleView' | 'multiView'
  )
  const [modelSpeed, setModelSpeed] = React.useState<'standard' | 'turbo'>(
    isTurbo ? 'turbo' : 'standard'
  )
  const [guidanceStrength, setGuidanceStrength] = React.useState(
    assetRequest?.input?.ssGuidanceStrength ||
    ModelAttributeDefaults.ssGuidanceStrength.defaultValue
  )
  const [detailGuidance, setDetailGuidance] = React.useState(
    assetRequest?.input?.slatGuidanceStrength ||
    ModelAttributeDefaults.slatGuidanceStrength.defaultValue
  )
  const [simplifyMesh, setSimplifyMesh] = React.useState(
    assetRequest?.input?.meshSimplify ||
    ModelAttributeDefaults.meshSimplify.defaultValue
  )
  const [guidanceScale, setGuidanceScale] = React.useState(
    assetRequest?.input?.guidanceScale ||
    ModelAttributeDefaults.guidanceScale.defaultValue
  )
  const [shapeDetail, setShapeDetail] = React.useState(
    assetRequest?.input?.octreeResolution ||
    ModelAttributeDefaults.octreeResolution.defaultValue
  )
  const [textureSize, setTextureSize] = React.useState<TrellisTextureSize>(
    assetRequest?.input?.textureSize as TrellisTextureSize || 1024
  )
  const [uuidOrFileInputs, setUuidOrFileInputs] = React.useState<(File | string)[]>(
    inputFblrStr?.split(',').map(s => s.trim()) || [])
  const meshInputParentRequest = useFblrParentRequest(uuidOrFileInputs)

  // NOTE(coco): Handle the state when users are picking an image prompt from the library
  const [selectedSide, setSelectedSide] =
    React.useState<'front' | 'back' | 'right' | 'left' | undefined>(undefined)

  const updateFblr = (
    updates: {
      front?: FblrAsset
      back?: FblrAsset
      left?: FblrAsset
      right?: FblrAsset
    }
  ) => {
    const updatedFblr = [...uuidOrFileInputs]
    Object.entries(updates).forEach(([orientation, fblrAsset]) => {
      switch (orientation) {
        case 'front':
          updatedFblr[0] = fblrAsset
          break
        case 'back':
          updatedFblr[1] = fblrAsset
          break
        case 'left':
          updatedFblr[2] = fblrAsset
          break
        case 'right':
          updatedFblr[3] = fblrAsset
          break
        default:
          break
      }
    })
    setUuidOrFileInputs(updatedFblr)
    // Only update global FBLR when it's serializable.
    if (onlyHasUuidInputs(updatedFblr)) {
      assetLabCtx.setState({inputFblr: updatedFblr.join(',')})
      // updatedFblr may contain null or undefined entries.
      const numInputImages = updatedFblr.filter(Boolean)
      // The following if/else only sets lineage when a valid front view or FBLR has a parent.
      // Otherwise, setRequestLineageFromInput(undefined) is a no-op.
      if (numInputImages.length === 1 && updatedFblr[0]) {
        assetLabCtx.setRequestLineageFromInput(assetGenerations[updatedFblr[0]]?.RequestUuid)
      } else if (numInputImages.length === 4) {
        const root = assetRequests[updatedFblr[0]]?.uuid
        const blr = updatedFblr.slice(1)
        if (blr.every(uuid => assetRequests[uuid]?.ParentRequestUuid === root)) {
          assetLabCtx.setRequestLineageFromInput(assetGenerations[updatedFblr[1]]?.RequestUuid)
        }
      }
    }
  }

  const {
    processedMeshUrl, postProcessing,
  } = useRequestModelPostProcessing(resultMeshUrl, getOptimizationSettings(model))
  const meshUrl = processedMeshUrl ?? resultMeshUrl

  const classes = useAssetLabStyles()

  const {generateAssets} = useActions(assetLabActions)
  const currentAccount = useCurrentAccount()
  const {refetch: creditRefetch, status: creditStatus} = useCreditQuery()
  const [isImporting, setIsImporting] = React.useState(false)

  const {importFromUrl, hasApp} = useImportIntoProject()

  const [isSubmitting, setIsSubmitting] = React.useState(false)
  const isMultiView = inputMode === 'multiView'
  const submittingModel = getSubmittingModel(model, isMultiView, modelSpeed === 'turbo')

  const handleGenerate = async () => {
    setIsSubmitting(true)
    assetLabCtx.setState({userInputError: null})

    // The ordering is important ['front', 'back', 'left', 'right']
    // Will likely need to have our action reshuffle these asset gen UUIDs based on their
    // orientation. If no orientation found, then they are in their given order.
    if (!isMultiView && uuidOrFileInputs.length < 1) {
      // We need something like the error prompt at the context level
      // TODO(dat): Add error message to assetLabCtx
      // assetLabCtx.setState({error: t('asset_lab.model_gen.single_view_error_need_image')})
      return
    }
    if (isMultiView && uuidOrFileInputs.length < 4) {
      // TODO(dat): Add error message
      // assetLabCtx.setState({error: t('asset_lab.model_gen.multi_view_error_need_images')})
      return
    }

    const images: AssetInput[] = promptInputsToInputs(isMultiView
      ? uuidOrFileInputs
      : [uuidOrFileInputs[0]])

    const request: ImageToMesh = {
      type: 'IMAGE_TO_MESH',
      modelId: submittingModel,
      // Trellis
      ssGuidanceStrength: guidanceStrength,
      ssSamplingSteps: ModelAttributeDefaults.ssSamplingSteps.defaultValue,
      slatGuidanceStrength: detailGuidance,
      slatSamplingSteps: ModelAttributeDefaults.slatSamplingSteps.defaultValue,
      meshSimplify: simplifyMesh,
      textureSize,
      // Hunyuan3D
      numInferenceSteps: ModelAttributeDefaults.numInferenceSteps.defaultValue,
      guidanceScale,
      octreeResolution: shapeDetail,
      images,
      numRequests: 1,
      ParentRequestUuid: meshInputParentRequest?.uuid,
    }

    try {
      const {assetRequest: assetRequestRes} = await generateAssets(currentAccount.uuid, request)
      assetLabCtx.setState({
        assetRequestUuid: assetRequestRes.uuid,
        assetGenerationUuid: undefined,
        inputFblr: extractFblr(assetRequestRes).join(','),
      })
    } catch (error) {
      if (error.status === 400) {
        assetLabCtx.setState({
          userInputError: error.message || t('asset_lab.model_gen.invalid_input'),
        })
      }
      // rethrow for logging
      throw error
    } finally {
      setIsSubmitting(false)
    }
    creditRefetch()
  }

  const workflowParameters = WorkflowToParameters[workflow] || new Set([])
  const hasEnoughInput = !isMultiView
    ? uuidOrFileInputs.length >= 1
    : (uuidOrFileInputs.length > 3 &&
      !uuidOrFileInputs.some(img => img === undefined || img === null || img === ''))

  const cannotGenerate = creditStatus !== 'success' || !hasEnoughInput || isGenerating ||
  isSubmitting

  const parameterToState: Map<keyof ImageToMeshHunyuanBase | keyof ImageToMeshTrellisBase,
  {value: number, setValue: (value: number) => void}> = new Map([
    ['ssGuidanceStrength', {value: guidanceStrength, setValue: setGuidanceStrength}],
    ['slatGuidanceStrength', {value: detailGuidance, setValue: setDetailGuidance}],
    ['meshSimplify', {value: simplifyMesh, setValue: setSimplifyMesh}],
    ['guidanceScale', {value: guidanceScale, setValue: setGuidanceScale}],
    ['octreeResolution', {value: shapeDetail, setValue: setShapeDetail}],
  ])

  const setModelSmartly = (v: SelectableToMeshModelIds) => {
    setModel(v as ToMeshModelIds)
    if (v === 'fal-ai/hunyuan3d/v2/mini' || v === 'fal-ai/hunyuan3d-v21') {
      setInputMode('singleView')
    }
    if (v === 'fal-ai/trellis') {
      setModelSpeed('standard')
    }
  }

  const modelParameters = Array.from(
    ModelToMeshParameters[extractNameAndMode(model).modelName] ?? []
  ) as Array<keyof ImageToMeshHunyuanBase | keyof ImageToMeshTrellisBase>
  const modelParametersExtra = ModelToMeshParametersExtra[model] || new Set([])

  return (
    <div className={classes.assetGenSplit}>
      <div className={combine(classes.genForm, classes.flex4)}>
        <AssetLab3dModelDropdown
          label={t('asset_lab.model_gen.image_to_3d_model')}
          model={model}
          setModel={setModelSmartly}
        />
        <div className={classes.modelParameterRow}>
          <StandardFieldLabel label={t('asset_lab.model_gen.image_prompt')} mutedColor starred />
          {!isMultiView && !selectedSide && (
            <GridOfSquares numColumns={1}>
              <AssetGenImageDrop
                size={360}
                onDrop={(file) => {
                  updateFblr({front: file})
                }}
                onClear={() => updateFblr({front: null})}
                uuidOrFile={uuidOrFileInputs[0]}
                onAddFromLibrary={() => setSelectedSide('front')}
              />
            </GridOfSquares>
          )}
          {isMultiView && !selectedSide && (
            <div style={{maxWidth: '360px'}}>
              <GridOfSquares numColumns={2}>
                <AssetGenImageDrop
                  size={180}
                  bottomLeftContent={<StandardChip text={t('asset_lab.model_gen.front')} />}
                  onDrop={(file) => {
                    updateFblr({front: file})
                  }}
                  onClear={() => updateFblr({front: null})}
                  uuidOrFile={uuidOrFileInputs[0]}
                  onAddFromLibrary={() => setSelectedSide('front')}
                  id='asset-gen-image-drop-front'
                />
                <AssetGenImageDrop
                  size={180}
                  bottomLeftContent={<StandardChip text={t('asset_lab.model_gen.back')} />}
                  onDrop={(file) => {
                    updateFblr({back: file})
                  }}
                  onClear={() => updateFblr({back: null})}
                  uuidOrFile={uuidOrFileInputs[1]}
                  onAddFromLibrary={() => setSelectedSide('back')}
                  id='asset-gen-image-drop-back'
                />
                <AssetGenImageDrop
                  size={180}
                  bottomLeftContent={<StandardChip text={t('asset_lab.model_gen.left')} />}
                  onDrop={(file) => {
                    updateFblr({left: file})
                  }}
                  onClear={() => updateFblr({left: null})}
                  uuidOrFile={uuidOrFileInputs[2]}
                  onAddFromLibrary={() => setSelectedSide('left')}
                  id='asset-gen-image-drop-left'
                />
                <AssetGenImageDrop
                  size={180}
                  bottomLeftContent={<StandardChip text={t('asset_lab.model_gen.right')} />}
                  onDrop={(file) => {
                    updateFblr({right: file})
                  }}
                  onClear={() => updateFblr({right: null})}
                  uuidOrFile={uuidOrFileInputs[3]}
                  onAddFromLibrary={() => setSelectedSide('right')}
                  id='asset-gen-image-drop-right'
                />
              </GridOfSquares>
            </div>
          )}
          {selectedSide && (
            <AssetLabLibraryPicker
              onSelect={(uuid) => { updateFblr({[selectedSide]: uuid}) }}
              onClose={() => setSelectedSide(undefined)}
              filters={['IMAGE']}
            />
          )}
        </div>
        {modelParametersExtra.has('multiview') && (
          <div>
            <StandardFieldLabel label={t('asset_lab.image_gen.input_mode')} mutedColor />
            <div className={classes.inlineToggleContainer}>
              <JointToggleButton
                options={[
                  {
                    value: 'singleView',
                    content: (
                      <div className={combine(classes.iconToggleLabel, classes.strokeIcon)}>
                        <Icon stroke='oneRect' /> {t('asset_lab.image_gen.single_view')}
                      </div>
                    ),
                  },
                  {
                    value: 'multiView',
                    content: (
                      <div className={combine(classes.iconToggleLabel, classes.strokeIcon)}>
                        <Icon stroke='fourRects' />
                        {t('asset_lab.image_gen.multi_view')}
                      </div>
                    ),
                  },
                ]}
                value={inputMode}
                onChange={value => setInputMode(value)}
              />
            </div>
          </div>
        )}
        {modelParametersExtra.has('speed') && (
          <div>
            <StandardFieldLabel label={t('asset_lab.model_gen.speed')} mutedColor />
            <div className={classes.inlineToggleContainer}>
              <JointToggleButton
                options={[
                  {
                    value: 'standard',
                    content: (
                      <div className={classes.iconToggleLabel}>
                        <Icon stroke='ring' /> {t('asset_lab.model_gen.standard')}
                      </div>
                    ),
                  },
                  {
                    value: 'turbo',
                    content: (
                      <div className={classes.iconToggleLabel}>
                        <Icon stroke='leftLightning' /> {t('asset_lab.model_gen.turbo')}
                      </div>
                    ),
                  },
                ]}
                value={modelSpeed}
                onChange={value => setModelSpeed(value)}
              />
            </div>
          </div>
        )}
        {ModelToMeshParameters[extractNameAndMode(model).modelName] &&
          modelParameters.map((parameter) => {
            const parameterData = ModelAttributeDefaults[parameter] as ModelAttribute
            if (!parameterData || parameterData.render === false) {
              return null
            }
            return (
              <div key={parameter} className={classes.modelParameterRow}>
                <StandardFieldLabel
                  label={t(parameterData?.label)}
                  mutedColor
                />
                <div className={classes.guidanceSliderCombo}>
                  <RangeSliderInput
                    id={`model-gen-${parameter}-slider`}
                    value={parameterToState.get(parameter)?.value ?? 0}
                    min={parameterData?.min}
                    step={parameterData?.step}
                    max={parameterData?.max}
                    onChange={value => parameterToState.get(parameter)?.setValue(value)}
                    color={gray4}
                  />
                  <AssetLabNumberInput
                    id={`model-gen-${parameter}-input`}
                    value={parameterToState.get(parameter)?.value ?? 0}
                    onChange={value => parameterToState.get(parameter)
                      .setValue(value)}
                    min={parameterData?.min}
                    step={parameterData?.step}
                    max={parameterData?.max}
                  />
                </div>
              </div>
            )
          })
        }
        {model === 'fal-ai/trellis' && (
          <div className={classes.modelParameterRow}>
            <StandardFieldLabel
              label={t('asset_lab.model_gen.parameter.texture_size_label')}
              mutedColor
            />
            <StandardDropdownField
              id='model-gen-texture-size'
              label=''
              options={[
                {value: '512', content: '512 x 512'},
                {value: '1024', content: '1024 x 1024'},
                {value: '2048', content: '2048 x 2048'},
              ]}
              value={textureSize.toString()}
              onChange={value => setTextureSize(value as unknown as TrellisTextureSize)}
            />
          </div>
        )}
        <AssetLabInputError />
        <div className={classes.generateRow}>
          <div className={classes.generateRowLeft}>
            <AssetLabButton onClick={() => {
              assetLabCtx.setState({
                // Prefer the last multi-view request if there is one, otherwise use the front view.
                assetRequestUuid: requestLineage.multiViews?.[0] || requestLineage.root,
                mode: 'image',
                imageInputUuid: extractFblr(meshInputParentRequest)[0],
              })
            }}
            >
              {t('asset_lab.model_gen.back_to_image')}
            </AssetLabButton>
          </div>
          <div className={classes.generateButton}>
            {BuildIf.LOCAL_DEV && submittingModel}
            {workflowParameters.has('hasAnimation') &&
              <AssetLabButton
                onClick={() => {
                  // set the inputs for 3d model onto the assetLabCtx then move onto the next step
                  // We assume that the extraAssetGen is the front view
                  assetLabCtx.setState({
                    mode: 'animation',
                    inputFblr: undefined,
                    inputMode: 'singleView',
                    assetGenerationUuid: undefined,
                    assetRequestUuid: undefined,
                  })
                }}
                className={classes.tertiaryButton}
              >
                {t('asset_lab.image_gen.skip')}
              </AssetLabButton>
            }
            <AssetLabButtonWithCost
              a8='click;studio;generate-model-button'
              onClick={handleGenerate}
              disabled={cannotGenerate}
              active={!cannotGenerate && (!workflowParameters.has('hasAnimation') && hasResult)}
              label={t('asset_lab.model_gen.generate_3d')}
              type='IMAGE_TO_MESH'
              modelId={submittingModel}
            />
          </div>
        </div>
      </div>
      <div className={combine(classes.genResults, classes.flex6)}>
        {assetRequestUuid && <AssetRequestIntervalChecker id={assetRequestUuid} />}
        {hasResult && (
          <div className={classes.modelContainerOutput}>
            <StudioModelPreviewWithLoader src={meshUrl} envName={BASIC} />
          </div>
        )}
        {!hasResult && (
          <PlaceholderBox requestId={assetRequestUuid} width='100%' height='60vh'>
            <AssetGenProgressIndicator id={assetRequestUuid} />
          </PlaceholderBox>
        )}
        <div className={classes.buttonBar}>
          {hasResult && (
            <AssetLabButton onClick={() => {
              assetLabCtx.setState({
                mode: 'optimizer',
                prevMode: assetLabCtx.state.mode,
                assetGenerationUuid: resultAssetGenerationUuid,
              })
            }}
            >
              <Icon inline stroke='wrench' /> {t('asset_lab.open_in_3d_model_optimizer')}
            </AssetLabButton>
          )}
          {workflow === 'gen-3d-model' && (
            <>
              <AssetLabButtonLink
                href={meshUrl}
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
                      meshUrl, `${resultMesh.uuid}.glb`, 'model/gltf-binary'
                    )
                    setIsImporting(false)
                  }}
                  disabled={!hasResult}
                >
                  <Icon stroke='downloadFile' inline />
                  {t('asset_lab.model_gen.import_into_project')}
                </AssetLabButton>
              )}
            </>
          )}
          {workflow === 'gen-animated-char' && (
            <>
              <div className={classes.generateRowLeft}>
                <AssetLabButtonLink
                  href={resultMeshUrl}
                  target='_blank'
                  rel='noopener noreferrer'
                >
                  <Icon inline stroke='download' />&nbsp;{t('asset_lab.detail_view.download_asset')}
                </AssetLabButtonLink>
                {hasApp && (
                  <AssetLabButton
                    onClick={async () => {
                      setIsImporting(true)
                      await importFromUrl(
                        resultMeshUrl, `${resultMesh.uuid}.glb`, 'model/gltf-binary'
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
              {hasResult && (
                <AssetLabButton
                  onClick={() => {
                    assetLabCtx.setState({
                      mode: 'animation',
                      animInputUuid: resultMesh.uuid,
                      assetRequestUuid: undefined,
                      assetGenerationUuid: undefined,
                    })
                  }}
                  disabled={!hasResult || postProcessing || isGenerating || isSubmitting}
                  active={
                    // eslint-disable-next-line max-len
                    !postProcessing && !isGenerating && workflowParameters.has('hasAnimation') && hasResult
                  }
                >
                  <Icon stroke='guyRunningRight' inline />
                  {t('asset_lab.model_gen.send_to_animation')}
                </AssetLabButton>
              )}

            </>
          )}

        </div>
      </div>
    </div>
  )
}

export {AssetLabModelGen}
