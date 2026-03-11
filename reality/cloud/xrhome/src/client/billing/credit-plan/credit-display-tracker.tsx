import React from 'react'
import {useTranslation, Trans} from 'react-i18next'

import {mobileViewOverride} from '../../static/styles/settings'
import {Icon} from '../../ui/components/icon'
import {Loader} from '../../ui/components/loader'
import {createThemedStyles} from '../../ui/theme'
import {useCreditQuery} from '../use-credit-query'
import {convertBipsToCredits} from '../../../shared/feature-utils'
import type {Feature} from '../../common/types/db'

const useStyles = createThemedStyles(theme => ({
  creditTrackerContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '1em',
    [mobileViewOverride]: {
      alignSelf: 'flex-start',
    },
  },
  mainCreditTracker: {
    padding: '0.75em 1.25em',
    display: 'flex',
    flexDirection: 'column',
    borderRadius: '1em',
    gap: '0.25em',
    backgroundColor: theme.tabPaneBg,
    color: theme.controlActive,
    minHeight: '4.7em',
    justifyContent: 'center',
  },
  tooltipCreditTracker: {
    display: 'flex',
    flexDirection: 'row',
    color: theme.fgMain,
    alignItems: 'center',
    gap: '8px',
    fontWeight: 700,
  },
  insufficientCreditTracker: {
    display: 'inline-flex',
    flexDirection: 'column',
    justifyContent: 'center',
    alignItems: 'flex-start',
    gap: '2px',
    color: theme.sfcDisabledColor,
  },
  mainCreditAmountContainer: {
    fontSize: '1.125em',
    display: 'flex',
    alignItems: 'center',
    gap: '0.25em',
  },
  mainCreditAmount: {
    fontSize: '1.25em',
    fontWeight: 700,
  },
  mainCreditDate: {
    'color': theme.creditTrackerFg,
    'display': 'flex',
    'gap': '0.25em',
    'alignItems': 'center',
    '& svg': {
      fill: theme.creditTrackerFg,
    },
  },
  mainCreditExpiration: {
    'color': theme.fgMuted,
    'display': 'flex',
    'alignItems': 'center',
    'gap': '4px',
    '& svg': {
      fill: theme.fgMuted,
    },
  },
  topUpCreditContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.25em',
    color: theme.fgMuted,
  },
  topUpCreditTracker: {
    fontStyle: 'italic',
  },
}))

interface ICreditDisplayTracker {
  activeCreditGrant?: Feature
  showAmount?: boolean
  tooltip?: boolean
  hasSufficientCredit?: boolean
}

const CreditDisplayTracker: React.FC<ICreditDisplayTracker> = ({
  activeCreditGrant, showAmount = true, tooltip, hasSufficientCredit = true,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['account-pages'])
  const {status, data} = useCreditQuery()
  const showMainCreditTracker = !!activeCreditGrant || showAmount  // Otherwise nothing to show

  if (status !== 'success') {
    return (
      <div className={classes.creditTrackerContainer}>
        <div className={classes.mainCreditTracker}>
          <Loader inline />
        </div>
      </div>
    )
  }

  const creditRefreshDate =
    data.categorizedCreditGrants.PAID_PLAN?.slice().reverse().find(g => g.expiresAt).expiresAt ??
      data.categorizedCreditGrants.FREE_STANDARD?.slice().reverse().find(g => g.expiresAt).expiresAt

  const creditGrants = Object.entries(data.categorizedCreditGrants)
    .flatMap(([, grants]) => grants.filter(g => g.remainingQuantity > 0))

  const CreditDate = ({date}: {date: Date}) => (
    date &&
      <div className={classes.mainCreditDate}>
        <Icon stroke='clockOutline' />
        {t(
          'plan_billing_page.credit_display_tracker.remaining_credits_main_expiration',
          {expirationDate: date.toLocaleDateString()}
        )}
      </div>
  )

  if (tooltip) {
    return (
      hasSufficientCredit
        ? (
          <div className={classes.tooltipCreditTracker}>
            <Icon stroke='creditsBold' />
            {t(
              'plan_billing_page.credit_display_tracker.remaining_credits_main_count.tooltip',
              {creditAmount: data.creditAmount}
            )}
          </div>)
        : (
          <div className={classes.insufficientCreditTracker}>
            <div className={classes.tooltipCreditTracker}>
              <Icon stroke='creditsBold' />
              {t('plan_billing_page.credit_display_tracker.insufficient_credits')}
            </div>
            <div className={classes.mainCreditExpiration}>
              <CreditDate date={new Date(creditRefreshDate)} />
            </div>
          </div>
        )
    )
  }

  const getMainCreditDate = () => {
    if (activeCreditGrant?.subscriptionEndsAt) {
      return (
        <div className={classes.mainCreditDate}>
          <Icon stroke='clockOutline' />
          {t(
            'plan_billing_page.credit_display_tracker.credit_plan_main_end_date',
            {
              endDate: new Date(activeCreditGrant.subscriptionEndsAt).toLocaleDateString(),
            }
          )}
        </div>
      )
    }

    if (creditRefreshDate) {
      return (
        <CreditDate date={new Date(creditRefreshDate)} />
      )
    }

    return null
  }

  return (
    <div className={classes.creditTrackerContainer}>
      {showMainCreditTracker &&
        <div className={classes.mainCreditTracker}>
          {showAmount &&
            <div className={classes.mainCreditAmountContainer}>
              <Icon stroke='creditsBold' size={1.25} />
              <div>
                <Trans
                  ns='account-pages'
                  i18nKey='plan_billing_page.credit_display_tracker.remaining_credits_main_count'
                  components={{
                    1: <span className={classes.mainCreditAmount} />,
                  }}
                  values={{creditAmount: data.creditAmount}}
                />
              </div>
            </div>
          }
          {getMainCreditDate()}
        </div>
      }
      <div className={classes.topUpCreditContainer}>
        {creditGrants.map(grant => (
          <div key={grant.uuid} className={classes.topUpCreditTracker}>
            {t(
              'plan_billing_page.credit_display_tracker.remaining_credits_topup',
              {
                creditAmount: convertBipsToCredits(grant.remainingQuantity),
                expirationDate: new Date(grant.expiresAt).toLocaleDateString(),
              }
            )}
          </div>
        ))}
      </div>
    </div>
  )
}

export {CreditDisplayTracker}
