const path = require('path')
const {ConcatSource} = require('webpack-sources')
const {Compilation} = require('webpack')

const createAutoImportImageTargetsPlugin = ({imageTargetsPath, bundleName}) => ({
  apply: (compiler) => {
    compiler.hooks.compilation.tap('AutoImportImageTargetsPlugin', (compilation) => {
      const loadTargetData = () => {
        try {
          const contents = compilation.inputFileSystem.readdirSync(imageTargetsPath)
          return contents
            .filter(e => e.endsWith('.json'))
            .map(e => path.join(imageTargetsPath, e))
            .map(e => compilation.inputFileSystem.readFileSync(e))
            .map(e => JSON.parse(e))
        } catch (err) {
          if (err.code === 'ENOENT') {
            return []
          }
          throw err
        }
      }

      const processAssets = (assets) => {
        if (assets[bundleName]) {
          const data = loadTargetData()
          const additionalCode = `
(() => {
  const onxrloaded = () => XR8.XrController.configure({
    imageTargetData: ${JSON.stringify(data, null, 2)},
  })
  window.XR8 ? onxrloaded() : window.addEventListener('xrloaded', onxrloaded)
})();
`
          compilation.updateAsset(bundleName, old => new ConcatSource(old, additionalCode))
        }
      }
      compilation.hooks.processAssets.tap({
        name: 'AutoImportImageTargetsPlugin',
        stage: Compilation.PROCESS_ASSETS_STAGE_ADDITIONS,
      }, processAssets)
    })
  },
})

module.exports = createAutoImportImageTargetsPlugin
