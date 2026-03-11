import React from 'react'
import {useTranslation} from 'react-i18next'

import {StandardTextAreaField} from '../ui/components/standard-text-area-field'
import {JointToggleButton} from '../ui/components/joint-toggle-button'
import {StandardFieldLabel} from '../ui/components/standard-field-label'
import {useSelector} from '../hooks'
import {AssetGenImageDrop} from './widgets/asset-gen-image-drop'
import {useAssetLabStyles} from './asset-lab-styles'
import {AssetLabImageModelDropdown} from './widgets/asset-lab-model-dropdown'
import type {GenerateRequest} from '../../shared/genai/types/generate'
import {RerollOrientation, ToImage} from '../../shared/genai/types/base'
import {ImageStyle} from '../../shared/genai/types/base'
import {SquareAsset, SquareAssetFwdRef} from './widgets/square-asset'
import {AssetGenProgressIndicator} from './widgets/asset-gen-progress-indicator'
import {useAssetLabStateContext} from './asset-lab-context'
import {
  GridOfSquares, ONE_COLUMN_ASSET_GRID_SIZE, GRID_OF_SQUARES_ASSET_SIZE,
} from './widgets/grid-of-squares'
import {combine} from '../common/styles'
import {Icon} from '../ui/components/icon'
import useActions from '../common/use-actions'
import assetLabActions from './asset-lab-actions'
import type {ImageToImage} from '../../shared/genai/types/image-to-image'
import type {TextToImage} from '../../shared/genai/types/text-to-image'
import useCurrentAccount from '../common/use-current-account'
import {useImportIntoProject} from './hooks/use-import-into-project'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {AssetRequestIntervalChecker} from './widgets/asset-request-interval-checker'
import {
  AllToImageKeys, ModelToParameters, MULTIVEW_COUNT_WITHOUT_FRONT, WorkflowToParameters,
  modelsThatRequireImages,
  type ToImageModelIds,
} from './constants'
import type {AssetInput} from '../../shared/genai/types/base'
import {useCreditQuery} from '../billing/use-credit-query'
import {extractFblr, makeUuidInput, promptInputToInputs, urlToUuid} from './generate-request'
import {SolidMessageBanner} from './widgets/solid-banner'
import {AssetLabButton, AssetLabButtonWithCost} from './widgets/asset-lab-button'
import {useRequestResult} from './hooks/use-request-result'
import {MIN_PROMPT_LEN} from '../../shared/genai/constants'
import {AssetLabInputError} from './widgets/asset-lab-input-error'
import {usePrevResults} from './hooks/use-prev-results'
import {AssetGenerationsForRequest} from './asset-generations-for-request'
import {AssetRequestPrompt} from './widgets/asset-gen-prompt'
import {useMultiViewParentRequest} from './hooks/use-parent-request'
import {useDownloadAssets} from './hooks/use-download-assets'
import AssetLabLibraryPicker from './widgets/asset-lab-library-picker'
import {AssetGenActionButtons} from './widgets/asset-gen-action-buttons'
import {AssetLabRerollButton} from './widgets/detail-view-button-widgets'

type AssetReqToReroll = Record<string, Partial<Record<RerollOrientation, string>>>

const DEFAULT = {
  aspectRatio: '1:1' as ToImage['aspectRatio'],
}

const indexToOrientation: Record<number, RerollOrientation> = {
  0: RerollOrientation.BACK,
  1: RerollOrientation.LEFT,
  2: RerollOrientation.RIGHT,
}

