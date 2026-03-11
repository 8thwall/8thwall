import React from 'react'
import {formatColor, parseColor} from '@ecs/shared/color'

import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {combine} from '../../common/styles'
import {useEphemeralEditState} from './ephemeral-edit-state'
import {useStyles} from './row-styles'
import {handleBlurInputField} from './handle-blur-input-field'
import {DeleteButton} from './row-delete-button'
import {renderValueColor} from './diff-chip-default-renderers'
import type {ExpanseField} from './expanse-field-types'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import {useIdFallback} from '../use-id-fallback'

interface IRowColorField {
  id?: string
  label: React.ReactNode
  value: string
  onChange: (newValue: string) => void
  noMargin?: boolean
  rightContent?: React.ReactNode
  onDelete?: () => void
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<string, string[][]>>
}

interface IColorPicker extends Pick<IRowColorField, 'value' | 'onChange'> {
  id: string
  ariaLabel?: string
  hideInput?: boolean
  defaultValue?: string
}

const ColorPicker: React.FC<IColorPicker> = ({
  id, value, onChange, ariaLabel, hideInput, defaultValue,
}) => {
  const classes = useStyles()

  const {editValue, setEditValue, clear} = useEphemeralEditState({
    value,
    deriveEditValue: v => v.toUpperCase().replace(/^#/, ''),
    parseEditValue: (v: string) => {
      if (!v) {
        if (defaultValue !== undefined) {
          return [true, defaultValue] as const
        }
        return [false]
      }
      const hex = formatColor(v)
      if (!parseColor(hex)) {
        return [false]
      }
      return [true, hex]
    },
    onChange,
  })

  return (
    <div className={!hideInput ? classes.flexItem : undefined}>
      <StandardFieldContainer>
        <div className={classes.colorPickerContainer}>
          <input
            id={id}
            type='color'
            value={value}
            onChange={e => onChange(e.target.value)}
            className={combine('style-reset', classes.swatch)}
          />
          {!hideInput && <input
            type='text'
            value={editValue}
            aria-label={ariaLabel}
            onChange={e => setEditValue(e.target.value)}
            className={combine(classes.input, classes.subInput)}
            onBlur={clear}
            onKeyDown={handleBlurInputField}
          />}
        </div>
      </StandardFieldContainer>
    </div>
  )
}

const RowColorField: React.FC<IRowColorField> = ({
  id: idOverride, label, value, onChange, noMargin, rightContent, onDelete,
  expanseField,
}) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    <label htmlFor={id} className={combine(noMargin && classes.noMargin, classes.row)}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={{defaultRenderValue: renderValueColor}}
      />}
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          mutedColor
        />
      </div>
      <div className={combine(classes.flexItem, classes.flexItemGroup)}>
        <ColorPicker
          id={id}
          value={value}
          onChange={onChange}
        />
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

export {RowColorField, ColorPicker}
export type {IRowColorField, IColorPicker}
