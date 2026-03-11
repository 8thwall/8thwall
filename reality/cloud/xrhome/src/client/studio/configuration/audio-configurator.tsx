import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import type {AudioSettings, DistanceModel, Resource} from '@ecs/shared/scene-graph'
import {
  DEFAULT_VOLUME, DEFAULT_PITCH, DEFAULT_REF_DISTANCE, DEFAULT_DISTANCE_MODEL,
  DEFAULT_ROLLOFF_FACTOR, DEFAULT_MAX_DISTANCE,
} from '@ecs/shared/audio-constants'

import {AssetSelector} from '../ui/asset-selector'
import {RowNumberField, RowSelectField, RowBooleanField} from './row-fields'
import {copyDirectProperties} from './copy-component'
import {useStudioStateContext} from '../studio-state-context'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {AUDIO_COMPONENT} from '../hooks/available-components'
import {AUDIO_COMPONENT as GRAPH_AUDIO_COMPONENT} from './direct-property-components'
import {ScenePathScopeProvider} from '../scene-path-input-context'
import {renderValueBoolean} from './diff-chip-default-renderers'
import {CURRENT_SCOPE} from './expanse-field-diff-chip'

type AudioControlSettings = DeepReadonly<Omit<AudioSettings, 'src'>> | null | undefined

interface IAudioSettingsConfigurator {
  audioSettings: AudioControlSettings
  onChange: (updater: (current: AudioControlSettings) => AudioControlSettings) => void
}

const AudioSettingsConfigurator: React.FC<IAudioSettingsConfigurator> = ({
  audioSettings,
  onChange,
}) => {
  const id = React.useId()
  const {t} = useTranslation(['cloud-studio-pages'])

  const distanceModel = audioSettings?.distanceModel ?? DEFAULT_DISTANCE_MODEL

  const audioOptions = [
    {value: 'linear', content: t('audio_configurator.dropdown.select.linear')},
    {value: 'inverse', content: t('audio_configurator.dropdown.select.inverse')},
    {value: 'exponential', content: t('audio_configurator.dropdown.select.exponential')},
  ]

  const handleRolloffChange = (value: number) => {
    onChange(current => current && ({
      ...current,
      rolloffFactor: distanceModel === 'linear' ? Math.min(value, DEFAULT_ROLLOFF_FACTOR) : value,
    }))
  }

  const handleNumberInputChange = (
    value: number, key: keyof AudioSettings
  ) => {
    if (key === 'rolloffFactor') {
      handleRolloffChange(value)
      return
    }

    onChange(current => current && ({
      ...current,
      [key]: value,
    }))
  }

  return (
    <>
      <div>
        <RowNumberField
          id={`audio-volume${id}`}
          expanseField={{
            leafPaths: [['volume']],
            diffOptions: {
              defaults: [DEFAULT_VOLUME],
            },
          }}
          onChange={volume => onChange(current => current && ({
            ...current,
            volume,
          }))}
          value={audioSettings?.volume ?? DEFAULT_VOLUME}
          min={0}
          max={1}
          step={0.01}
          label={t('audio_configurator.volume.label')}
        />
        <RowBooleanField
          label={t('audio_configurator.positional.label')}
          id={`audio-positional-box${id}`}
          expanseField={{
            leafPaths: [['positional']],
            diffOptions: {
              defaults: [false],
            },
          }}
          checked={!!audioSettings?.positional}
          onChange={e => onChange(current => current && ({
            ...current,
            positional: e.target.checked,
          }))}
        />
      </div>
      {audioSettings?.positional && (
        <div>
          <RowNumberField
            id={`audio-ref-distance-input${id}`}
            label={t('audio_configurator.ref_distance.label')}
            expanseField={{
              leafPaths: [['refDistance']],
              diffOptions: {
                defaults: [DEFAULT_REF_DISTANCE],
              },
            }}
            value={audioSettings?.refDistance ?? DEFAULT_REF_DISTANCE}
            onChange={value => handleNumberInputChange(value, 'refDistance')}
            min={0}
            step={0.1}
          />
          <RowSelectField
            label={t('audio_configurator.distance_model.label')}
            id={`audio-distance-model-select${id}`}
            expanseField={{
              leafPaths: [['distanceModel']],
              diffOptions: {
                defaults: [DEFAULT_DISTANCE_MODEL],
              },
            }}
            value={distanceModel}
            options={audioOptions}
            onChange={(value: DistanceModel) => {
              onChange((current) => {
                const currentRolloffFactor = current?.rolloffFactor ?? DEFAULT_ROLLOFF_FACTOR
                return current && {
                  ...current,
                  distanceModel: value,
                  rolloffFactor: value === 'linear'
                    ? Math.min(currentRolloffFactor, DEFAULT_ROLLOFF_FACTOR)
                    : current.rolloffFactor,
                }
              })
            }}
          />
          {distanceModel === 'linear' && (
            <RowNumberField
              id={`audio-max-distance-input${id}`}
              label={t('audio_configurator.max_distance.label')}
              expanseField={{
                leafPaths: [['maxDistance']],
                diffOptions: {
                  defaults: [DEFAULT_MAX_DISTANCE],
                },
              }}
              value={audioSettings?.maxDistance ?? DEFAULT_MAX_DISTANCE}
              onChange={value => handleNumberInputChange(value, 'maxDistance')}
              min={1}
              step={1}
            />
          )}
          <RowNumberField
            id={`audio-rolloff-factor-input${id}`}
            label={t('audio_configurator.rolloff_factor.label')}
            expanseField={{
              leafPaths: [['rolloffFactor']],
              diffOptions: {
                defaults: [DEFAULT_ROLLOFF_FACTOR],
              },
            }}
            value={audioSettings?.rolloffFactor ?? DEFAULT_ROLLOFF_FACTOR}
            onChange={value => handleNumberInputChange(value, 'rolloffFactor')}
            min={0}
            step={0.1}
          />
        </div>
      )}
    </>
  )
}

