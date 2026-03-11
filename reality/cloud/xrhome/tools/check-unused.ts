/* eslint-disable no-console */
import util from 'util'
import fs from 'fs'
import path from 'path'
import precinct from 'precinct'
import type {DeepReadonly} from 'ts-essentials'
import {parse} from '@babel/parser'

import {makeRunQueue} from '../src/shared/run-queue'

const readDir = util.promisify(fs.readdir)
const statFile = util.promisify(fs.stat)
const readFile = util.promisify(fs.readFile)

const runQueue = makeRunQueue(100)

const IGNORED_UNUSED_PACKAGES = new Set([
  // Used by unit tests
  '@types/mocha',
  'chai-as-promised',
  'chai',
  'mocha',
  'sinon',
  'ts-mocha',

  'real-semantic-ui-react',  // alias of semantic-ui-react

  // Used by shared ecs code
  '@automerge/automerge',
  '@tweenjs/tween.js',
  'base64-js',
  'three-nebula',
  'yoga-layout',

  // Used by webpack
  '@babel/core',
  '@babel/node',
  '@babel/plugin-proposal-class-properties',
  '@babel/plugin-proposal-decorators',
  '@babel/plugin-proposal-export-default-from',
  '@babel/plugin-proposal-export-namespace-from',
  '@babel/plugin-proposal-object-rest-spread',
  '@babel/plugin-proposal-optional-chaining',
  '@babel/plugin-syntax-dynamic-import',
  '@babel/plugin-syntax-export-default-from',
  '@babel/plugin-transform-object-assign',
  '@babel/plugin-transform-react-jsx',
  '@babel/plugin-transform-runtime',
  '@babel/plugin-transform-spread',
  '@babel/preset-env',
  '@babel/preset-react',
  '@babel/preset-typescript',
  '@babel/register',
  'assert',
  'babel-loader',
  'babel-plugin-css-modules-transform',
  'browserify-zlib',
  'buffer',
  'console-browserify',
  'constants-browserify',
  'copy-webpack-plugin',
  'crypto-browserify',
  'css-loader',
  'domain-browser',
  'events',
  'file-loader',
  'html-webpack-plugin',
  'https-browserify',
  'os-browserify',
  'path-browserify',
  'process',
  'punycode',
  'querystring-es3',
  'raw-loader',
  'react-refresh',
  'sass-loader',
  'sass',
  'stream-browserify',
  'stream-http',
  'string_decoder',
  'string-replace-loader',
  'style-loader',
  'terser-webpack-plugin',
  'timers-browserify',
  'to-string-loader',
  'tslib',
  'tty-browserify',
  'typescript',
  'vm-browserify',
  'webpack-node-externals',
  'worker-loader',

  // Used by storybook
  '@storybook/addon-essentials',
  '@storybook/addon-interactions',
  '@storybook/addon-links',
  '@storybook/addon-onboarding',
  '@storybook/addon-styling-webpack',
  '@storybook/blocks',
  '@storybook/react-webpack5',
  '@storybook/react',
  '@storybook/testing-library',
  'storybook',

  // Used by CLI tools
  '@typescript-eslint/eslint-plugin',
  'csv-writer',
  'esm',
  'gulp-cli',
  'patch-package',
  'ts-node',
  'tsconfig-paths',

  // Used by studiomcp
  'zod-form-data',
  'zod',
])

const ENTRY_POINTS = [
  'src/client/apps/image-targets/conical/unconify.worker.js',
  'src/client/apps/image-targets/conical/unconify.worker.d.ts',
  'src/client/worker/index.ts',
  'src/client/desktop/index.tsx',
  'src/client/desktop/global.d.ts',
  'src/client/studio/worker/analysis-worker.ts',
  'src/client/studio/worker/gltf-transform-worker.ts',
  'gulpfile.ts',
  'tools/check-unused.ts',
  'src/client/ui/form.tsx',
  'src/shared/run-queue.ts',
  'src/shared/registry.ts',
  'src/shared/asset-pointer.ts',  // Used by shared code
]

