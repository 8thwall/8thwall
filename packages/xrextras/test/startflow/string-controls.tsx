import {h} from 'preact'
import type {FunctionComponent} from 'preact'

import type {StartFlowStrings} from '../../src/startflow/types'
import styles from './debug.module.css'

interface StringOverridesControlProps {
  keys: (keyof StartFlowStrings)[]
  onChange: (key: keyof StartFlowStrings, value: string) => void
  strings: StartFlowStrings
}

const StringOverridesControl: FunctionComponent<StringOverridesControlProps> = ({
  keys, onChange, strings,
}) => (
  <details className={`${styles.fieldset} ${styles.stringOverrides}`}>
    <summary>String overrides</summary>
    {keys.map(key => (
      <label key={key} className={styles.field}>
        <span>{key}</span>
        <input
          type='text'
          aria-label={key}
          value={strings[key]}
          onInput={event => onChange(key, event.currentTarget.value)}
        />
      </label>
    ))}
  </details>
)

export {StringOverridesControl}
