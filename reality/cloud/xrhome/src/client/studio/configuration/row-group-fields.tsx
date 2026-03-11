import React from 'react'

import {useStyles} from './row-styles'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {combine} from '../../common/styles'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import type {ConfigDiffInfo} from './diff-chip-types'
import type {ExpanseField} from './expanse-field-types'

interface IRowGroupFields {
  label: React.ReactNode
  children: React.ReactNode
  contentFlexDirection?: 'row' | 'column'
  formatLabel?: boolean
  expanseField?: ExpanseField<ConfigDiffInfo<unknown, string[][]>>
}

const RowGroupFields: React.FC<IRowGroupFields> = ({
  label, children, formatLabel = true, contentFlexDirection = 'row',
  expanseField,
}) => {
  const classes = useStyles()

  return (
    <fieldset className={combine('style-reset', classes.row, classes.rowAlignTop)}>
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
      />}
      {formatLabel
        ? (
          <div className={classes.flexItem}>
            <StandardFieldLabel
              label={label}
              mutedColor
            />
          </div>
        )
        : label}
      <div
        className={combine(
          contentFlexDirection === 'row' ? classes.rowGroup : classes.columnGroup, classes.flexItem,
          classes.rowAlignTop
        )}
      >
        {children}
      </div>
    </fieldset>
  )
}

export {RowGroupFields}