const IGNORED_PATHS = [
  '/src/client/desktop/index.html',
  '/src/client/static/semantic',
  '/src/client/ui/',
  '/src/shared/build8.ts',
  '/src/shared/ecs/',
  '/src/shared/gateway/',
  '/src/shared/globals.d.ts',
  '/src/shared/module/',
  '/src/shared/nae/',
  '/src/shared/react.d.ts',
  '/src/shared/static.d.ts',
  '/src/shared/studio/',
  '/src/shared/desktop/',
  '/src/shared/studiomcp/',
]

// We'll need these eventually
const WAITING_FOR_USE = [
  'src/client/common/delay.ts',
  'src/client/common/use-previous.ts',
  'src/client/studio/configuration/row-floating-panel-button.tsx',
]

const isIgnoredFile = (filePath: string) => (
  /(\/i18n\/.*\.(json|html)$)|.DS_Store|eslintrc|\.gitignore|(\.md$)|LICENSE|mock|\/BUILD$/
    .test(filePath) ||
  IGNORED_PATHS.some(e => filePath.includes(e))
)

const listFiles = async (files: string[], filePath: string) => {
  if (isIgnoredFile(filePath)) {
    return
  }
  const info = await runQueue.next(async () => statFile(filePath))

  if (info.isDirectory()) {
    const contents = await runQueue.next(async () => readDir(filePath))
    await Promise.all(contents.map(c => listFiles(files, path.join(filePath, c))))
  } else if (info.isFile()) {
    files.push(filePath)
  } else {
    console.warn('Ignoring', filePath)
  }
}

type AnalysisContext = {
  attemptedFiles: Set<string>
  crawledFiles: Set<string>
  resolvedDeps: Record<string, string[]>
  externalDeps: Set<string>
  exportedNames: Record<string, Set<string>>
  importedNames: Record<string, Set<string>>
  importedWildcards: Set<string>
}

const BINARY_EXTENSIONS = [
  '.png', '.jpg', '.svg', '.webp', '.mp4', '.woff', '.wasm', '.gif', '.ico', '.json', '.capnp',
  '.webm', '.md',
]
const CODE_EXTENSIONS = ['.ts', '.js', '.tsx', '.d.ts']
const CODE_RESOLUTION_SUFFIXES = [...CODE_EXTENSIONS, ...CODE_EXTENSIONS.map(e => `/index${e}`)]

const parserByExtension: DeepReadonly<Record<string, string>> = {
  '.js': 'commonjs',
  '.ts': 'ts',
  '.tsx': 'tsx',
  '.scss': 'scss',
  '.css': 'scss',
}

const storeExternalDependency = (importedName: string, ctx: AnalysisContext) => {
  if (importedName[0] === '@') {
    const [scope, packageName] = importedName.split('/')
    ctx.externalDeps.add(`${scope}/${packageName}`)
  } else {
    const name = importedName.split('/')[0]
    ctx.externalDeps.add(name)
    ctx.externalDeps.add(`@types/${name}`)
  }
}

