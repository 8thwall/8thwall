import {useState, useEffect, useMemo} from 'react'

import useActions from '../../common/use-actions'
import imageTargetsActions from '../../image-targets/actions'
import {useEnclosedApp} from '../../apps/enclosed-app-context'
import type {IImageTarget} from '../../common/types/models'
import {useSelector} from '../../hooks'

const useImageTarget = (targetName: string): [IImageTarget | undefined, boolean] => {
  const [loading, setLoading] = useState<boolean>(true)
  const app = useEnclosedApp()
  const {fetchSingleTargetByNameForApp} = useActions(imageTargetsActions)
  const targetsByUuid = useSelector(s => s.imageTargets.targetsByUuid)

  // TODO(chloe): Ideally have targetsByName or something to avoid traversing through all targets
  const targetData = useMemo(() => Object.values(targetsByUuid).find(
    it => it.AppUuid === app.uuid && it.name === targetName
  ), [targetName, app.uuid, targetsByUuid])

  useEffect(() => {
    const fetchTarget = async () => {
      setLoading(true)
      await fetchSingleTargetByNameForApp(app.uuid, targetName)
      setLoading(false)
    }

    if (!targetData) {
      fetchTarget()
    }
  }, [targetData, app.uuid, targetName])

  return [targetData, loading]
}

export {useImageTarget}
