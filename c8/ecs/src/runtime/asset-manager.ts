import type {
  AssetManifest, StoredAssetManifest, AssetManifestMappings,
} from '../shared/asset-manifest'
import type {Eid} from '../shared/schema'

type Asset = {
  // TODO(Dale): Make data the ArrayBuffer itself
  // the response blob from the fetch call on remoteUrl
  data: Blob
  // where we sent fetch to get our asset data blob
  remoteUrl?: string
  // data url from URL.createObjectURL made from data
  localUrl: string
}

type AssetManager = {
  load: (request: AssetRequest) => Promise<Asset>
  clear: (request: AssetRequest) => void
  loadSync: (request: AssetRequest) => Asset
  // merge new manifest into the internal manifest
  setAssetManifest: (newManifest: AssetManifest) => void
  // resolve an asset path, e.g. asset/my-model.glb to the current url suffix on the CDN
  resolveAsset: (assetPath: string) => string | null
  getStatistics: () => AssetStatistics
}

type AssetStatistics = {
  pending: number
  complete: number
  total: number
}

type AssetRequest = {
  id?: Eid
  url: string
}

const isMappings = (manifest: AssetManifest): manifest is StoredAssetManifest => (
  !!manifest.assets
)

const createAssetManager = (): AssetManager => {
  let completeCount = 0
  let totalCount = 0
  const assets: Map<string, Asset> = new Map()
  const pendingLoads: Map<string, Promise<Asset>> = new Map()

  // file path to CDN url path.
  // e.g. 'assets/some/path/skyview2.png' => 'assets/skyview2-abcdefg12j.png'
  const pathToUrl: Map<string, string> = new Map()

  const setAssetManifest = (newManifest: AssetManifest) => {
    const mappings = isMappings(newManifest) ? newManifest.assets : newManifest
    Object.entries(mappings).forEach(([filePath, url]) => {
      pathToUrl.set(filePath, url)
    })
  }

  // @param assetUrl e.g. assets/my-model.glb
  // @returns use the manifest to map assetUrl to current location on CDN. Return original assetUrl
  //          if does not exist.
  const resolveAsset = (assetUrl: string): string => pathToUrl.get(assetUrl) ?? assetUrl

  // TODO(Dale): Implement defineAsset

  const fetchAsset = async (request: AssetRequest): Promise<Asset> => {
    try {
      totalCount++
      const remoteUrl = resolveAsset(request.url)
      const response = await fetch(remoteUrl)
      if (!response.ok) {
        throw new Error(`Failed to fetch asset: ${response.statusText} from ${remoteUrl}`)
      }
      const data = await response.blob()
      const localUrl = URL.createObjectURL(data)
      const asset = {data, remoteUrl, localUrl}
      assets.set(request.url, asset)
      return asset
    } finally {
      completeCount++
    }
  }

  const load = async (request: AssetRequest): Promise<Asset> => {
    const asset = assets.get(request.url)
    if (asset) {
      return asset
    }
    const pending = pendingLoads.get(request.url)
    if (pending) {
      return pending
    }
    const promise = fetchAsset(request)
    pendingLoads.set(request.url, promise)
    return promise
  }

  const loadSync = (request: AssetRequest): Asset => {
    const asset = assets.get(request.url)
    if (asset) {
      return asset
    }
    throw new Error(`Asset not found: ${request.url}`)
  }

  const clear = (request: AssetRequest) => {
    const asset = assets.get(request.url)
    if (asset) {
      URL.revokeObjectURL(asset.localUrl)
    }
    assets.delete(request.url)
  }

  const getStatistics = (): AssetStatistics => ({
    pending: totalCount - completeCount,
    complete: completeCount,
    total: totalCount,
  })

  return {
    load,
    clear,
    loadSync,
    setAssetManifest,
    resolveAsset,
    getStatistics,
  }
}

export {
  createAssetManager,
}

export type {
  AssetManager,
  AssetStatistics,
  AssetManifest,
  AssetManifestMappings,
  StoredAssetManifest,
}
