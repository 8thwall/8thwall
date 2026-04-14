import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {ImageTarget as ImageTargetConfig} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'
import {Trans} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {StandardLink} from '../../ui/components/standard-link'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {
  IMAGE_TARGET_PAGE_LINK, OFFLINE_IMAGE_TARGET_PAGE_LINK,
} from '../../apps/image-targets/image-target-constants'
import {ImageTargetConfiguratorMenu} from './image-target-configurator-menu'
import {ImageTargetVisualizer, ImageTargetVisualizerFrame} from './image-target-visualizer'
import {copyDirectProperties} from './copy-component'
import {useStudioStateContext, type ImageTargetShowOption} from '../studio-state-context'
import {useImageTarget} from '../hooks/use-image-target'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {RowMultiSelect, RowTextField} from './row-fields'
import {IMAGE_TARGET_COMPONENT} from './direct-property-components'

const useStyles = createUseStyles({
  container: {
    display: 'flex',
    flexDirection: 'column',
    gap: '1em',
    paddingTop: '1em',
  },
  bottomBar: {
    padding: '4px',
    display: 'flex',
    justifyContent: 'center',
  },
})

interface IImageTargetConfigurator {
  imageTarget: DeepReadonly<ImageTargetConfig>
  onChange: (updater: (newImageTarget: ImageTargetConfig) => ImageTargetConfig) => void
  objectId: string
  resetToPrefab: (componentIds: string[], nonDirect?: any) => void
}

const ImageTargetConfiguratorFull: React.FC<IImageTargetConfigurator> = (
  {imageTarget, onChange, objectId, resetToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const [targetData] = useImageTarget(imageTarget.name)
  const stateCtx = useStudioStateContext()
  const {state: {selectedImageTargetViews}} = stateCtx
  const isTargetInvalid = !targetData && !!imageTarget.name
  const selectedOptions = selectedImageTargetViews[objectId] ??
    ['showGeometry', 'showFullImage', 'showTrackedRegion']

  const filterOptions = [
    {id: 'showGeometry', label: t('image_target_configurator.views.geometry_label')},
    {id: 'showFullImage', label: t('image_target_configurator.views.full_image_label')},
    {id: 'showTrackedRegion', label: t('image_target_configurator.views.tracked_region_label')},
  ]

  const toggleOption = (selectedOption: ImageTargetShowOption) => {
    stateCtx.update((s) => {
      const currOptions = s.selectedImageTargetViews[objectId] ??
      ['showGeometry', 'showFullImage', 'showTrackedRegion']

      const newOptions = currOptions.includes(selectedOption)
        ? currOptions.filter(option => option !== selectedOption)
        : [...currOptions, selectedOption]

      return {
        ...s,
        selectedImageTargetViews: {
          ...s.selectedImageTargetViews,
          [objectId]: newOptions,
        },
      }
    })
  }

  return (
    <ComponentConfiguratorTray
      title={t('image_target_configurator.title')}
      description={(
        <Trans
          i18nKey='image_target_configurator.description'
          ns='cloud-studio-pages'
          components={{
            linkTo: <StandardLink newTab href={IMAGE_TARGET_PAGE_LINK} />,
          }}
        />
      )}
      sectionId='image-target'
      onRemove={() => onChange(() => undefined)}
      onCopy={() => copyDirectProperties(stateCtx, {imageTarget})}
      onResetToPrefab={resetToPrefab
        ? () => resetToPrefab([IMAGE_TARGET_COMPONENT])
        : undefined}
      componentData={[IMAGE_TARGET_COMPONENT]}
    >
      <div className={classes.container}>
        <ImageTargetVisualizerFrame>
          <ImageTargetVisualizer imageTarget={targetData} showWarning={isTargetInvalid} />
          {targetData &&
            <div className={classes.bottomBar}>
              <FloatingPanelButton
                onClick={() => {
                  stateCtx.selectImageTarget(targetData.uuid)
                }}
              >
                {t('image_target_configurator.visualizer.configure_button.label')}
              </FloatingPanelButton>
            </div>
          }
        </ImageTargetVisualizerFrame>
        <ImageTargetConfiguratorMenu
          imageTarget={targetData}
          onChange={e => onChange(o => ({
            ...o,
            name: e.name,
            staticOrientation: e.staticOrientation,
          }))}
        />
      </div>
      <RowMultiSelect
        options={filterOptions}
        selectedIds={selectedOptions}
        onChange={(opt: ImageTargetShowOption) => toggleOption(opt)}
        label={t('image_target_configurator.views.label')}
        formIdLabel='image-target-views'
      />
    </ComponentConfiguratorTray>
  )
}

const ImageTargetConfiguratorMinimal: React.FC<Omit<IImageTargetConfigurator, 'objectId'>> = (
  {imageTarget, onChange, resetToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const stateCtx = useStudioStateContext()

  return (
    <ComponentConfiguratorTray
      title={t('image_target_configurator.title')}
      description={(
        <Trans
          i18nKey='image_target_configurator.offline_description'
          ns='cloud-studio-pages'
          components={{
            linkTo: <StandardLink newTab href={OFFLINE_IMAGE_TARGET_PAGE_LINK} />,
          }}
        />
      )}
      sectionId='image-target'
      onRemove={() => onChange(() => undefined)}
      onCopy={() => copyDirectProperties(stateCtx, {imageTarget})}
      onResetToPrefab={resetToPrefab
        ? () => resetToPrefab([IMAGE_TARGET_COMPONENT])
        : undefined}
      componentData={[IMAGE_TARGET_COMPONENT]}
    >
      <div className={classes.container}>
        <RowTextField
          label={t('image_target_configurator_menu.selector.label')}
          value={imageTarget.name ?? ''}
          onChange={e => onChange(o => ({
            ...o,
            name: e.target.value,
          }))}
        />
      </div>
    </ComponentConfiguratorTray>
  )
}

const ImageTargetConfigurator: React.FC<IImageTargetConfigurator> =
  BuildIf.STUDIO_IMAGE_TARGETS_20260210
    ? ImageTargetConfiguratorFull
    : ImageTargetConfiguratorMinimal

export {
  ImageTargetConfigurator,
}
