import {useSelector} from '../../hooks'
import {extractFblr} from '../generate-request'

const useMeshGeneration = (id: string) => {
  const meshGeneration = useSelector(s => s.assetLab.assetGenerations[id])
  const assetRequest = useSelector(s => s.assetLab.assetRequests[meshGeneration?.RequestUuid])
  const meshInputFblr = extractFblr(assetRequest, meshGeneration)

  return {
    meshGeneration,
    meshInputFblr,
  }
}

export {useMeshGeneration}
