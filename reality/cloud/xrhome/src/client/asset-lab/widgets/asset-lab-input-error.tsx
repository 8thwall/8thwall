import React from 'react'

import {useAssetLabStateContext} from '../asset-lab-context'
import {StaticBanner} from '../../ui/components/banner'

const AssetLabInputError: React.FC<{}> = () => {
  const assetLabCtx = useAssetLabStateContext()
  const {userInputError} = assetLabCtx.state
  if (!userInputError) {
    return null
  }

  return (
    <StaticBanner type='danger'>
      {userInputError}
    </StaticBanner>
  )
}

export {AssetLabInputError}
