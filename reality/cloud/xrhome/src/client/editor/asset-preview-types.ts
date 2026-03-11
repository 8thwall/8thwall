interface AssetDimension {
  width: Number
  height: Number
}

interface AdditionalAssetData {
  dimension: AssetDimension
  duration: Number
}

interface ModelInfo {
  vertices: number
  triangles: number
  sizeInBytes?: number
  maxTextureSize?: number
  isDraco?: boolean
}

export {
  AssetDimension,
  AdditionalAssetData,
  ModelInfo,
}