const AssetLabImageGen = () => {
  const {t} = useTranslation('asset-lab')
  const assetLabCtx = useAssetLabStateContext()
  const {assetRequestUuid, workflow, requestLineage, imageInputUuid} = assetLabCtx.state
  const {
    assetGenerations: assetGenerationsFromRequest, hasResult, isGenerating, assetRequest,
  } = useRequestResult(assetRequestUuid)
  const assetRequests = useSelector(s => s.assetLab.assetRequests)
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)
  const assetGenByReq = useSelector(s => s.assetLab.assetGenerationsByAssetRequest)
  const parentRequest = useMultiViewParentRequest(
    workflow !== 'gen-image' ? assetRequestUuid : null
  )
  const frontViewGen = parentRequest?.AssetGenerations?.[0]
  const [model, setModel] = React.useState<ToImageModelIds>(
    assetRequest?.input?.modelId as ToImageModelIds || 'gpt-image-1'
  )
  const [prompt, setPrompt] = React.useState(
    assetRequest?.input?.prompt as string || ''
  )
  const [negativePrompt, setNegativePrompt] = React.useState(
    assetRequest?.input?.negativePrompt as string || ''
  )
  const [aspectRatio, setAspectRatio] = React.useState<ToImage['aspectRatio']>(
    assetRequest?.input?.aspectRatio as ToImage['aspectRatio'] || DEFAULT.aspectRatio
  )
  const [numRequests, setNumRequests] = React.useState(
    assetRequest?.input?.numRequests === 1 ? '1' : '4'
  )
  const [background, setBackground] = React.useState(
    assetRequest?.input?.background || 'opaque'
  )
  const [uuidOrFileInput, setUuidOrFileInput] = React.useState<string | File>(imageInputUuid || '')
  const currentAccount = useCurrentAccount()

  const classes = useAssetLabStyles()
  const {generateAssets} = useActions(assetLabActions)
  const {refetch: creditRefetch, status: creditStatus} = useCreditQuery()
  const [assetReqToReroll, setAssetReqToReroll] = React.useState<AssetReqToReroll>({})

  const has4Results = assetGenerationsFromRequest?.length >= 4 ||
   (assetGenerationsFromRequest?.length === 3 && !!frontViewGen)

  const requestNum = Number.parseInt(numRequests, 10)
  const isRequests4 = requestNum >= 4

  const workflowParameters = WorkflowToParameters[workflow] || new Set([])
  const modelParameters = ModelToParameters[model] as Set<AllToImageKeys> || new Set([])

  // NOTE(coco): Handle the state when users are picking an image prompt from the library
  const [isSelecting, setIsSelecting] = React.useState(false)

  // Support scrolling of previous asset requests
  const {prevAssetRequests, appendAssetRequest} = usePrevResults(
    (requestLineage.multiViews?.length > 1 ? requestLineage.multiViews.slice(1) : []).map(uuid => ({
      assetRequestUuid: uuid,
      extraAssetGenUuid: extractFblr(assetRequests[uuid])[0],
    }))
  )

  React.useEffect(() => {
    if (workflowParameters.has('hasAnimation')) {
      setModel('gpt-image-1')
    }
  }, [workflow])

  // When the user clicks, we need to disable the buttons immediately while we prep for sending
  const [isSubmitting, setIsSubmitting] = React.useState(false)
  const handleGenerateRaw = async (
    modelId: TextToImage['modelId'] | ImageToImage['modelId'],
    style: ImageStyle | null,
    batchSize: number,
    images?: AssetInput[],
    skipAssetRequestHistory = false,
    orientation?: RerollOrientation
  ) => {
    setIsSubmitting(true)
    let request: GenerateRequest
    // When we switch from a model that supports image-to-image to one that does not, images.length
    // is not 0 but we still need to use text-to-image request type.
    const requestType = (images?.length > 0 && ModelToParameters[modelId].has('images'))
      ? 'IMAGE_TO_IMAGE'
      : 'TEXT_TO_IMAGE'
    const requestAspectRatio = workflowParameters.has('aspectRatio')
      ? aspectRatio
      : DEFAULT.aspectRatio
    if (requestType === 'IMAGE_TO_IMAGE') {
      request = {
        type: 'IMAGE_TO_IMAGE',
        modelId: modelId as ImageToImage['modelId'],
        prompt,
        aspectRatio: requestAspectRatio,
        numRequests: batchSize,
        images,
        outputFormat: 'png',
        style,
        ParentRequestUuid: (batchSize === MULTIVEW_COUNT_WITHOUT_FRONT || orientation !== undefined)
          ? parentRequest?.uuid
          : undefined,
        orientation,
      }
    } else {
      // Support negative prompt instead of images
      request = {
        type: 'TEXT_TO_IMAGE',
        modelId: modelId as TextToImage['modelId'],
        prompt,
        aspectRatio: requestAspectRatio,
        numRequests: batchSize,
        outputFormat: 'png',
        negativePrompt,
        style,
      }
    }

    if (modelParameters.has('background') &&
      (request.type === 'IMAGE_TO_IMAGE' || request.type === 'TEXT_TO_IMAGE')
    ) {
      request.background = background
    }

    try {
      const {assetRequest: assetRequestRes} = await generateAssets(currentAccount.uuid, request)
      // Store the old asset request for scrolling below
      if (assetRequestUuid && !skipAssetRequestHistory) {
        const extraAssetGenUuid = assetGenerationsFromRequest?.length > 1
          ? frontViewGen?.uuid
          : undefined
        appendAssetRequest(assetRequestUuid, extraAssetGenUuid)
      }
      if (orientation) {
        setAssetReqToReroll(prev => ({
          ...prev,
          [assetRequestUuid]: {
            ...(prev[assetRequestUuid] || {}),
            [orientation]: assetRequestRes.uuid,
          },
        }))
      } else {
        assetLabCtx.setState({assetRequestUuid: assetRequestRes.uuid})
      }
    } catch (error) {
      if (error.status === 400) {
        assetLabCtx.setState({
          userInputError: error.message || t('asset_lab.image_gen.invalid_input'),
        })
      }
      // rethrow for logging
      throw error
    } finally {
      setIsSubmitting(false)
    }
    creditRefetch()
  }

  // --- Visualization squares
  // This works because requestNum is cap at 4.
  const resultSquareRefs = [
    React.useRef<HTMLImageElement>(),
    React.useRef<HTMLImageElement>(),
    React.useRef<HTMLImageElement>(),
    React.useRef<HTMLImageElement>(),
  ]

  // --- Main generation button
  const handleGenerate = async () => {
    assetLabCtx.setState({userInputError: null})

    if (workflow === 'gen-image') {
      // A straight image gen has no added style. Style might modify the prompt internally.
      return handleGenerateRaw(model, null, requestNum, promptInputToInputs(uuidOrFileInput))
    }

    if (workflow === 'gen-3d-model') {
      // 3d workflow
      // We are generating ONE (1) frontal image, allow user to drop their image prompt.
      // User should NOT call this for the 2nd step. See generateMultiview below.
      return handleGenerateRaw(
        model,
        isRequests4 ? ImageStyle.GAME_ASSET : ImageStyle.SINGLEVIEW,
        1,
        promptInputToInputs(uuidOrFileInput),
        false
      )
    }

    if (workflow === 'gen-animated-char') {
      // animation workflow
      // We are generating ONE (1) frontal image, allow user to drop their image prompt.
      // User should NOT call this for the 2nd step. See generateMultiview below.

      // Animation workflow does NOT allow single view
      return handleGenerateRaw(
        model,
        ImageStyle.ANIMATED_GAME_ASSET,
        1,
        promptInputToInputs(uuidOrFileInput),
        false
      )
    }

    // TODO(dat): Show error message on UI that input is not valid
    return null
  }

  const mainGenerateButtonLabel = (workflow !== 'gen-image' && isRequests4)
    ? t('asset_lab.image_gen.generate_front_view')
    : t('asset_lab.image_gen.generate')

  // --- Next step generation button
  const generateMultiview = async () => {
    if (!(hasResult || frontViewGen)) {
      return null
    }
    // Skip the asset request history since we are showing the previous one in the extra asset
    // square.
    return handleGenerateRaw(
      'gpt-image-1',
      workflowParameters.has('hasAnimation') ? ImageStyle.ANIMATED_MULTIVIEW : ImageStyle.MULTIVIEW,
      MULTIVEW_COUNT_WITHOUT_FRONT,
      [makeUuidInput(frontViewGen.uuid)],
      true
    )
  }

  const generateReroll = async (inputIndex: number) => {
    if (!hasResult && !frontViewGen) {
      return null
    }
    return handleGenerateRaw(
      'gpt-image-1',
      workflowParameters.has('hasAnimation') ? ImageStyle.ANIMATED_MULTIVIEW : ImageStyle.MULTIVIEW,
      1,
      [makeUuidInput(frontViewGen.uuid), ...assetGenerationsFromRequest.map(
        (g, idx) => (idx !== inputIndex
          ? makeUuidInput(g)
          : null)
      ).filter(Boolean)],
      true,
      indexToOrientation[inputIndex]
    )
  }
  // --- Action and status state
  const [isImporting, setIsImporting] = React.useState(false)
  const {importImg, hasApp} = useImportIntoProject()
  const {downloadFromUrl} = useDownloadAssets()
  const assetGenImporteds = useCurrentGit(git => (
    assetGenerationsFromRequest?.map(agId => !!git.filesByPath[`assets/${agId}.png`])
  ))
  const importAllIntoProject = async () => {
    setIsImporting(true)
    await Promise.all(
      assetGenerationsFromRequest?.map((agUuid, i) => {
        const fileName = `${agUuid}.png`
        if (assetGenImporteds[i]) {
          return Promise.resolve()
        }
        return importImg(resultSquareRefs[i], fileName)
      })
    )
    setIsImporting(false)
  }

  const [isDownloading, setIsDownloading] = React.useState(false)
  const downloadAll = async () => {
    setIsDownloading(true)
    await Promise.all(
      resultSquareRefs?.map((e) => {
        if (!e.current) {
          return Promise.resolve()
        }
        return downloadFromUrl(e.current.src, urlToUuid(e.current.src))
      })
    )
    setIsDownloading(false)
  }

  const requiresImage = modelsThatRequireImages.has(model)

  const cannotGenerate = creditStatus !== 'success' || prompt.trim().length < MIN_PROMPT_LEN ||
   (requiresImage && !uuidOrFileInput) || isGenerating || isSubmitting

  const rerollProps = workflowParameters.has('has3d') && {
    generateReroll,
    creditStatus,
    isGenerating,
    isSubmitting,
  }

  return (
    <div className={classes.assetGenSplit}>
      <div className={classes.genForm}>
        {workflow === 'gen-3d-model' && (
          <SolidMessageBanner
            type='info'
            message={t('asset_lab.image_gen.generate_from_info_message')}
          />
        )}
        {workflow === 'gen-animated-char' && (
          <SolidMessageBanner
            type='info'
            message={t('asset_lab.image_gen.generate_animated_char_info_message')}
          />
        )}
        <AssetLabImageModelDropdown
          label={t('asset_lab.image_gen.text_to_image_model')}
          model={model}
          setModel={(value) => {
            setModel(value as ToImageModelIds)
            if (value !== 'gpt-image-1') {
              setNumRequests('1')
            }
          }}
        />
        <StandardTextAreaField
          starredLabel
          id='image-gen-prompt'
          label={t('asset_lab.image_gen.text_prompt')}
          value={prompt}
          onChange={e => setPrompt(e.target.value)}
        />
        {modelParameters.has('negativePrompt') &&
          <StandardTextAreaField
            id='image-gen-negative-prompt'
            label={t('asset_lab.image_gen.negative_prompt_optional')}
            value={negativePrompt}
            onChange={e => setNegativePrompt(e.target.value)}
          />
        }
        {modelParameters.has('images') &&
          <div>
            <StandardFieldLabel
              starred={requiresImage}
              label={requiresImage
                ? t('asset_lab.image_gen.image_prompt')
                : t('asset_lab.image_gen.image_prompt_optional')}
              mutedColor
            />
            {isSelecting
              ? <AssetLabLibraryPicker
                  onSelect={(uuid) => {
                    setUuidOrFileInput(uuid)
                    assetLabCtx.setState({imageInputUuid: uuid})
                  }}
                  onClose={() => setIsSelecting(false)}
                  filters={['IMAGE']}
              />
              : <AssetGenImageDrop
                  onAddFromLibrary={() => setIsSelecting(true)}
                  size={360}
                  uuidOrFile={uuidOrFileInput}
                  onClear={() => {
                    setUuidOrFileInput(null)
                    assetLabCtx.setState({imageInputUuid: undefined})
                  }}
                  onDrop={(file) => { setUuidOrFileInput(file) }}
              />
              }
          </div>
        }
        {modelParameters.has('aspectRatio') && workflowParameters.has('aspectRatio') &&
          <div>
            <StandardFieldLabel
              label={t('asset_lab.image_gen.aspect_ratio')}
              mutedColor
            />
            <JointToggleButton
              options={[
                {
                  value: '1:1' as const,
                  content: t('asset_lab.image_gen.square'),
                },
                {
                  value: '2:3' as const,
                  content: t('asset_lab.image_gen.portrait'),
                },
                {
                  value: '3:2' as const,
                  content: t('asset_lab.image_gen.landscape'),
                },
              ]}
              value={aspectRatio}
              onChange={value => setAspectRatio(value)}
            />
          </div>
        }
        {workflowParameters.has('singleView') && model === 'gpt-image-1' && (
          <div>
            {workflow === 'gen-image'
              ? <StandardFieldLabel
                  label={t('asset_lab.image_gen.batch_size')}
                  mutedColor
              />
              : <StandardFieldLabel
                  label={t('asset_lab.image_gen.input_mode')}
                  mutedColor
              />
            }
            <div className={classes.inlineToggleContainer}>
              <JointToggleButton
                options={[
                  {
                    value: '1' as const,
                    content: (
                      <div className={combine(classes.iconToggleLabel, classes.strokeIcon)}>
                        <Icon stroke='oneRect' />
                        {workflow === 'gen-image'
                          ? t('asset_lab.images', {count: 1})
                          : t('asset_lab.image_gen.single_view')}
                      </div>
                    ),
                  },
                  {
                    value: '4' as const,
                    content: (
                      <div className={combine(classes.iconToggleLabel, classes.strokeIcon)}>
                        <Icon stroke='fourRects' />
                        {workflow === 'gen-image'
                          ? t('asset_lab.images', {count: 4})
                          : t('asset_lab.image_gen.multi_view')}
                      </div>
                    ),
                  },
                ]}
                value={numRequests}
                onChange={value => setNumRequests(value)}
              />
            </div>
          </div>
        )}
        {modelParameters.has('background') &&
          <>
            <StandardFieldLabel
              label={t('asset_lab.image_gen.background.label')}
              mutedColor
            />
            <div className={classes.inlineToggleContainer}>
              <JointToggleButton
                options={[
                  {
                    value: 'transparent' as const,
                    content: (
                      <div className={classes.iconToggleLabel}>
                        <Icon stroke='transparent' />
                        {t('asset_lab.image_gen.background.transparent')}
                      </div>
                    ),
                  },
                  {
                    value: 'opaque' as const,
                    content: (
                      <div className={classes.iconToggleLabel}>
                        <Icon stroke='opaque' color='muted' />
                        {t('asset_lab.image_gen.background.opaque')}
                      </div>
                    ),
                  },
                ]}
                value={background}
                onChange={value => setBackground(value)}
              />
            </div>
          </>
        }
        <AssetLabInputError />
        <div className={classes.generateRow}>
          <div className={classes.generateButton}>
            {workflowParameters.has('has3d') &&
              <AssetLabButton
                onClick={() => {
                  // set the inputs for 3d model onto the assetLabCtx then move onto the next step
                  // We assume that the extraAssetGen is the front view
                  assetLabCtx.setState({
                    mode: 'model',
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
              a8='click;studio;generate-image-button'
              onClick={handleGenerate}
              disabled={cannotGenerate}
              active={!cannotGenerate && (workflow === 'gen-image' || !hasResult)}
              label={mainGenerateButtonLabel}
              type='TO_IMAGE'
              modelId={model}
              multiplier={(workflowParameters.has('has3d') && isRequests4) ? 1 : requestNum}
            />
          </div>
        </div>
      </div>
      <div className={classes.genResults}>
        <div className={classes.genResultsContent}>
          {assetRequestUuid &&
            <>
              <AssetRequestIntervalChecker id={assetRequestUuid} />
              <AssetGenerationsForRequest
                assetRequestUuid={assetRequestUuid}
                render={(assetReqId, assetGenIds) => (
                  <>
                    <AssetRequestPrompt uuid={assetReqId} />
                    <GridOfSquares
                      numColumns={assetRequests[assetReqId]?.input?.numRequests === 1 ? 1 : 2}
                      columnSize={`${GRID_OF_SQUARES_ASSET_SIZE}px`}
                    >
                      {frontViewGen && frontViewGen.uuid !== assetGenIds[0] &&
                        <SquareAssetFwdRef
                          key={frontViewGen.uuid}
                          generationId={frontViewGen.uuid}
                          requestId={assetReqId}
                          size={GRID_OF_SQUARES_ASSET_SIZE}
                          ref={resultSquareRefs[0]}
                          onHoverContent={() => <AssetGenActionButtons id={frontViewGen.uuid} />}
                        />
                      }
                      {assetGenIds.map((agId, idx) => {
                        const rerollReq = assetReqToReroll[assetReqId]?.[indexToOrientation[idx]]
                        const reqId = rerollReq || assetReqId
                        const genId = rerollReq ? assetGenByReq[rerollReq]?.[0] : agId
                        return (
                          <React.Fragment key={genId || `placeholder-${idx}`}>
                            {rerollReq && <AssetRequestIntervalChecker id={reqId} />}
                            <SquareAssetFwdRef
                              generationId={genId}
                              requestId={reqId}
                              size={GRID_OF_SQUARES_ASSET_SIZE}
                              ref={resultSquareRefs[frontViewGen ? idx + 1 : idx]}
                              onHoverContent={() => (
                                genId &&
                                  <AssetGenActionButtons
                                    id={genId}
                                    bottomRight={rerollProps &&
                                      <AssetLabRerollButton {...rerollProps} inputIndex={idx} />
                                    }
                                  />
                              )}
                            >
                              <AssetGenProgressIndicator id={reqId} />
                            </SquareAssetFwdRef>
                          </React.Fragment>
                        )
                      })}
                    </GridOfSquares>
                  </>
                )}
              />
            </>
          }
          {!assetRequestUuid &&
            <SquareAssetFwdRef
              key=''
              generationId=''
              requestId=''
              size={ONE_COLUMN_ASSET_GRID_SIZE}
              ref={resultSquareRefs[0]}
            >
              <AssetGenProgressIndicator id='' />
            </SquareAssetFwdRef>
          }
          <div className={classes.buttonBar}>
            <AssetLabButton
              onClick={downloadAll}
              disabled={!hasResult}
              loading={isDownloading}
            >
              <Icon inline stroke='download' />&nbsp;
              {t(`asset_lab.detail_view.download_asset${
                assetGenerationsFromRequest?.length !== 1 ? 's' : ''
              }`)}
            </AssetLabButton>
            {workflow === 'gen-image' && hasApp && (
              <AssetLabButton
                onClick={importAllIntoProject}
                disabled={!hasResult}
                loading={isImporting}
              >
                <Icon stroke='downloadFile' inline />
                {t('asset_lab.image_gen.import_all_into_project')}
              </AssetLabButton>
            )}
            {workflowParameters.has('has3d') && (
              <>
                {isRequests4 && (
                  <div className={classes.generateButton}>
                    <AssetLabButtonWithCost
                      active={!has4Results}
                      onClick={generateMultiview}
                      disabled={
                        assetRequest?.status === 'PROCESSING' ||
                        assetRequest?.status === 'REQUESTED' ||
                        !(frontViewGen || hasResult)
                      }
                      label={t('asset_lab.image_gen.generate_multi_view')}
                      type='TO_IMAGE'
                    // Multi-view always uses gpt-image-1.
                      modelId='gpt-image-1'
                      multiplier={MULTIVEW_COUNT_WITHOUT_FRONT}
                    />
                  </div>
                )}
                <AssetLabButton
                  onClick={() => {
                  // set the inputs for 3d model onto the assetLabCtx then move onto the next step
                  // We assume that the extraAssetGen is the front view
                    if (numRequests === '1') {
                    // single view mode
                      assetLabCtx.setState({
                        mode: 'model',
                        inputFblr: assetGenerationsFromRequest[0],
                        inputMode: 'singleView',
                        assetGenerationUuid: undefined,
                        assetRequestUuid: undefined,
                      })
                    } else {
                      const blrMap = assetGenerationsFromRequest
                        .reduce((acc, genUuid) => {
                          const assetGen = assetGenerations[genUuid]
                          acc[assetGen?.metadata?.orientation as RerollOrientation] = genUuid
                          return acc
                        }, {} as Record<RerollOrientation, string>)
                      const inputFblr = [
                        frontViewGen.uuid,
                        blrMap.back,
                        blrMap.left,
                        blrMap.right,
                      ]
                      assetLabCtx.setState({
                        mode: 'model',
                        inputFblr: inputFblr.join(','),
                        inputMode: 'multiView',
                        assetGenerationUuid: undefined,
                        assetRequestUuid: undefined,
                      })
                    }
                  }}
                  disabled={!hasResult || (isRequests4 && !has4Results)}
                  active={(isRequests4 && has4Results) || (!isRequests4 && hasResult)}
                >
                  <Icon stroke='meshCube' inline />
                  {t('asset_lab.image_gen.send_to_3d_model')}
                </AssetLabButton>
              </>
            )}
          </div>
        </div>
        {prevAssetRequests.length > 0 && (
          prevAssetRequests
            .filter(({
              assetRequestUuid: prevAssetRequestUuid,
            }) => assetRequests[prevAssetRequestUuid]?.status === 'SUCCESS')
            .map(({assetRequestUuid: prevAssetRequestUuid, extraAssetGenUuid}) => (
              <div className={classes.genResultsContent} key={prevAssetRequestUuid}>
                <AssetGenerationsForRequest
                  assetRequestUuid={prevAssetRequestUuid}
                  render={(assetReqId, assetGenIds) => (
                    <>
                      <AssetRequestPrompt uuid={assetReqId} />
                      <GridOfSquares
                        numColumns={assetGenIds.length > 1 ? 2 : 1}
                        columnSize={`${GRID_OF_SQUARES_ASSET_SIZE}px`}
                      >
                        {extraAssetGenUuid &&
                          <SquareAsset
                            key={extraAssetGenUuid}
                            generationId={extraAssetGenUuid}
                            size={GRID_OF_SQUARES_ASSET_SIZE}
                            onHoverContent={() => <AssetGenActionButtons id={extraAssetGenUuid} />}
                          />
                      }
                        {assetGenIds.map(agId => (
                          <SquareAsset
                            key={agId}
                            generationId={agId}
                            requestId={assetRequestUuid}
                            size={GRID_OF_SQUARES_ASSET_SIZE}
                            onHoverContent={() => <AssetGenActionButtons id={agId} />}
                          />
                        ))}
                      </GridOfSquares>
                    </>
                  )}
                />
              </div>
            ))
        )}
      </div>
    </div>
  )
}

export {AssetLabImageGen}