type AudioConfig = DeepReadonly<AudioSettings> | null | undefined

interface IAudioConfigurator {
  audioSettings: AudioConfig
  onChange: (updater: (current: AudioConfig) => AudioConfig) => void
  resetToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

const AudioConfigurator: React.FC<IAudioConfigurator> = (
  {audioSettings, onChange, resetToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const stateCtx = useStudioStateContext()

  const handleSrcChange = (resource: Resource | undefined) => {
    onChange(current => ({
      ...current,
      src: resource,
    }))
  }

  return (
    <ScenePathScopeProvider path={['audio']}>
      <ComponentConfiguratorTray
        title={t('audio_configurator.title')}
        description={t('audio_configurator.description')}
        expanseField={{
          leafPaths: CURRENT_SCOPE,
        }}
        sectionId={AUDIO_COMPONENT}
        onRemove={() => onChange(() => undefined)}
        onCopy={() => copyDirectProperties(stateCtx, {audio: audioSettings})}
        onReset={() => onChange(() => ({}))}
        onResetToPrefab={resetToPrefab ? () => resetToPrefab([GRAPH_AUDIO_COMPONENT]) : undefined}
        componentData={[GRAPH_AUDIO_COMPONENT]}
      >
        <AssetSelector
          label={t('audio_configurator.source.label')}
          assetKind='audio'
          resource={audioSettings?.src}
          onChange={handleSrcChange}
          leafPaths={[['src']]}
        />
        <RowBooleanField
          label={t('audio_configurator.autoplay.label')}
          id='audio-autoplay-checkbox'
          expanseField={{
            leafPaths: [['paused']],
            diffOptions: {
              defaults: [false],
              renderValue: (paused: boolean) => (
                renderValueBoolean(!paused)
              ),
            },
          }}
          checked={!audioSettings?.paused}
          onChange={e => onChange(current => current && ({
            ...current,
            paused: !e.target.checked,
          }))}
        />
        <RowBooleanField
          label={t('audio_configurator.loop.label')}
          id='audio-loop-checkbox'
          expanseField={{
            leafPaths: [['loop']],
            diffOptions: {
              defaults: [false],
            },
          }}
          checked={!!audioSettings?.loop}
          onChange={e => onChange(current => current && ({
            ...current,
            loop: e.target.checked,
          }))}
        />
        <RowNumberField
          id='audio-pitch-input'
          label={t('audio_configurator.pitch.label')}
          expanseField={{
            leafPaths: [['pitch']],
            diffOptions: {
              defaults: [DEFAULT_PITCH],
            },
          }}
          value={audioSettings?.pitch ?? DEFAULT_PITCH}
          min={0}
          step={0.01}
          onChange={pitch => onChange(current => current && ({
            ...current,
            pitch,
          }))}
        />
        <AudioSettingsConfigurator
          audioSettings={audioSettings}
          onChange={onChange}
        />
      </ComponentConfiguratorTray>
    </ScenePathScopeProvider>
  )
}

export {
  AudioConfigurator,
  AudioSettingsConfigurator,
}
