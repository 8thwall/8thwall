import React from 'react'

import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {ExpanseFieldTooltip, useTooltipData} from './expanse-field-tooltip'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import type {ExpanseField} from './expanse-field-types'

interface IExpanseFieldLabel {
  expanseField: ExpanseField<DefaultableConfigDiffInfo<unknown, string[][]>>
  children: React.ReactNode
}

const ExpanseFieldLabel: React.FC<IExpanseFieldLabel> = ({children, expanseField}) => {
  const tooltip = useTooltipData(expanseField)

  if (tooltip) {
    return (
      <ExpanseFieldTooltip
        data={tooltip}
      >
        {children}
      </ExpanseFieldTooltip>
    )
  } else {
    // eslint-disable-next-line react/jsx-no-useless-fragment
    return <>{children}</>
  }
}

interface IRowFieldLabel {
  label: React.ReactNode
  expanseField: IExpanseFieldLabel['expanseField'] | undefined
  disabled?: boolean
  starred?: boolean
}

const RowFieldLabel: React.FC<IRowFieldLabel> = ({expanseField, disabled, label, starred}) => {
  const inner = (
    <StandardFieldLabel
      label={label}
      disabled={disabled}
      starred={starred}
      mutedColor
    />
  )
  if (BuildIf.CONFIGURATOR_TOOLTIPS_20250808 && expanseField) {
    return (
      <ExpanseFieldLabel
        expanseField={expanseField}
      >
        {inner}
      </ExpanseFieldLabel>
    )
  } else {
    return inner
  }
}

export {
  RowFieldLabel,
}