const resolveDependencies = async (filePath: string, ctx: AnalysisContext) => {
  if (ctx.attemptedFiles.has(filePath)) {
    return
  }
  ctx.attemptedFiles.add(filePath)
  const ext = path.extname(filePath)
  if (BINARY_EXTENSIONS.includes(ext)) {
    return
  }

  let contents: string
  try {
    contents = await runQueue.next(() => readFile(filePath, 'utf8'))
  } catch (err) {
    if (err.code === 'ENOENT' || err.code === 'ENOTDIR') {
      if (!CODE_EXTENSIONS.includes(ext)) {
        console.warn('Not found:', filePath)
      }
      return
    }
    throw err
  }

  const withExtensionDeps: string[] = []
  try {
    const type = parserByExtension[ext]

    if (!type) {
      console.warn('Unknown extension:', filePath)
      return
    }

    const deps: string[] = precinct(contents, {type, scss: {url: true}})
    const dir = path.dirname(filePath)
    deps.forEach(((importPath) => {
      if (!importPath.startsWith('.')) {
        storeExternalDependency(importPath, ctx)
        return
      }

      const fullPath = path.resolve(dir, importPath)

      if (path.extname(fullPath)) {
        withExtensionDeps.push(fullPath)
      } else {
        withExtensionDeps.push(...CODE_RESOLUTION_SUFFIXES.map(e => fullPath + e))
      }
    }))

    if (ext === '.ts' || ext === '.tsx' || ext === '.js') {
      try {
        const plugins: Array<'typescript' | 'jsx'> = []
        if (ext === '.ts' || ext === '.tsx') {
          plugins.push('typescript')
        }
        if (ext === '.tsx') {
          plugins.push('jsx')
        }
        const ast = parse(contents, {
          sourceType: 'module',
          plugins,
        })

        // Store exported names (types and values)
        const exportedNames = new Set<string>()
        ast.program.body.forEach((node) => {
          switch (node.type) {
            case 'ExportNamedDeclaration':
              if (node.declaration) {
                switch (node.declaration.type) {
                  case 'VariableDeclaration':
                    node.declaration.declarations.forEach((decl) => {
                      switch (decl.type) {
                        case 'VariableDeclarator':
                          switch (decl.id.type) {
                            case 'Identifier':
                              exportedNames.add(decl.id.name)
                              break
                            default:
                              throw new Error(`Unknown declaration id type: ${decl.id.type}`)
                          }
                          break
                        default:
                          // Handle other declaration types if needed
                          break
                      }
                    })
                    break
                  case 'FunctionDeclaration':
                  case 'ClassDeclaration':
                  case 'TSInterfaceDeclaration':
                  case 'TSTypeAliasDeclaration':
                  case 'TSDeclareFunction':
                    if (node.declaration.id && node.declaration.id.name) {
                      exportedNames.add(node.declaration.id.name)
                    }
                    break
                  default:
                    // Handle other declaration types if needed
                    break
                }
              }
              if (node.specifiers) {
                node.specifiers.forEach((spec) => {
                  switch (spec.type) {
                    case 'ExportSpecifier':
                      switch (spec.exported.type) {
                        case 'Identifier':
                          exportedNames.add(spec.exported.name)
                          break
                        default:
                          throw new Error(`Unknown exported specifier type: ${spec.exported.type}`)
                      }
                      break
                    default:
                      // Handle other specifier types if needed
                      break
                  }
                })
              }
              break
            case 'ExportDefaultDeclaration':
              exportedNames.add('default')
              break
            case 'ExportAllDeclaration':
              console.warn('Export all detected in', filePath)
              ctx.importedWildcards.add(filePath)
              break
            default:
              // Handle other export node types if needed
              break
          }
        })
        ctx.exportedNames[filePath] = exportedNames

        // Record imported names based on the imported file
        ast.program.body.forEach((node) => {
          if (node.type === 'ImportDeclaration') {
            const importSource = node.source.value
            if (!importSource.startsWith('.')) return  // Only handle local imports
            const resolvedImportPath = path.extname(importSource)
              ? path.resolve(dir, importSource)
              : CODE_RESOLUTION_SUFFIXES.map(e => path.resolve(dir, importSource + e))
            const importPaths = Array.isArray(resolvedImportPath)
              ? resolvedImportPath
              : [resolvedImportPath]
            node.specifiers.forEach((spec) => {
              importPaths.forEach((importPath) => {
                if (!ctx.importedNames[importPath]) {
                  ctx.importedNames[importPath] = new Set()
                }

                switch (spec.type) {
                  case 'ImportDefaultSpecifier':
                    ctx.importedNames[importPath].add('default')
                    break
                  case 'ImportNamespaceSpecifier':
                    ctx.importedWildcards.add(importPath)
                    break
                  case 'ImportSpecifier':
                    switch (spec.imported.type) {
                      case 'Identifier':
                        ctx.importedNames[importPath].add(spec.imported.name)
                        break
                      default:
                        throw new Error(`Unknown imported specifier type: ${spec.imported.type}`)
                    }
                    break
                  default:
                    throw new Error(`Unknown import specifier type: ${JSON.stringify(spec)}`)
                }
              })
            })
          }
        })
      } catch (err) {
        console.error('Error parsing exports/imports in', filePath)
        console.error(err)
      }
    }
  } catch (err) {
    console.error('Error while parsing', filePath)
    console.error(err)
    throw err
  }
  ctx.crawledFiles.add(filePath)
  ctx.resolvedDeps[filePath] = withExtensionDeps
  await Promise.all(withExtensionDeps.map(dep => resolveDependencies(dep, ctx)))
}

