import React from 'react'
import {useTranslation} from 'react-i18next'
import {Link} from 'react-router-dom'

import {createThemedStyles} from '../ui/theme'
import {Icon} from '../ui/components/icon'
import {CreditDisplayInterval} from '../billing/credit-plan/credit-display-interval'
import {useStudioMenuStyles} from '../studio/ui/studio-menu-styles'
import {SelectMenu} from '../studio/ui/select-menu'
import {combine} from '../common/styles'
import useCurrentAccount from '../common/use-current-account'
import {CreditDisplayTracker} from '../billing/credit-plan/credit-display-tracker'
import {CREDIT_GRANT_FEATURE} from '../../shared/feature-config'
import {getActiveCreditGrant} from '../../shared/feature-utils'
import {useCreditQuery} from '../billing/use-credit-query'

const useStyles = createThemedStyles(theme => ({
  creditRemaining: {
    flex: '0 0 auto',
    textAlign: 'right',
    color: theme.fgMuted,
    padding: '0.425em 0.75em',
    background: theme.tabPaneBg,
    borderRadius: '0.5em',
    border: `1px solid ${theme.studioAssetBorder}`,
  },
  creditTrackerContainer: {
    zIndex: '100 !important',
    fontSize: '14px',
    padding: '1.25em 1em',
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.75em',
    color: theme.fgMain,
    userSelect: 'none',
  },
  creditTracker: {
    flex: '1 1 0',
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'flex-end',
  },
  creditTrackerHeader: {
    display: 'flex',
    gap: '0.5em',
    alignItems: 'center',
  },
  creditPlanName: {
    textTransform: 'uppercase',
    fontSize: '1.25em',
    fontWeight: 700,
  },
  creditTrackerLink: {
    'display': 'flex',
    'alignItems': 'center',
    'color': theme.fgMuted,
    'textDecoration': 'none',
    '&:hover': {
      color: theme.fgMain,
    },
  },
}))

const AssetLabCreditTracker: React.FC = () => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const {t} = useTranslation(['asset-lab', 'common'])
  const account = useCurrentAccount()
  const activeCreditGrant = getActiveCreditGrant(account.Features)
  const activeCreditGrantName = activeCreditGrant?.optionName || CREDIT_GRANT_FEATURE.Free.name
  const {status, data} = useCreditQuery()

  if (status !== 'success') {
    return <div className={classes.creditTracker} />
  }

  const creditTrackerSelect = (
    <div className={classes.creditRemaining}>
      <Icon stroke='credits' inline />
      {t('asset_lab.credits_remaining', {count: data.creditAmount})}
    </div>
  )

  return (
    <div className={classes.creditTracker}>
      <SelectMenu
        id='asset-lab-credit-tracker-dropdown'
        menuWrapperClassName={combine(menuStyles.studioMenu, classes.creditTrackerContainer)}
        trigger={creditTrackerSelect}
        placement='bottom-end'
        margin={5}
      >
        {() => (
          <>
            <div className={classes.creditTrackerHeader}>
              <span className={classes.creditPlanName}>
                {CREDIT_GRANT_FEATURE[activeCreditGrantName].planName}
              </span>
              <CreditDisplayInterval
                planName={CREDIT_GRANT_FEATURE[activeCreditGrantName].planName}
                creditAmount={CREDIT_GRANT_FEATURE[activeCreditGrantName].creditAmount}
              />
            </div>
            <CreditDisplayTracker activeCreditGrant={activeCreditGrant} showAmount={false} />
            {Build8.PLATFORM_TARGET === 'desktop'
              ? (
                <button
                  type='button'
                  className={combine('style-reset', classes.creditTrackerLink)}
                  onClick={() => {
                    // TODO(christoph): Clean up
                  }}
                >
                  {t('asset_lab.credit_tracker.get_more_credits')}&nbsp;
                  <Icon stroke='popOut' />
                </button>
              )
              : (
                <Link
                  className={classes.creditTrackerLink}
                  // TODO(christoph): Clean up
                  to='/'
                >
                  {t('asset_lab.credit_tracker.get_more_credits')}&nbsp;
                  <Icon stroke='popOut' />
                </Link>
              )}
          </>
        )}
      </SelectMenu>
    </div>
  )
}

export {
  AssetLabCreditTracker,
}
