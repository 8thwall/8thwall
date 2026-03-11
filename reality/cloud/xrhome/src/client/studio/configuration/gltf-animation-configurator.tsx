import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import type {GltfModel} from '@ecs/shared/scene-graph'

import {PLAY_ALL_ANIMATIONS_KEY} from '@ecs/shared/gltf-constants'

import {useGltfWithDraco} from '../hooks/gltf'

import {RowSelectField, RowBooleanField, RowNumberField} from './row-fields'

type Model = DeepReadonly<GltfModel> | null | undefined

interface IGltfAnimationConfigurator {
  url: string
  gltfModel: Model
  onChange: (updater: (current: Model) => Model) => void
}

const GltfAnimationConfigurator: React.FC<IGltfAnimationConfigurator> = ({
  url, gltfModel, onChange,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const visibleAnimationClip = gltfModel?.animationClip || ''
  const gltf = useGltfWithDraco(url)

  const animationClips = gltf?.animations?.map(clip => clip.name)

  return (
    <div>
      {animationClips?.length > 0 && gltfModel &&
        <div>
          <RowSelectField
            label={t('gltf_animation_configurator.animation_clip.label')}
            id='animation-clip'
            value={visibleAnimationClip}
            options={[
              {value: '', content: t('gltf_animation_configurator.animation_clip.option.none')},
              {
                value: PLAY_ALL_ANIMATIONS_KEY,
                content: t('gltf_animation_configurator.animation_clip.option.play_all'),
              },
              ...animationClips.map(clip => ({value: clip, content: clip})),
            ].filter(Boolean)}
            onChange={value => onChange(current => current && ({
              ...current,
              animationClip: value,
            }))}
          />
          <RowBooleanField
            label={t('gltf_animation_configurator.pause_enable_box.label')}
            id='pause-enable-box'
            checked={!!gltfModel.paused}
            onChange={() => onChange(current => current && ({
              ...current,
              paused: !current.paused,
            }))}
          />
          <RowBooleanField
            label={t('gltf_animation_configurator.loop_enable_box.label')}
            id='loop-enable-box'
            checked={!!gltfModel.loop}
            onChange={() => onChange(current => current && ({
              ...current,
              loop: !current.loop,
            }))}
          />
          {gltfModel.loop &&
            <>
              <RowBooleanField
                label={t('gltf_animation_configurator.reverse_enable_box.label')}
                id='reverse-enable-box'
                checked={!!gltfModel.reverse}
                onChange={() => onChange(current => current && ({
                  ...current,
                  reverse: !current.reverse,
                }))}
              />
              <RowBooleanField
                label={t('gltf_animation_configurator.repeat_forever.label')}
                id='repeat-forever-enable-box'
                checked={!gltfModel.repetitions}
                onChange={() => onChange(current => current && ({
                  ...current,
                  repetitions: current.repetitions ? undefined : 1,
                }))}
              />
              {!!gltfModel.repetitions &&
                <RowNumberField
                  label={t('gltf_animation_configurator.repetitions.label')}
                  id='repetitions'
                  value={gltfModel.repetitions ?? 1}
                  min={1}
                  step={1}
                  normalizer={Math.trunc}
                  onChange={value => onChange(current => current && ({
                    ...current,
                    repetitions: value,
                  }))}
                />
              }
            </>
          }
          <RowNumberField
            label={t('gltf_animation_configurator.timescale.label')}
            id='timeScale'
            value={gltfModel.timeScale ?? 1}
            step={0.1}
            onChange={value => onChange(current => current && ({
              ...current,
              timeScale: value,
            }))}
          />

          {gltfModel.animationClip !== PLAY_ALL_ANIMATIONS_KEY &&
            <RowNumberField
              label={t('gltf_animation_configurator.cross_fade_duration.label')}
              id='cross-fade-duration'
              value={gltfModel.crossFadeDuration ?? 0}
              min={-1}
              step={0.1}
              onChange={value => onChange(current => current && ({
                ...current,
                crossFadeDuration: value,
              }))}
            />
          }
        </div>
      }
    </div>
  )
}

export {
  GltfAnimationConfigurator,
}
