import React from 'react'

import {useSelector} from '../../hooks'
import assetLabActions from '../asset-lab-actions'
import useActions from '../../common/use-actions'
import useCurrentAccount from '../../common/use-current-account'

interface IAssetRequestIntervalChecker {
  // uuid of the asset request
  id: string
}
const AssetRequestIntervalChecker: React.FC<IAssetRequestIntervalChecker> = ({id}) => {
  const assetRequest = useSelector(s => s.assetLab.assetRequests[id])
  const intervalRef = React.useRef<ReturnType<typeof setInterval> | null>(null)
  const {getAssetRequests} = useActions(assetLabActions)
  const accountUuid = useCurrentAccount().uuid

  React.useEffect(() => {
    if (assetRequest?.status === 'REQUESTED' || assetRequest?.status === 'PROCESSING') {
      if (intervalRef.current) {
        // Already running
        return null
      }

      intervalRef.current = setInterval(() => {
        getAssetRequests(accountUuid, {assetRequestUuids: [assetRequest.uuid]})
      }, 5000)  // Check every 5 seconds
    }
    // We don't need to clear interval for FAILED / SUCCESS because when the status change,
    // this assetRequest object will be updated and our interval is cleaned up.
    return () => {
      if (!intervalRef.current) {
        return
      }
      clearInterval(intervalRef.current)
      intervalRef.current = null
    }
  }, [assetRequest])

  return null
}

export {
  AssetRequestIntervalChecker,
}
