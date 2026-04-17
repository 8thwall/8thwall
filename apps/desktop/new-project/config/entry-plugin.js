const path = require('path')
const VirtualModulesPlugin = require('webpack-virtual-modules')

const isCodeFile = file => file.endsWith('.ts') || file.endsWith('.js')
const IGNORED_FOLDERS = ['assets', '.dependencies']

const SCENE_INIT_CONTENT = `
import scene from './.expanse.json'

delete scene.history
delete scene.historyVersion

window.ecs.application.init(scene)

if (module.hot) {
  const isInline = window.location.href.includes('liveSyncMode=inline')

  const handler = isInline
    ? () => { }
    : async () => {
      const updatedScene = (await import('./.expanse.json')).default

      delete updatedScene.history
      delete updatedScene.historyVersion

      window.ecs.application.getScene().updateBaseObjects(updatedScene.objects)
      window.ecs.application.getScene().updateDebug(updatedScene)
    }

  module.hot.accept('./.expanse.json', handler)
}`

const createVirtualEntryPlugin = (options = {}) => {
  const {srcDir} = options
  const virtualModules = new VirtualModulesPlugin()
  const importedFiles = new Set()
  const ignoredFoldersPaths = IGNORED_FOLDERS.map(folder => path.join(srcDir, folder))

  let hasInit = false
  let watcher = null

  const removeImports = (files) => {
    files.forEach(file => importedFiles.delete(file))
  }

  const updateVirtualModules = () => {
    const appFiles = Array.from(importedFiles).filter(
      file => file === path.join(srcDir, 'app.js') || file === path.join(srcDir, 'app.ts')
    )
    const otherFiles = Array.from(importedFiles).filter(
      file => !appFiles.includes(file)
    )

    const imports = [...appFiles, ...otherFiles]
      .map((file) => {
        const relativePath = path.relative(srcDir, file)
        return `import ${JSON.stringify(`./${relativePath.replace(/\\/g, '/')}`)}`
      })
      .join('\n')

    const content = `${imports}\n${SCENE_INIT_CONTENT}`

    virtualModules.writeModule(path.join(srcDir, 'entry.js'), content)
  }

  const initFileWatcher = async () => {
    if (watcher) {
      return
    }

    try {
      const chokidar = await import('chokidar')
      watcher = chokidar.default.watch(srcDir, {
        ignored: filePath => ignoredFoldersPaths.some(folder => filePath.startsWith(folder)),
        persistent: true,
      })

      watcher.on('add', (filePath) => {
        if (isCodeFile(filePath)) {
          importedFiles.add(filePath)
          updateVirtualModules()
        }
      })
    } catch (err) {
      // chokidar not available, file watching disabled
    }
  }

  const closeFileWatcher = () => {
    if (watcher) {
      watcher.close()
      watcher = null
    }
  }

  const getCodeFiles = (fs, dir) => {
    const files = fs.readdirSync(dir)
    files.forEach((file) => {
      const fullPath = path.join(dir, file)
      try {
        if (fs.lstatSync(fullPath).isDirectory()) {
          if (!IGNORED_FOLDERS.includes(file)) {
            getCodeFiles(fs, fullPath)
          }
        } else if (isCodeFile(fullPath)) {
          importedFiles.add(fullPath)
        }
      } catch (err) {
        // ignore virtual files
      }
    })
  }

  return {
    apply: (compiler) => {
      compiler.hooks.watchRun.tapAsync('VirtualEntryPlugin', async (compilation, callback) => {
        const {removedFiles} = compilation

        if (!hasInit) {
          hasInit = true
          updateVirtualModules()
          await initFileWatcher()
        }

        if (removedFiles) {
          const removedCodeFiles = Array.from(removedFiles).filter(isCodeFile)
          if (removedCodeFiles.length > 0) {
            removeImports(removedCodeFiles)
            updateVirtualModules()
          }
        }

        callback()
      })

      compiler.hooks.shutdown.tap('VirtualEntryPlugin', () => {
        closeFileWatcher()
      })

      compiler.hooks.beforeCompile.tapAsync('VirtualEntryPlugin', (_, callback) => {
        if (!compiler.watching && !hasInit) {
          getCodeFiles(compiler.inputFileSystem, srcDir)
          updateVirtualModules()
          hasInit = true
        }
        callback()
      })

      virtualModules.apply(compiler)
    },
  }
}

module.exports = createVirtualEntryPlugin
