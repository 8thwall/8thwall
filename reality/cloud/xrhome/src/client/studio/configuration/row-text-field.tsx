import React from 'react'

import type {IStandardTextField} from '../../ui/components/standard-text-field'
import {useStyles} from './row-styles'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {combine} from '../../common/styles'
import {handleBlurInputField} from './handle-blur-input-field'
import {DeleteButton} from './row-delete-button'
import type {ExpanseField} from './expanse-field-types'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import {useIdFallback} from '../use-id-fallback'
import {RowFieldLabel} from './row-field-label'

interface IRowTextField extends IStandardTextField {
  icon?: React.ReactNode
  onDelete?: () => void
  invalid?: boolean
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<string, string[][]>>
}

const RowTextField: React.FC<IRowTextField> = ({
  id: idOverride, label, disabled, icon, onDelete, invalid,
  expanseField, ...rest
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  return (
    <label htmlFor={id} className={classes.row}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={{defaultRenderValue: v => v}}
      />}
      <div className={classes.flexItem}>
        <RowFieldLabel
          label={label}
          disabled={disabled}
          expanseField={expanseField}
        />
      </div>
      <div className={combine(classes.flexItem, onDelete && classes.flexItemGroupSpaceBetween)}>
        <StandardFieldContainer disabled={disabled} invalid={invalid}>
          <div className={icon && classes.textIconInputContainer}>
            {icon}
            <input
              {...rest}
              id={id}
              type='text'
              className={classes.input}
              disabled={disabled}
              onFocus={e => e.target.select()}
              onKeyDown={handleBlurInputField}
            />
          </div>
        </StandardFieldContainer>
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

export {RowTextField}
export type {IRowTextField}
