import React from 'react'

import {ImageStyle, RerollOrientation} from '../../shared/genai/types/base'
import {useSelector} from '../hooks'
import {useRequestResult} from './hooks/use-request-result'

interface IAssetGenerationsForRequest {
  assetRequestUuid: string
  render: (assetRequest: string, assetGenerations: readonly string[]) => React.ReactNode
}
const AssetGenerationsForRequest: React.FC<IAssetGenerationsForRequest> = ({
  assetRequestUuid, render,
}) => {
  const {assetGenerations: assetGenerationUuids, assetRequest} = useRequestResult(assetRequestUuid)
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)
  const isMultiViewReq = assetRequest?.input?.style === ImageStyle.ANIMATED_MULTIVIEW ||
    assetRequest?.input?.style === ImageStyle.MULTIVIEW

  if (isMultiViewReq) {
    const assetGensByOrientation = assetGenerationUuids?.reduce((acc, uuid) => {
      const assetGen = assetGenerations[uuid]
      if (!assetGen) {
        return acc
      }
      acc[assetGen.metadata?.orientation as RerollOrientation] = uuid || ''
      return acc
    }, {} as Record<RerollOrientation, string>)
    const sortedAssetGenUuids = [
      assetGensByOrientation?.back,
      assetGensByOrientation?.left,
      assetGensByOrientation?.right,
    ]
    return <>{render(assetRequestUuid, sortedAssetGenUuids)}</>
  }

  const sortedAssetGenUuids = assetGenerationUuids
    ?.map(uuid => assetGenerations[uuid])
    .sort((a, b) => a.createdAt.localeCompare(b.createdAt))
    .map(gen => gen.uuid) || Array.from({length: assetRequest?.input?.numRequests})

  return <>{render(assetRequestUuid, sortedAssetGenUuids)}</>
}

export {AssetGenerationsForRequest}
