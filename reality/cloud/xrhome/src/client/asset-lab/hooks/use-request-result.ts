import {useSelector} from '../../hooks'
import {getAssetGenUrl} from '../urls'

const useRequestResult = (assetRequestUuid: string | undefined) => {
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetRequestUuid])
  const isGenerating = !!assetRequest && (
    assetRequest.status !== 'FAILED' && assetRequest.status !== 'SUCCESS'
  )
  const assetGenerations = useSelector(s => (
    s.assetLab.assetGenerationsByAssetRequest[assetRequestUuid]
  ))
  const hasResult = !!assetGenerations?.length
  // The result mesh is the first asset generation for the asset request
  const resultAssetGenerationUuid = assetGenerations?.[0]
  const resultMesh = useSelector(s => s.assetLab.assetGenerations[resultAssetGenerationUuid])
  const resultMeshUrl = getAssetGenUrl(resultMesh)
  return {
    assetGenerations,
    hasResult,
    resultMesh,
    resultMeshUrl,
    resultAssetGenerationUuid,
    assetRequest,
    isGenerating,
  }
}

export {useRequestResult}
