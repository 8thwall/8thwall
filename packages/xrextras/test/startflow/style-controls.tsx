import {h} from 'preact'
import type {FunctionComponent} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import styles from './debug.module.css'

const STYLE_FIELDS = [
  {token: '--xrextras-accent-color', defaultValue: '#ffffff', color: true},
  {
    token: '--xrextras-accent-soft-color',
    defaultValue: 'rgba(255, 255, 255, 0.14)',
    color: true,
    pickerDefault: '#626377',
  },
  {token: '--xrextras-accent-text-color', defaultValue: '#2d2e43', color: true},
  {token: '--xrextras-background-blur', defaultValue: '0vmax'},
  {token: '--xrextras-background-color', defaultValue: 'linear-gradient(#464766, #2D2E43)'},
  {token: '--xrextras-background-image', defaultValue: 'none'},
  {
    token: '--xrextras-body-font',
    defaultValue: '\'Nunito-SemiBold\', \'Nunito\', sans-serif',
  },
  {token: '--xrextras-body-font-size', defaultValue: '16px'},
  {token: '--xrextras-border-radius', defaultValue: '12px'},
  {token: '--xrextras-button-border-radius', defaultValue: '999px'},
  {token: '--xrextras-error-color', defaultValue: '#ff8998', color: true},
  {
    token: '--xrextras-error-soft-color',
    defaultValue: 'rgba(255, 80, 105, 0.18)',
    color: true,
    pickerDefault: '#704752',
  },
  {token: '--xrextras-focus-ring-color', defaultValue: '#ffffff', color: true},
  {token: '--xrextras-muted-text-color', defaultValue: '#d1d1d8', color: true},
  {
    token: '--xrextras-neutral-soft-color',
    defaultValue: 'rgba(255, 255, 255, 0.12)',
    color: true,
    pickerDefault: '#5e5f74',
  },
  {token: '--xrextras-panel-backdrop-filter', defaultValue: 'none'},
  {
    token: '--xrextras-panel-background-color',
    defaultValue: 'transparent',
    color: true,
    pickerDefault: '#56576d',
  },
  {token: '--xrextras-panel-border-radius', defaultValue: '0px'},
  {token: '--xrextras-panel-shadow', defaultValue: 'none'},
  {token: '--xrextras-preview-aspect-ratio', defaultValue: '16 / 9'},
  {
    token: '--xrextras-spinner-track-color',
    defaultValue: 'rgba(255, 255, 255, 0.24)',
    color: true,
    pickerDefault: '#77788a',
  },
  {token: '--xrextras-text-color', defaultValue: '#ffffff', color: true},
  {token: '--xrextras-text-shadow', defaultValue: 'none'},
  {
    token: '--xrextras-title-font',
    defaultValue: '\'Nunito-SemiBold\', \'Nunito\', sans-serif',
  },
  {token: '--xrextras-title-font-size', defaultValue: '28px'},
] as const

type StyleToken = typeof STYLE_FIELDS[number]['token']
type StyleOverrides = Partial<Record<StyleToken, string>>
type StyleField = typeof STYLE_FIELDS[number]

