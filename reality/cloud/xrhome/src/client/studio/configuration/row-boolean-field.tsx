import React from 'react'

import type {IStandardCheckboxField} from '../../ui/components/standard-checkbox-field'
import {combine} from '../../common/styles'
import {FloatingTrayCheckboxInput} from '../../ui/components/floating-tray-checkbox-input'
import {RowFieldLabel} from './row-field-label'
import {DeleteButton} from './row-delete-button'
import {useStyles} from './row-styles'
import {
  makeRenderValueGroup,
  renderValueBoolean,
} from './diff-chip-default-renderers'
import type {ExpanseField} from './expanse-field-types'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import {useIdFallback} from '../use-id-fallback'
import {createThemedStyles} from '../../ui/theme'

interface IRowBooleanField extends IStandardCheckboxField {
  disabled?: boolean
  tag?: React.ReactNode
  a8?: string
  checkBoxAlign?: 'left' | 'right'
  onDelete?: () => void
  noMargin?: boolean
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<boolean, string[][]>>
}

const RowBooleanField: React.FC<IRowBooleanField> = ({
  id: idOverride, label, checked, onChange, disabled, tag, a8, checkBoxAlign = 'left', onDelete,
  noMargin, expanseField,
}) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    // TODO(Carson): Fix htmlFor bug, since there are multiple with same id.
    // eslint-disable-next-line jsx-a11y/label-has-associated-control
    <label htmlFor={id} className={combine(classes.row, noMargin && classes.noMargin)}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={{defaultRenderValue: renderValueBoolean}}
      />}
      <div className={classes.flexItem}>
        <RowFieldLabel
          label={label}
          disabled={disabled}
          expanseField={expanseField}
        />
      </div>
      <div
        className={combine(
          classes.flexItem,
          onDelete && classes.flexItemGroupSpaceBetween,
          checkBoxAlign === 'right' && classes.reverseRow
        )}
      >
        <FloatingTrayCheckboxInput
          a8={a8}
          id={id}
          checked={checked}
          onChange={onChange}
          disabled={disabled}
          tag={tag}
        />
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

const RowBooleanFields: React.FC<{
  label: string
  fields: IStandardCheckboxField[]
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<boolean[], string[][]>>
}> = (
  {label, fields, expanseField}
) => {
  const classes = useStyles()

  const defaultRenderValue = makeRenderValueGroup(
    fields.map(field => field.label),
    fields.map(() => renderValueBoolean)
  )

  if (!fields.length) {
    return null
  }

  return (
    <div className={classes.row}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
        defaultRenderers={{defaultRenderValue}}
      />}
      <div className={classes.flexItem}>
        <RowFieldLabel
          label={label}
          expanseField={expanseField}
        />
      </div>
      <div className={combine(classes.rowGroup, classes.flexItem)}>
        {fields.map(field => (
          <label key={field.id} htmlFor={field.id} className={classes.checkboxesRow}>
            <FloatingTrayCheckboxInput
              id={field.id}
              checked={field.checked}
              onChange={field.onChange}
            />
            <span className={classes.checkboxLabel}>{field.label} </span>
          </label>
        ))}
      </div>
    </div>
  )
}

const useBooleanFieldsStyles = createThemedStyles(() => ({
  grid: {
    display: 'grid',
    gap: '0.5em',
    gridTemplateColumns: 'repeat(auto-fit, minmax(12rem, 1fr))',
    alignItems: 'center',
  },
}))

const BooleanFields: React.FC<{
  fields: IStandardCheckboxField[]
}> = ({fields}) => {
  const classes = useStyles()
  const booleanFieldsClasses = useBooleanFieldsStyles()

  return (
    <div className={booleanFieldsClasses.grid}>
      {fields.map(field => (
        <label key={field.id} htmlFor={field.id} className={classes.checkboxesRow}>
          <FloatingTrayCheckboxInput
            id={field.id}
            checked={field.checked}
            onChange={field.onChange}
          />
          <span className={classes.checkboxLabel}>{field.label} </span>
        </label>
      ))}
    </div>
  )
}

export {RowBooleanField, RowBooleanFields, BooleanFields}
export type {IRowBooleanField}
