import React from 'react'

import {bool, combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  creditLabel: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    backgroundColor: theme.primaryBtnHoverBg,
    borderRadius: '6px',
    padding: '0.25rem 0.5rem',
  },
  creditLabelDisabled: {
    backgroundColor: `${theme.primaryBtnDisabledBgOpaque} !important`,
  },
}))

interface ICreditLabel {
  text: string
  disabled?: boolean
}

const CreditLabel: React.FC<ICreditLabel> = ({disabled, text}) => {
  const classes = useStyles()

  return (
    <div className={combine(classes.creditLabel, bool(disabled, classes.creditLabelDisabled))}>
      <Icon stroke='credits' />{text}
    </div>
  )
}

export {CreditLabel}
