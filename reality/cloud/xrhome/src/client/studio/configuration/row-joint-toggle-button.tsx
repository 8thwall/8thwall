import React from 'react'

import {IJointToggleButton, JointToggleButton} from '../../ui/components/joint-toggle-button'
import {IToggleButtonGroup, ToggleButtonGroup} from '../../ui/components/toggle-button-group'
import {combine} from '../../common/styles'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {DeleteButton} from './row-delete-button'
import {useStyles} from './row-styles'
import type {ExpanseField} from './expanse-field-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import {useIdFallback} from '../use-id-fallback'

interface IRowJointToggleButton<T extends string> extends IJointToggleButton<T> {
  id?: string
  label: React.ReactNode
  leftContent?: React.ReactNode
  rightContent?: React.ReactNode
  onDelete?: () => void
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<T, string[][]>>
}

// eslint-disable-next-line arrow-parens
const RowJointToggleButton = <T extends string>({
  id: idOverride, label, disabled, options, value, leftContent, rightContent, onChange, onDelete,
  expanseField,
}: IRowJointToggleButton<T>) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    // TODO: Switch from label to fieldset and legend for better accessibility for the
    // JointToggleButton, which is a group of buttons, not a single form control.
    <label htmlFor={id} className={classes.row}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={{defaultRenderValue: v => options.find(e => e.value === v)?.content}}
      />}
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          mutedColor
        />
      </div>
      <div className={combine(
        classes.flexItem,
        onDelete && classes.flexItemGroupSpaceBetween,
        classes.rowJointToggleButton
      )}
      >
        {leftContent}
        <JointToggleButton
          options={options}
          value={value}
          disabled={disabled}
          onChange={onChange}
        />
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

interface IRowToggleButtonGroup extends IToggleButtonGroup {
  label: React.ReactNode
  leftContent?: React.ReactNode
  rightContent?: React.ReactNode
  onDelete?: () => void
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<unknown, string[][]>>
}

const RowToggleButtonGroup = ({
  label, disabled, leftContent, rightContent, onDelete, expanseField, ...rest
}: IRowToggleButtonGroup) => {
  const classes = useStyles()

  return (
    <div className={classes.row} role='group'>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
      />}
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          disabled={disabled}
          mutedColor
        />
      </div>
      <div className={combine(
        classes.flexItem,
        onDelete && classes.flexItemGroupSpaceBetween,
        classes.rowJointToggleButton
      )}
      >
        {leftContent}
        <ToggleButtonGroup disabled={disabled} {...rest} />
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </div>
  )
}

export {RowJointToggleButton, RowToggleButtonGroup}
export type {IRowJointToggleButton, IRowToggleButtonGroup}
