import {h} from 'preact'
import type {FunctionComponent} from 'preact'

import {DEFAULT_PREVIEW_PATH} from './debug-config'
import styles from './debug.module.scss'
import type {DebugLandingOptions} from './debug-types'

type TextOption = 'url' | 'mediaSrc' | 'logoSrc' | 'mediaAnimation' | 'sceneEnvMap'
type ChangeOption = <K extends keyof DebugLandingOptions>(
  key: K, value: DebugLandingOptions[K]
) => void

interface Props {
  legend: string
  onChange: ChangeOption
  onPreset: (options: Partial<DebugLandingOptions>) => void
  options: DebugLandingOptions
}

const TEXT_FIELDS: {key: TextOption, label: string, type?: string}[] = [
  {key: 'url', label: 'Experience URL', type: 'url'},
  {key: 'mediaSrc', label: 'Media URL', type: 'url'},
  {key: 'logoSrc', label: 'Logo URL', type: 'url'},
  {key: 'mediaAnimation', label: 'Model animation'},
  {key: 'sceneEnvMap', label: 'Model environment map'},
]

const PRESETS: {label: string, options: Partial<DebugLandingOptions>}[] = [
  {label: 'Blank', options: {}},
  {label: 'Image', options: {mediaSrc: DEFAULT_PREVIEW_PATH}},
  {
    label: 'Video',
    options: {mediaSrc: 'https://cdn.8thwall.com/web/assets/beach.mp4'},
  },
  {
    label: 'YouTube',
    options: {mediaSrc: 'https://www.youtube.com/watch?v=qFLhGq0060w'},
  },
  {
    label: '3D model',
    options: {
      mediaSrc: 'https://static.8thwall.app/assets/robot-i1wciu9sai.glb',
      mediaAnimation: 'ThumbsUp',
    },
  },
]

const LandingOptionsControl: FunctionComponent<Props> = ({
  legend, onChange, onPreset, options,
}) => (
  <fieldset className={styles.fieldset}>
    <legend>{legend}</legend>
    <div className={styles.presetGroup}>
      <span className={styles.presetLabel}>Presets</span>
      <div className={styles.presetButtons}>
        {PRESETS.map(preset => (
          <button
            key={preset.label}
            type='button'
            onClick={() => onPreset(preset.options)}
          >
            {preset.label}
          </button>
        ))}
      </div>
    </div>
    {TEXT_FIELDS.map(({key, label, type}) => (
      <label key={key} className={styles.field}>
        <span>{label}</span>
        <input
          type={type || 'text'}
          value={options[key]}
          onInput={event => onChange(key, event.currentTarget.value)}
        />
      </label>
    ))}
    <label className={styles.field}>
      <span>Video controls</span>
      <select
        value={options.mediaControls}
        onChange={event => onChange(
          'mediaControls', event.currentTarget.value as DebugLandingOptions['mediaControls']
        )}
      >
        <option value='minimal'>Minimal</option>
        <option value='browser'>Browser</option>
        <option value='none'>None</option>
      </select>
    </label>
    <label className={styles.field}>
      <span>Idle orbit</span>
      <select
        value={options.sceneOrbitIdle}
        onChange={event => onChange(
          'sceneOrbitIdle', event.currentTarget.value as DebugLandingOptions['sceneOrbitIdle']
        )}
      >
        <option value='spin'>Spin</option>
        <option value='bounce'>Bounce</option>
        <option value='none'>None</option>
      </select>
    </label>
    <label className={styles.field}>
      <span>Model interaction</span>
      <select
        value={options.sceneOrbitInteraction}
        onChange={event => onChange(
          'sceneOrbitInteraction',
          event.currentTarget.value as DebugLandingOptions['sceneOrbitInteraction']
        )}
      >
        <option value='drag'>Drag</option>
        <option value='none'>None</option>
      </select>
    </label>
    <label className={styles.field}>
      <span>Lighting intensity</span>
      <input
        type='number'
        step='0.1'
        value={options.sceneLightingIntensity}
        onInput={event => onChange(
          'sceneLightingIntensity', event.currentTarget.valueAsNumber || 0
        )}
      />
    </label>
    <label className={styles.checkboxField}>
      <input
        type='checkbox'
        checked={options.mediaAutoplay}
        onChange={event => onChange('mediaAutoplay', event.currentTarget.checked)}
      />
      <span>Autoplay media</span>
    </label>
  </fieldset>
)

export {LandingOptionsControl}
