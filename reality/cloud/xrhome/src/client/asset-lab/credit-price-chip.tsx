import React from 'react'
import {useTranslation} from 'react-i18next'

import {calcGeneratePrice, WorkflowPrices} from '../../shared/genai/pricing'
import type {GenerateRequest} from '../../shared/genai/types/generate'
import {StandardChip} from '../ui/components/standard-chip'

interface ICreditPrice {
  type: keyof WorkflowPrices
  modelId: GenerateRequest['modelId']
  multiplier?: number
}

const CreditPriceChip: React.FC<ICreditPrice> = ({type, modelId, multiplier = 1}) => {
  const {t} = useTranslation('asset-lab')
  if (!type || !modelId) {
    return null  // Invalid input, do not render chip
  }

  const price = calcGeneratePrice(type, modelId, multiplier)
  if (!price) {
    return null
  }
  return (
    <StandardChip
      iconStroke='credits'
      inline
      text={t('asset_lab.credits_other', {count: price})}
    />
  )
}

export {
  CreditPriceChip,
}

export type {ICreditPrice}
