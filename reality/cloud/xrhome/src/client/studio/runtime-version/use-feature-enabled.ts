import {useRuntimeMetadata} from './use-runtime-metadata'

type RuntimeFeature = {
  name: string
  edition: number
}

const useFeatureEnabled = (feature: RuntimeFeature) => {
  const metadata = useRuntimeMetadata()
  if (metadata.features.edition > feature.edition) {
    return true
  }
  return metadata.features[feature.name] === true
}

export {
  useFeatureEnabled,
}
