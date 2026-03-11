import React from 'react'
import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'
import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'
import {hexColorWithAlpha} from '../../../shared/colors'
import {CREDIT_GRANT_FEATURE} from '../../../shared/feature-config'

const useStyles = createThemedStyles(theme => ({
  intervalContainer: {
    padding: '0 0.75em',
    display: 'flex',
    alignItems: 'center',
    borderRadius: '1em',
    gap: '0.25em',
    fontSize: '14px',
  },
  enterprise: {
    'border': `1px solid ${hexColorWithAlpha(theme.primaryBtnBg, 0.5)}`,
    'color': theme.primaryBtnBg,
    '& svg': {
      fill: theme.primaryBtnBg,
    },
  },
  power: {
    'border': `1px solid ${hexColorWithAlpha(theme.primaryBtnBg, 0.5)}`,
    'color': theme.primaryBtnBg,
    '& svg': {
      fill: theme.primaryBtnBg,
    },
  },
  core: {
    'border': `1px solid ${hexColorWithAlpha(theme.badgeMintColor, 0.5)}`,
    'color': theme.badgeMintColor,
    '& svg': {
      fill: theme.badgeMintColor,
    },
  },
  free: {
    'border': `1px solid ${hexColorWithAlpha(theme.fgMuted, 0.5)}`,
    'color': theme.fgMuted,
    '& svg': {
      fill: theme.fgMuted,
    },
  },
}))

interface ICreditDisplayInterval {
  planName: string
  creditAmount?: number
}

const CreditDisplayInterval: React.FC<ICreditDisplayInterval> = ({
  planName, creditAmount,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['account-pages'])

  const isEnterprise = planName === CREDIT_GRANT_FEATURE.Enterprise.planName
  const isPower = planName === CREDIT_GRANT_FEATURE.PowerSub.planName
  const isCore = planName === CREDIT_GRANT_FEATURE.CoreSub.planName
  const isFree = planName === CREDIT_GRANT_FEATURE.Free.planName

  return (
    <div
      className={combine(classes.intervalContainer, isEnterprise && classes.enterprise,
        isPower && classes.power, isCore && classes.core, isFree && classes.free)}
    >
      <Icon stroke='creditsBold' />
      {isEnterprise
        ? t('plan_billing_page.credit_display_interval.custom')
        : t('plan_billing_page.credit_display_interval', {creditAmount})
      }
    </div>
  )
}

export {CreditDisplayInterval}
