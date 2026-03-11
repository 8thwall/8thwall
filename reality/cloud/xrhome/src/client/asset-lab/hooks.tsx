import useCurrentAccount from '../common/use-current-account'
import {useSelector} from '../hooks'

const useCurrentAccountAssetGenerations = () => {
  const currentAccount = useCurrentAccount()
  const assetGenerationIds = useSelector(s => s.assetLab.assetGenerationsByAccount[
    currentAccount?.uuid
  ]) || []
  // Display in a reverse order on the UI
  const reversedIds = [...assetGenerationIds].reverse()
  return {
    assetGenerationIds: reversedIds,
  }
}

export {
  useCurrentAccountAssetGenerations,
}
