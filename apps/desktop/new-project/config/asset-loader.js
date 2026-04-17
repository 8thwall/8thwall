const path = require('path')

const toUnixPath = process.platform === 'win32'
  ? p => p.replace(/\\/g, '/').replace(/\/+/g, '/')
  : p => p

const makeExport = srcPath => `module.exports = ${JSON.stringify(toUnixPath(srcPath))};`

// Export the asset path as asset content
function assetLoader() {
  const srcPath = path.relative(this.rootContext, this.resourcePath)

  // Export the relative path for the asset file
  return makeExport(srcPath)
}

module.exports = assetLoader
