import React from 'react'

import AssetLabContainer from './asset-lab-container'
import {useAssetLabStateContext} from './asset-lab-context'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'

const AssetLabModal = () => {
  const assetLabCtx = useAssetLabStateContext()
  return (
    <FloatingTrayModal
      startOpen
      onOpenChange={() => assetLabCtx.setState({open: false})}
      trigger={null}
    >
      {() => (<AssetLabContainer isModal />)}
    </FloatingTrayModal>
  )
}

export default AssetLabModal
