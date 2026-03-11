import React from 'react'

interface PrevAssetReq {
  assetRequestUuid: string
  extraAssetGenUuid?: string
}

const usePrevResults = (prevAssetReq: PrevAssetReq[] = []) => {
  const [prevAssetRequests, setPrevAssetRequests] = React.useState<PrevAssetReq[]>(prevAssetReq)
  return {
    prevAssetRequests,
    appendAssetRequest: (assetRequestUuid: string, extraAssetGenUuid?: string) => {
      setPrevAssetRequests(prev => [{assetRequestUuid, extraAssetGenUuid}, ...prev])
    },
  }
}

export {
  usePrevResults,
}

export type {PrevAssetReq}