const logValidationFailures = (message: string, array: string[]) => {
  if (!array.length) {
    return
  }

  console.log(message.replace('COUNT', String(array.length)))
  array.forEach((e) => {
    console.log(`  ${e}`)
  })
}

// eslint-disable-next-line arrow-parens
const intersect = <T>(a: Set<T>, b: Set<T>): Array<T> => [...a].filter(e => b.has(e))

// eslint-disable-next-line arrow-parens
const subtract = <T>(a: Set<T>, b: Set<T>): Array<T> => [...a].filter(e => !b.has(e))

const run = async () => {
  const ctx: AnalysisContext = {
    attemptedFiles: new Set(),
    crawledFiles: new Set(),
    resolvedDeps: {},
    externalDeps: new Set(),
    exportedNames: {},
    importedNames: {},
    importedWildcards: new Set([]),
  }

  console.warn('Listing files')

  const allFiles: string[] = []
  await listFiles(allFiles, path.join(process.cwd(), 'src'))

  console.warn('Listed', allFiles.length, 'source files.')

  await Promise.all(ENTRY_POINTS.map(filename => (
    resolveDependencies(path.join(process.cwd(), filename), ctx)
  )))

  // NOTE(christoph): Some scss constructs like "meta.load-css(...)" aren't being detected
  //  as imports so force them to resolve here.
  await Promise.all(allFiles.filter(e => e.endsWith('.scss')).map(filePath => (
    resolveDependencies(filePath, ctx)
  )))

  console.warn('Crawled', ctx.crawledFiles.size, 'files.')

  const waitingForUsePaths = WAITING_FOR_USE.map(f => path.join(process.cwd(), f))

  waitingForUsePaths.forEach((f) => {
    if (ctx.attemptedFiles.has(f)) {
      console.warn('File is no longer unused, please remove from WAITING_FOR_USE list:', f)
    }
  })

  const unusedFiles = allFiles.filter(file => (
    !waitingForUsePaths.includes(file) && !ctx.attemptedFiles.has(file)
  )).sort()

  logValidationFailures(
    'Found COUNT unused files.',
    unusedFiles.map(e => path.join(path.relative(process.cwd(), e)))
  )

  // Validating all dependencies are used by something
  const packageJson = JSON.parse(await readFile('package.json', 'utf8'))

  const dependencies = [
    ...Object.keys(packageJson.dependencies),
    ...Object.keys(packageJson.devDependencies),
  ]

  const unusedExternals = dependencies.filter(e => (
    !ctx.externalDeps.has(e) && !IGNORED_UNUSED_PACKAGES.has(e)
  ))
  logValidationFailures('Found COUNT unused dependencies.', unusedExternals)

  logValidationFailures(
    'Found COUNT ignored dependencies that are not installed.',
    subtract(IGNORED_UNUSED_PACKAGES, new Set(dependencies))
  )

  logValidationFailures(
    'Found COUNT ignored dependencies that are actually used.',
    intersect(IGNORED_UNUSED_PACKAGES, ctx.externalDeps)
  )

  if (process.argv.includes('--report-exports')) {
    const none: never[] = []
    const unusedExportNames = Object.entries(ctx.exportedNames).flatMap(([filePath, names]) => {
      if (ENTRY_POINTS.includes(path.relative(process.cwd(), filePath))) {
        return none
      }
      if (ctx.importedWildcards.has(filePath)) {
        return none
      }
      if (IGNORED_PATHS.some(ignoredPath => filePath.includes(ignoredPath))) {
        return none
      }
      const importedNames = new Set(ctx.importedNames[filePath] || [])
      const unusedNames = subtract(names, importedNames)
      return unusedNames.map(name => `${filePath}: ${name}`)
    })

    logValidationFailures(
      'Found COUNT unused exported names.',
      unusedExportNames
    )
  }
}

run()
