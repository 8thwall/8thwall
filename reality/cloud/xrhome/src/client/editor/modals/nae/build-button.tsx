import React from 'react'
import {useTranslation} from 'react-i18next'

import {NAE_BUILD_COST} from '../../../../shared/nae/nae-constants'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {useCreditQuery} from '../../../billing/use-credit-query'
import {CreditDisplayTracker} from '../../../billing/credit-plan/credit-display-tracker'
import {TertiaryButton} from '../../../ui/components/tertiary-button'
import {Tooltip} from '../../../ui/components/tooltip'
import {CreditLabel} from '../../publishing/credit-label'
import {useNaeStyles} from './nae-styles'

interface IBuildButton {
  isBuilding: boolean
  buildDisabled: boolean
  onClose: () => void
  handleExport: () => void
}

const BuildButton: React.FC<IBuildButton> = ({
  isBuilding,
  buildDisabled,
  onClose,
  handleExport,
}) => {
  const classes = useNaeStyles()
  const {t} = useTranslation(['common'])

  const creditQuery = useCreditQuery()
  const canShowCredit = !creditQuery.isLoading && !creditQuery.isError
  const hasEnoughCredits = canShowCredit && creditQuery.data.creditAmount >= NAE_BUILD_COST

  const exportButtonDisabled = buildDisabled || !hasEnoughCredits

  return (
    isBuilding
      ? (
        <TertiaryButton
            // eslint-disable-next-line local-rules/ui-component-styling
          className={classes.whiteSpaceNoWrap}
          height='small'
          onClick={onClose}
        >
          {t('button.minimize_and_continue', {ns: 'common'})}
        </TertiaryButton>
      )
      : (
        <Tooltip
          content={<CreditDisplayTracker tooltip hasSufficientCredit={hasEnoughCredits} />}
          position='top'
          zIndex={1000}
        >
          <PrimaryButton
              // eslint-disable-next-line local-rules/ui-component-styling
            className={classes.buildButton}
            a8='click;studio-export-flow;build'
            color='purple'
            disabled={exportButtonDisabled}
            height='small'
            onClick={handleExport}
          >
            {t('button.build', {ns: 'common'})}
            <CreditLabel disabled={exportButtonDisabled} text={`${NAE_BUILD_COST}`} />
          </PrimaryButton>
        </Tooltip>
      )
  )
}

export {BuildButton}
