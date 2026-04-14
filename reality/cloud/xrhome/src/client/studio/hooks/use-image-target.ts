import {useImageTargetsOrLoading} from '../../image-targets/use-image-targets'

const useImageTarget = (name?: string | null) => {
  const targets = useImageTargetsOrLoading()
  const target = targets.data?.find(e => e.name === name)
  return [target, targets.isLoading] as const
}

export {
  useImageTarget,
}
