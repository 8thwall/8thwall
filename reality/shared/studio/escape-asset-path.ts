const ASSET_PREFIX = '/assets/'

const escapeAssetPath = (assetPath: string) => {
  if (typeof assetPath !== 'string' || !assetPath.startsWith(ASSET_PREFIX)) {
    throw new Error(`Invalid asset path: ${JSON.stringify(assetPath)}`)
  }

  return encodeURIComponent(assetPath)
    .replace(/\+/g, '%2B')
    .replace(/%2F/gi, '/')
}

export {
  escapeAssetPath,
}
