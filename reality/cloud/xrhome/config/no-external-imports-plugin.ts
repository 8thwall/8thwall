import type {Compiler} from 'webpack'
import path from 'path'

const makeNoExternalImportsPlugin = (rootPath: string) => ({
  apply(compiler: Compiler) {
    compiler.hooks.normalModuleFactory.tap('NoExternalImports', (normalModuleFactory) => {
      normalModuleFactory.hooks.afterResolve.tap('NoExternalImports', (data) => {
        const resolvedPath = data.createData.resource
        if (typeof resolvedPath === 'string') {
          const isExternal = path.relative(rootPath, resolvedPath).startsWith('..')
          if (isExternal) {
            throw new Error(`[NoExternalImports] External import disallowed: ${resolvedPath}`)
          }
        }
      })
    })
  },
})

export {
  makeNoExternalImportsPlugin,
}
