import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'

import type {VideoControlsGraphSettings} from '@ecs/shared/scene-graph'
import {VIDEO_DEFAULTS} from '@ecs/shared/video-constants'

import {VIDEO_COMPONENT} from '../hooks/available-components'
import {VIDEO_COMPONENT as GRAPH_VIDEO_COMPONENT} from './direct-property-components'
import {AudioSettingsConfigurator} from './audio-configurator'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {copyDirectProperties} from './copy-component'
import {useStudioStateContext} from '../studio-state-context'
import {RowNumberField} from './row-number-field'
import {RowBooleanField} from './row-boolean-field'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {useStyles as rowFieldStyles} from './row-fields'

type VideoConfig = DeepReadonly<VideoControlsGraphSettings> | null | undefined

interface IVideoControlsConfigurator {
  videoControls: VideoConfig
  onChange: (updater: (current: VideoConfig) => VideoConfig) => void
}

const VideoControlsConfigurator: React.FC<IVideoControlsConfigurator> = (
  {videoControls, onChange}
) => {
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation(['cloud-studio-pages'])
  const rowFieldClasses = rowFieldStyles()

  return (
    <ComponentConfiguratorTray
      title={t('video_controls_configurator.title')}
      description={t('video_controls_configurator.description')}
      sectionId={VIDEO_COMPONENT}
      onRemove={() => onChange(() => undefined)}
      onCopy={() => copyDirectProperties(stateCtx, {videoControls})}
      componentData={[GRAPH_VIDEO_COMPONENT]}
    >
      <RowBooleanField
        label={t('video_controls_configurator.paused.label')}
        id='video-autoplay-checkbox'
        checked={!videoControls?.paused}
        onChange={e => onChange(current => current && ({
          ...current,
          paused: !e.target.checked,
        }))}
      />
      <RowBooleanField
        label={t('video_controls_configurator.loop.label')}
        id='video-loop-checkbox'
        checked={!!videoControls?.loop}
        onChange={e => onChange(current => current && ({
          ...current,
          loop: e.target.checked,
        }))}
      />
      <RowNumberField
        id='video-speed-input'
        label={t('video_controls_configurator.speed.label')}
        value={videoControls?.speed ?? VIDEO_DEFAULTS.speed}
        min={0}
        step={0.01}
        onChange={speed => onChange(current => current && ({
          ...current,
          speed,
        }))}
      />
      <div className={rowFieldClasses.row}>
        <StandardFieldLabel bold label={t('video_controls_configurator.audio.header')} />
      </div>
      <AudioSettingsConfigurator audioSettings={videoControls} onChange={onChange} />
    </ComponentConfiguratorTray>
  )
}

export {
  VideoControlsConfigurator,
}