const THEME_PRESETS: {label: string, values: StyleOverrides}[] = [
  {label: 'Landing', values: {}},
  {
    label: 'Light',
    values: {
      '--xrextras-background-color': '#f5f5f7',
      '--xrextras-panel-background-color': '#ffffff',
      '--xrextras-text-color': '#17171b',
      '--xrextras-muted-text-color': '#65656f',
      '--xrextras-accent-color': '#5b55e7',
      '--xrextras-accent-text-color': '#ffffff',
      '--xrextras-accent-soft-color': '#e9e8fc',
      '--xrextras-error-color': '#df3b4f',
      '--xrextras-error-soft-color': '#fbe7e9',
      '--xrextras-neutral-soft-color': '#e8e8e9',
      '--xrextras-spinner-track-color': '#ddddf9',
      '--xrextras-focus-ring-color': '#aaa7f4',
      '--xrextras-panel-backdrop-filter': 'none',
      '--xrextras-panel-border-radius': '24px',
      '--xrextras-panel-shadow': '0 18px 55px rgba(27, 27, 45, 0.13)',
    },
  },
  {
    label: 'Midnight',
    values: {
      '--xrextras-background-color': 'linear-gradient(#172033, #080b12)',
      '--xrextras-panel-background-color': 'rgba(15, 23, 42, 0.78)',
      '--xrextras-text-color': '#f8fafc',
      '--xrextras-muted-text-color': '#b6c2d2',
      '--xrextras-accent-color': '#67e8f9',
      '--xrextras-accent-text-color': '#083344',
      '--xrextras-accent-soft-color': 'rgba(103, 232, 249, 0.14)',
      '--xrextras-neutral-soft-color': 'rgba(255, 255, 255, 0.09)',
      '--xrextras-spinner-track-color': 'rgba(103, 232, 249, 0.24)',
      '--xrextras-focus-ring-color': '#67e8f9',
      '--xrextras-panel-border-radius': '24px',
      '--xrextras-panel-backdrop-filter': 'blur(18px)',
      '--xrextras-panel-shadow': '0 22px 70px rgba(0, 0, 0, 0.42)',
    },
  },
]

interface StyleOverrideFieldProps {
  field: StyleField
  override?: string
  onChange: (token: StyleToken, value: string, defaultValue: string) => void
}

const StyleOverrideField: FunctionComponent<StyleOverrideFieldProps> = ({
  field, override, onChange,
}) => {
  const inputId = `style-${field.token.slice(2)}`
  const effectiveValue = override || field.defaultValue
  const [draft, setDraft] = useState(effectiveValue)
  const supportsColor = 'color' in field && field.color
  const pickerDefault = 'pickerDefault' in field ? field.pickerDefault : field.defaultValue
  const pickerValue = /^#[0-9a-f]{6}$/i.test(draft) ? draft : pickerDefault

  useEffect(() => {
    setDraft(effectiveValue)
  }, [effectiveValue])

  const updateText = (value: string) => {
    setDraft(value)
    if (value.trim()) {
      onChange(field.token, value, field.defaultValue)
    }
  }

  const handleBlur = () => {
    if (!draft.trim()) {
      setDraft(field.defaultValue)
      onChange(field.token, field.defaultValue, field.defaultValue)
    }
  }

  const updateColor = (value: string) => {
    setDraft(value)
    onChange(field.token, value, field.defaultValue)
  }

  return (
    <div className={styles.field}>
      <label htmlFor={inputId}>{field.token}</label>
      <div className={styles.styleInput}>
        <input
          id={inputId}
          type='text'
          value={draft}
          onInput={event => updateText(event.currentTarget.value)}
          onBlur={handleBlur}
        />
        {supportsColor && (
          <input
            className={styles.colorPicker}
            type='color'
            aria-label={`${field.token} color picker`}
            value={pickerValue}
            onInput={event => updateColor(event.currentTarget.value)}
          />
        )}
      </div>
    </div>
  )
}

interface StyleOverridesControlProps {
  onChange: StyleOverrideFieldProps['onChange']
  onPreset: (values: StyleOverrides) => void
  onReset: () => void
  overrides: StyleOverrides
}

const StyleOverridesControl: FunctionComponent<StyleOverridesControlProps> = ({
  onChange, onPreset, onReset, overrides,
}) => (
  <details className={`${styles.fieldset} ${styles.styleOverrides}`}>
    <summary>Style overrides</summary>
    <div className={styles.presetGroup}>
      <span className={styles.presetLabel}>Theme presets</span>
      <div className={styles.presetButtons}>
        {THEME_PRESETS.map(({label, values}) => (
          <button key={label} type='button' onClick={() => onPreset(values)}>
            {label}
          </button>
        ))}
      </div>
    </div>
    {STYLE_FIELDS.map(field => (
      <StyleOverrideField
        key={field.token}
        field={field}
        override={overrides[field.token]}
        onChange={onChange}
      />
    ))}
    <button className={styles.resetButton} type='button' onClick={onReset}>
      Reset styles
    </button>
  </details>
)

export type {StyleOverrides, StyleToken}
export {STYLE_FIELDS, StyleOverridesControl}
