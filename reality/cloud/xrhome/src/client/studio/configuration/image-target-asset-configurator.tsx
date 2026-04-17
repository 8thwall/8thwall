import React, {useEffect, useMemo} from 'react'
import {useTranslation} from 'react-i18next'

import {quat} from '@ecs/runtime/math/math'
import type {Vec4Tuple} from '@ecs/shared/scene-graph'

import {createThemedStyles} from '../../ui/theme'
import {
  useOtherImageNames, validateImageTargetName,
} from '../../../shared/validate-image-target-name'
import {useEphemeralEditState} from './ephemeral-edit-state'
import {Loader} from '../../ui/components/loader'
import {AssetNameConfigurator} from './asset-name-configurator'
import {Icon} from '../../ui/components/icon'
import {TertiaryButton} from '../../ui/components/tertiary-button'
import {PrimaryButton} from '../../ui/components/primary-button'
import {RowBooleanField, RowNumberField} from './row-fields'
import {useStudioStateContext} from '../studio-state-context'
import {useSceneContext} from '../scene-context'
import {useDerivedScene} from '../derived-scene-context'
import type {IImageTarget} from '../../common/types/models'
import {ImageTargetMetadataInfo} from './image-target-metadata-info'
import {ImageTargetUserMetadata, validateMetadata} from './image-target-user-metadata'
import {
  ImageTargetConfiguratorSection, ImageTargetSections, ImageTargetTestSection,
} from './image-target-asset-sections'
import {ImageTargetTrackingConfigurator} from './image-target-visualizer'
import {
  ImageTargetGeometryConfigurator, ImageTargetMetadata, InputMode,
} from './image-target-geometry-configurator'
import {
  DEFAULT_ARC_ANGLE, getConinessForRadii, DEFAULT_CYLINDER_CIRCUMFERENCE_BOTTOM,
  DEFAULT_CYLINDER_CIRCUMFERENCE_TOP, DEFAULT_CYLINDER_SIDE_LENGTH, DEFAULT_TARGET_CIRCUMFERENCE,
  DEFAULT_UNIT,
} from '../../apps/image-targets/curved-geometry'
import {ImageTargetModal} from '../image-target-modal'
import {DEFAULT_TOP_RADIUS} from '../image-target-upload'
import {MINIMUM_LONG_LENGTH, MINIMUM_SHORT_LENGTH} from '../../../shared/xrengine-config'
import type {ImageInfo} from '../../apps/image-targets/image-helpers'
import type {CropAreaPixels} from '../../common/image-cropper'
import {Tooltip} from '../../ui/components/tooltip'
import {useImageTarget} from '../hooks/use-image-target'
import {useImageTargetActions} from '../../image-targets/use-image-targets'

type VisualizerFocus = 'main' | 'trackingRegion' | 'arcCurves'
interface VisualizerState {
  focus: VisualizerFocus

  conicalOriginalImage: ImageInfo | undefined
  fullImageUnrotated: ImageInfo | undefined
  fullImageRotated: ImageInfo | undefined

  unconifying: boolean
  prevOriginalUrl: string
  prevTopRadius: number
  prevBottomRadius: number

  croppedImg: ImageInfo | undefined
  prevFullUrl: string
  prevCropArea: CropAreaPixels
}

const useStyles = createThemedStyles(theme => ({
  configuratorContainer: {
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'space-between',
    height: '100%',
  },
  nameIconContainer: {
    marginRight: '0.5em',
    display: 'flex',
  },
  configuratorBody: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
    padding: '1em',
    flexGrow: 1,
    overflowY: 'scroll',
  },
  saveButtonsContainer: {
    display: 'flex',
    flexDirection: 'row',
    gap: '0.5em',
    padding: '1em',
    borderTop: theme.studioSectionBorder,
  },
  modalTrigger: {
    display: 'none',
  },
}))

interface ILoadedImageTargetAssetConfigurator {
  imageTarget: IImageTarget
}

