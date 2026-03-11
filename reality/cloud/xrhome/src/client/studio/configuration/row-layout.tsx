import React from 'react'

import {useStyles} from './row-styles'
import type {ExpanseField} from './expanse-field-types'
import type {ConfigDiffInfo} from './diff-chip-types'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'

interface IRowLayout {
  children: React.ReactNode
  expanseField?: ExpanseField<ConfigDiffInfo<unknown, string[][]>>
}

const RowLayout: React.FC<IRowLayout> = ({
  children, expanseField,
}) => {
  const styles = useStyles()
  return (
    <div
      className={styles.row}
    >
      {expanseField && <ExpanseFieldDiffChip
        field={expanseField}
      />}
      {children}
    </div>
  )
}

export {
  RowLayout,
}
