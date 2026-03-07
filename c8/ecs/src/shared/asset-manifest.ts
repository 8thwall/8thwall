// @attr(visibility = ["//visibility:public"])

type AssetManifestMappings = {
  [filePath: string]: string
} & {assets?: never}

type StoredAssetManifest = {
  assets: AssetManifestMappings
}

// NOTE(christoph): For forwards compatibility, we accept the "stored" asset manifest format which
// is scoped by "assets" fro extensibility
type AssetManifest = StoredAssetManifest | AssetManifestMappings

export type {
  AssetManifest,
  AssetManifestMappings,
  StoredAssetManifest,
}