const LoadedImageTargetAssetConfigurator: React.FC<ILoadedImageTargetAssetConfigurator> = ({
  imageTarget,
}) => {
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()
  const {updateImageTarget} = useImageTargetActions()
  const otherImageNames = useOtherImageNames(imageTarget.uuid)
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()

  const renameModalTriggerRef = React.useRef<HTMLButtonElement>(null)
  const assetNameConfiguratorRef = React.useRef<HTMLInputElement>(null)

  const [section, setSection] = React.useState<ImageTargetConfiguratorSection>('configure')
  const [isSaving, setIsSaving] = React.useState(false)
  const [nameError, setNameError] = React.useState<string | null>(null)

  const updateErrorMsg = (error: string | null, previous: string | null) => {
    if (error) {
      stateCtx.update(p => ({...p, errorMsg: error}))
    } else {
      stateCtx.update(p => ({...p, errorMsg: p.errorMsg === previous ? undefined : p.errorMsg}))
    }
  }

  const jsonMetadata = imageTarget.properties as ImageTargetMetadata

  // savable state
  const defaults = useMemo(() => {
    const originalWidth = jsonMetadata.originalWidth || MINIMUM_SHORT_LENGTH
    const originalHeight = jsonMetadata.originalHeight || MINIMUM_LONG_LENGTH
    const topRadius = jsonMetadata.topRadius ?? DEFAULT_TOP_RADIUS
    const bottomRadius = jsonMetadata.bottomRadius ?? (DEFAULT_TOP_RADIUS - MINIMUM_LONG_LENGTH)

    return {
      name: imageTarget.name,
      hasUserMetadata: !!imageTarget.userMetadata,
      userMetadataIsJson: imageTarget.userMetadataIsJson,
      userMetadata: imageTarget.userMetadata,
      arcAngle: jsonMetadata.arcAngle ?? DEFAULT_ARC_ANGLE,
      coniness: jsonMetadata.coniness ?? getConinessForRadii(topRadius, bottomRadius),
      cylinderCircumferenceTop:
        jsonMetadata.cylinderCircumferenceTop ?? DEFAULT_CYLINDER_CIRCUMFERENCE_TOP,
      cylinderCircumferenceBottom:
        jsonMetadata.cylinderCircumferenceBottom ?? DEFAULT_CYLINDER_CIRCUMFERENCE_BOTTOM,
      cylinderSideLength: jsonMetadata.cylinderSideLength ?? DEFAULT_CYLINDER_SIDE_LENGTH,
      targetCircumferenceTop: jsonMetadata.targetCircumferenceTop ?? DEFAULT_TARGET_CIRCUMFERENCE,
      inputMode: jsonMetadata.inputMode ?? InputMode.BASIC,
      unit: jsonMetadata.unit ?? DEFAULT_UNIT,
      top: jsonMetadata.top ?? 0,
      left: jsonMetadata.left ?? 0,
      originalWidth,
      originalHeight,
      width: jsonMetadata.width ?? originalWidth,
      height: jsonMetadata.height ?? originalHeight,
      isRotated: jsonMetadata.isRotated,
      topRadius,
      bottomRadius,
      staticOrientation: jsonMetadata.staticOrientation ?? undefined,
    }
  }, [imageTarget, jsonMetadata])

  const [visualizerState, setVisualizerState] = React.useState<VisualizerState>({
    focus: 'main',
    conicalOriginalImage: undefined,
    fullImageUnrotated: undefined,
    fullImageRotated: undefined,
    unconifying: false,
    prevOriginalUrl: imageTarget.originalImageSrc,
    prevTopRadius: defaults.topRadius,
    prevBottomRadius: defaults.bottomRadius,
    croppedImg: undefined,
    prevFullUrl: imageTarget.type === 'CONICAL'
      ? imageTarget.geometryTextureImageSrc
      : imageTarget.originalImageSrc,
    prevCropArea: {...defaults},
  })

  const updateVisualizerState = (partial: Partial<VisualizerState>) => {
    setVisualizerState(prev => ({...prev, ...partial}))
  }

  const [name, setName] = React.useState(defaults.name)
  const [hasUserMetadata, setHasUserMetadata] = React.useState(defaults.hasUserMetadata)
  const [userMetadataIsJson, setUserMetadataIsJson] = React.useState(defaults.userMetadataIsJson)
  const [userMetadata, setUserMetadata] = React.useState(defaults.userMetadata)
  const [metadata, setMetadata] = React.useState<ImageTargetMetadata>({
    inputMode: defaults.inputMode,
    unit: defaults.unit,
    arcAngle: defaults.arcAngle,
    coniness: defaults.coniness,
    cylinderCircumferenceTop: defaults.cylinderCircumferenceTop,
    cylinderCircumferenceBottom: defaults.cylinderCircumferenceBottom,
    cylinderSideLength: defaults.cylinderSideLength,
    targetCircumferenceTop: defaults.targetCircumferenceTop,
    top: defaults.top,
    left: defaults.left,
    width: defaults.width,
    height: defaults.height,
    originalWidth: defaults.originalWidth,
    originalHeight: defaults.originalHeight,
    isRotated: defaults.isRotated,
    topRadius: defaults.topRadius,
    bottomRadius: defaults.bottomRadius,
    staticOrientation: jsonMetadata.staticOrientation ?? undefined,
  })
  const [staticImageEnabled, setStaticImageEnabled] = React.useState(
    !!jsonMetadata.staticOrientation
  )

  const {
    arcAngle, coniness, cylinderCircumferenceTop, cylinderCircumferenceBottom, cylinderSideLength,
    targetCircumferenceTop, inputMode, unit, top, left, width, height, isRotated, topRadius,
    bottomRadius, staticOrientation,
  } = metadata

  const reset = () => {
    updateVisualizerState({focus: 'main'})
    setIsSaving(false)
    setNameError(null)
    setName(defaults.name)
    setHasUserMetadata(defaults.hasUserMetadata)
    setUserMetadataIsJson(defaults.userMetadataIsJson)
    setUserMetadata(defaults.userMetadata)
    setMetadata({
      inputMode: defaults.inputMode,
      unit: defaults.unit,
      arcAngle: defaults.arcAngle,
      coniness: defaults.coniness,
      cylinderCircumferenceTop: defaults.cylinderCircumferenceTop,
      cylinderCircumferenceBottom: defaults.cylinderCircumferenceBottom,
      cylinderSideLength: defaults.cylinderSideLength,
      targetCircumferenceTop: defaults.targetCircumferenceTop,
      top: defaults.top,
      left: defaults.left,
      width: defaults.width,
      height: defaults.height,
      originalWidth: defaults.originalWidth,
      originalHeight: defaults.originalHeight,
      isRotated: defaults.isRotated,
      topRadius: defaults.topRadius,
      bottomRadius: defaults.bottomRadius,
    })
    setStaticImageEnabled(!!jsonMetadata.staticOrientation)
  }

  const effectiveUserMetadata = hasUserMetadata ? userMetadata : null
  const effectiveIsJson = hasUserMetadata ? userMetadataIsJson : defaults.userMetadataIsJson

  const userMetadataError = hasUserMetadata && validateMetadata(userMetadata, userMetadataIsJson, t)

  const hasChanges =
    name !== defaults.name ||
    effectiveIsJson !== defaults.userMetadataIsJson ||
    effectiveUserMetadata !== defaults.userMetadata ||
    arcAngle !== defaults.arcAngle ||
    coniness !== defaults.coniness ||
    cylinderCircumferenceTop !== defaults.cylinderCircumferenceTop ||
    cylinderCircumferenceBottom !== defaults.cylinderCircumferenceBottom ||
    cylinderSideLength !== defaults.cylinderSideLength ||
    targetCircumferenceTop !== defaults.targetCircumferenceTop ||
    inputMode !== defaults.inputMode ||
    unit !== defaults.unit ||
    top !== defaults.top ||
    left !== defaults.left ||
    width !== defaults.width ||
    height !== defaults.height ||
    isRotated !== defaults.isRotated ||
    topRadius !== defaults.topRadius ||
    bottomRadius !== defaults.bottomRadius ||
    (staticImageEnabled !== !!defaults.staticOrientation) ||
    (staticImageEnabled && (
      (staticOrientation?.rollAngle ?? 0) !== (defaults.staticOrientation?.rollAngle ?? 0) ||
      (staticOrientation?.pitchAngle ?? 0) !== (defaults.staticOrientation?.pitchAngle ?? 0)
    ))

  const hasMetadataError = targetCircumferenceTop > cylinderCircumferenceTop
  const knownErrors = !!nameError || !!userMetadataError || hasMetadataError

  const saveChanges = async (): Promise<boolean> => {
    setIsSaving(true)
    let success = true
    try {
      let newStaticOrientation
      if (BuildIf.STATIC_IMAGE_TARGETS_20250721 && imageTarget.type === 'PLANAR') {
        newStaticOrientation = staticImageEnabled ? staticOrientation : {}
      }
      let finalMetadata: unknown = null
      if (hasUserMetadata) {
        if (userMetadataIsJson) {
          finalMetadata = JSON.parse(userMetadata)
        } else {
          finalMetadata = userMetadata
        }
      }
      await updateImageTarget(imageTarget.name, {
        name,
        metadata: finalMetadata,
        // NOTE(christoph): We're passing all parameters regardless of the type,
        // depending on the electron side to use zod to pull out only the expected properties
        // based on type.
        // @ts-expect-error
        type: imageTarget.type,
        properties: {
          arcAngle,
          coniness,
          cylinderCircumferenceTop,
          cylinderCircumferenceBottom,
          cylinderSideLength,
          targetCircumferenceTop,
          inputMode,
          unit,
          top,
          left,
          width,
          height,
          isRotated,
          topRadius,
          bottomRadius,
          originalWidth: metadata.originalWidth,
          originalHeight: metadata.originalHeight,
          // @ts-expect-error
          staticOrientation: newStaticOrientation,
        },
      })

      updateVisualizerState({focus: 'main'})

      if (staticImageEnabled) {
        derivedScene.getAllSceneObjects().forEach((obj) => {
          if (obj.imageTarget?.name === name) {
            let newRotation = obj.rotation
            newRotation = quat.pitchYawRollDegrees({
              x: metadata.staticOrientation.rollAngle,
              y: 0,
              z: metadata.staticOrientation.pitchAngle,
            }).data() as Vec4Tuple
            ctx.updateObject(obj.id, oldObj => ({
              ...oldObj,
              imageTarget: {
                ...oldObj.imageTarget,
                staticOrientation: staticImageEnabled
                  ? {
                    rollAngle: metadata.staticOrientation.rollAngle,
                    pitchAngle: metadata.staticOrientation.pitchAngle,
                  }
                  : undefined,
              },
              rotation: newRotation,
            }))
          }
        })
      }

      success = true
    } catch (e) {
      // TODO(owenmech): J8W-4698 parse error and convert to translated strings from UX
      stateCtx.update(p => ({...p, errorMsg: e.message}))
      success = false
    }
    setIsSaving(false)
    return success
  }

  const {
    editValue: targetEditName,
    setEditValue: setTargetEditName,
    clear: clearTargetEditName,
  } = useEphemeralEditState({
    value: name,
    deriveEditValue: (v: string) => v,
    parseEditValue: (v: string) => (v ? [true, v] : [false]),
    onChange: (v: string) => {
      setName(v)
      const error = validateImageTargetName(v, {otherImageNames})
      updateErrorMsg(error, nameError)
      setNameError(error)
    },
  })

  useEffect(() => {
    setName(imageTarget.name)
  }, [imageTarget.name])

  // TODO (jeffha): improve error handling? need to visually show to users that there's an error
  //                https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/browse/J8W-4698
  if (hasMetadataError) {
    stateCtx.update(p => ({
      ...p,
      errorMsg:
        t('asset_configurator.image_target_configurator.error.invalid_target_circumference'),
    }))
  }

  const renameModalTrigger = (
    <button
      type='button'
      className={classes.modalTrigger}
      ref={renameModalTriggerRef}
      aria-label={t('file_browser.image_targets.context_menu.rename')}
    />
  )

  const saveButtonDisabled = !hasChanges || isSaving || knownErrors

  return (
    <div className={classes.configuratorContainer}>
      <AssetNameConfigurator
        assetEditName={targetEditName}
        setAssetEditName={setTargetEditName}
        clearAssetEditName={clearTargetEditName}
        submitAssetEditName={() => {}}
        ref={assetNameConfiguratorRef}
        icon={(
          <div className={classes.nameIconContainer}>
            <Icon stroke='imageTarget' />
          </div>
        )}
      />
      {BuildIf.IMAGE_TARGET_TEST_20260415 && <ImageTargetSections
        section={section}
        setSection={setSection}
        saveChanges={saveChanges}
        hasChanges={hasChanges}
        saveButtonDisabled={saveButtonDisabled}
        saveButtonLoading={isSaving}
      />}
      {section === 'configure' &&
        <div className={classes.configuratorBody}>
          <ImageTargetTrackingConfigurator
            imageTarget={imageTarget}
            metadata={metadata}
            onMetadataChange={data => setMetadata(prev => ({...prev, ...data}))}
            visualizerState={visualizerState}
            onTrackingRegionChange={data => setMetadata(prev => ({...prev, ...data}))}
            onVisualizerStateChange={updateVisualizerState}
          />
          <ImageTargetMetadataInfo imageTarget={imageTarget} />
          {(imageTarget.type === 'CONICAL' || imageTarget.type === 'CYLINDER') && (
            <ImageTargetGeometryConfigurator
              targetType={imageTarget.type}
              metadata={metadata}
              onMetadataChange={data => setMetadata(prev => ({...prev, ...data}))}
            />
          )}
          <ImageTargetUserMetadata
            hasUserMetadata={hasUserMetadata}
            setHasUserMetadata={setHasUserMetadata}
            userMetadata={userMetadata}
            setUserMetadata={setUserMetadata}
            userMetadataIsJson={userMetadataIsJson}
            setUserMetadataIsJson={setUserMetadataIsJson}
            userMetadataError={userMetadataError}
          />
          {BuildIf.STATIC_IMAGE_TARGETS_20250721 && (
            <>
              <RowBooleanField
                id='static-image-checkbox'
                label={(
                  <span style={{display: 'inline-flex', alignItems: 'center'}}>
                    {t('asset_configurator.image_target_configurator.static_image')}
                    <Tooltip
                      content={t('asset_configurator.image_target_configurator.description')}
                    >
                      <span style={{marginLeft: '0.5em', cursor: 'pointer'}}>
                        <Icon stroke='info' />
                      </span>
                    </Tooltip>
                  </span>
                )}
                checked={!!staticImageEnabled}
                onChange={(e) => {
                  const nowEnabled = e.target.checked
                  setStaticImageEnabled(nowEnabled)
                  setMetadata((prev) => {
                    let newStaticOrientation
                    if (nowEnabled) {
                      // Restore angles from public API (original metadata) if available
                      newStaticOrientation = jsonMetadata.staticOrientation
                        ? {...jsonMetadata.staticOrientation}
                        : {rollAngle: 0, pitchAngle: 0}
                    } else {
                      newStaticOrientation = {}
                    }
                    return {
                      ...prev,
                      staticOrientation: newStaticOrientation,
                    }
                  })
                }}
              />
              {staticImageEnabled && (
                <>
                  <RowNumberField
                    id='roll-angle'
                    label={t('asset_configurator.image_target_configurator.roll_angle')}
                    value={metadata.staticOrientation?.rollAngle ?? 0}
                    min={-180}
                    max={180}
                    step={1}
                    onChange={v => setMetadata(prev => ({
                      ...prev,
                      staticOrientation: {
                        ...(prev.staticOrientation ?? {}),
                        rollAngle: v,
                        pitchAngle: prev.staticOrientation?.pitchAngle ?? 0,
                      },
                    }))}
                  />
                  <RowNumberField
                    id='pitch-angle'
                    label={t('asset_configurator.image_target_configurator.pitch_angle')}
                    value={metadata.staticOrientation?.pitchAngle ?? 0}
                    min={-180}
                    max={0}
                    step={180}
                    onChange={v => setMetadata(prev => ({
                      ...prev,
                      staticOrientation: {
                        ...(prev.staticOrientation ?? {}),
                        rollAngle: prev.staticOrientation?.rollAngle ?? 0,
                        pitchAngle: v,
                      },
                    }))}
                  />
                </>
              )}
            </>
          )}
        </div>
      }
      {section === 'test' && <ImageTargetTestSection imageTarget={imageTarget} />}
      {section === 'configure' && (
        <div className={classes.saveButtonsContainer}>
          <TertiaryButton
            height='small'
            onClick={reset}
            spacing='full'
            disabled={!hasChanges || isSaving}
          >
            {t('asset_configurator.image_target_configurator.reset')}
          </TertiaryButton>
          <PrimaryButton
            height='small'
            onClick={() => {
              if (defaults.name !== name) {
                renameModalTriggerRef.current?.click()
              } else {
                saveChanges()
              }
            }}
            spacing='full'
            disabled={saveButtonDisabled}
            loading={isSaving}
          >
            {t('asset_configurator.image_target_configurator.save')}
          </PrimaryButton>
        </div>
      )}
      <ImageTargetModal
        trigger={renameModalTrigger}
        onSubmit={() => {
          saveChanges()
          assetNameConfiguratorRef.current?.focus()
        }}
        onCancel={() => {
          setName(defaults.name)
        }}
        heading={t('file_browser.image_targets.edit_modal.header')}
        body={t('file_browser.image_targets.edit_modal.body')}
        submitLabel={t('file_browser.image_targets.edit_modal.confirm')}
        cancelLabel={t('file_browser.image_targets.edit_modal.cancel')}
      />
    </div>
  )
}

const ImageTargetAssetConfigurator: React.FC = () => {
  const stateCtx = useStudioStateContext()
  const {selectedImageTarget} = stateCtx.state
  const [imageTarget, loading] = useImageTarget(selectedImageTarget)

  if (loading) {
    return <Loader />
  }

  return imageTarget
    ? (
      <LoadedImageTargetAssetConfigurator
        key={`${imageTarget.uuid}/${imageTarget.updated}`}
        imageTarget={imageTarget}
      />
    )
    : null
}

export type {
  VisualizerFocus,
  VisualizerState,
}

export {
  ImageTargetAssetConfigurator,
}
