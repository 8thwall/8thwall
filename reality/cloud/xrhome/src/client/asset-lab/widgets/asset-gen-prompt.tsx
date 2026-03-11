import React from 'react'

import {useSelector} from '../../hooks'

interface IAssetRequestPrompt {
  uuid: string
}

const AssetRequestPrompt: React.FC<IAssetRequestPrompt> = ({uuid}) => {
  const assetGenUuid = useSelector(s => s.assetLab.assetGenerationsByAssetRequest[uuid]?.[0])
  const assetGen = useSelector(s => s.assetLab.assetGenerations[assetGenUuid])
  // eslint-disable-next-line react/jsx-no-useless-fragment
  return <>{assetGen?.prompt}</>
}

export {
  AssetRequestPrompt,
}
