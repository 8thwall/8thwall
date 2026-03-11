import type {DeepReadonly} from 'ts-essentials'

import type {AssetRequest} from '../../common/types/db'
import {useSelector} from '../../hooks'
import type {FblrAsset} from '../types'

const useMultiViewParentRequest = (assetRequestUuid: string) => {
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetRequestUuid])
  const parentRequest = useSelector(s => s.assetLab.assetRequests[assetRequest?.ParentRequestUuid])
  // If the request has a parent request, then the previous asset request was a multi-view request.
  // The next multi-view request should use the same parent request i.e. the request that generated
  // the front-view.
  return parentRequest || assetRequest
}

// ParentRequestUuid on mesh requests should only be set if:
// - The multi-view request's parent is the image request that produced the front view.
// - None of the FBLR images are user-uploaded.
const useFblrParentRequest = (
  fblrUuids: (FblrAsset)[]
): DeepReadonly<AssetRequest> | null => useSelector((s) => {
  if (!fblrUuids?.length) {
    return null
  }

  const {assetGenerations, assetRequests} = s.assetLab
  const frontViewGen = assetGenerations[typeof fblrUuids[0] === 'string' ? fblrUuids[0] : undefined]
  if (!frontViewGen) {
    return null
  }

  const frontViewReq = assetRequests[frontViewGen.RequestUuid]
  if (fblrUuids.length === 1) {
    return frontViewReq
  }

  for (const genUuid of fblrUuids.slice(1)) {
    const gen = assetGenerations[typeof genUuid === 'string' ? genUuid : undefined]
    if (!gen) {
      return null
    }
    const req = assetRequests[gen.RequestUuid]
    if (req.ParentRequestUuid !== frontViewReq.uuid) {
      return null
    }
  }

  return frontViewReq
})

const useMeshParentRequest = (
  meshUuid: string
): DeepReadonly<AssetRequest> | null => useSelector((s) => {
  if (!meshUuid) {
    return null
  }

  const meshGen = s.assetLab.assetGenerations[meshUuid]
  if (!meshGen) {
    return null
  }

  return s.assetLab.assetRequests[meshGen.RequestUuid]
})

export {
  useMultiViewParentRequest,
  useFblrParentRequest,
  useMeshParentRequest,
}
