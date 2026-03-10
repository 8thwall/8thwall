// Load some node.js modules.
import * as fs from 'fs'
import * as path from 'path'
import {promisify} from 'util'

import type * as Webpack from 'webpack'
import type * as DtsBundleGenerator from 'dts-bundle-generator'

const cwd = process.cwd()

const resolveNodeModulesFolder = (basePath: string): string => (
  path.join(path.dirname(fs.realpathSync(path.join(basePath, 'fingerprint.sha1'))), 'node_modules')
)

// Get the location of the node_modules needed for running webpack
// e.g. /private/var/tmp/_bazel_USERNAME/1234/external/npm-webpack-build/node_modules
const webpackBuildNodeModules =
  resolveNodeModulesFolder(path.join(process.env.RUNFILES_DIR!, 'npm-webpack-build'))

// We can resolve the libraries we need by resolving in our specific node_modules folder
// e.g. css-loader
/* eslint-disable local-rules/commonjs, import/no-dynamic-require, global-require */
const resolveLib = (module: string): string => (
  require.resolve(module, {paths: [webpackBuildNodeModules]})
)
const requireLib = <T = any>(module: string): T => require(resolveLib(module))

// Load extra packages from our node_modules.
const webpack = promisify(requireLib('webpack'))
const {SourceMapDevToolPlugin, optimize: {LimitChunkCountPlugin}} = requireLib('webpack')
const {ConcatSource} = requireLib('webpack-sources')
const TerserPlugin = requireLib('terser-webpack-plugin')
const {BundleAnalyzerPlugin} = requireLib('webpack-bundle-analyzer')
const escapeStringRegexp = requireLib<(s: string) => string>('escape-string-regexp')
const {generateDtsBundle} = requireLib('dts-bundle-generator') as typeof DtsBundleGenerator

const EXTENSIONS = ['.ts', '.js', '.tsx', '.jsx']

const RED = '\x1b[31m'
const YELLOW = '\x1b[33m'
const CLEAR = '\x1b[0m'

// Get the process arguments as a js dictionary. Named arguments are keys to the dictionary.
// Unnamed arguments are available with their order preserved in '_ordered'.
//
// Example:
//
// ./webpack-build.js bazel-out/darwin-opt/bin/bzl/hellobuild/js/hi.js bzl/hellobuild/js/main.js
//
// Returns:
// {
//   _node: '/usr/local/Cellar/node/14.5.0/bin/node',
//   _script: '/private/var/tmp/.../darwin-sandbox/33/execroot/niantic/bzl/js/webpack-build.js',
//   _ordered: [
//     'bazel-out/darwin-opt/bin/bzl/hellobuild/js/hi.js',
//     'bzl/hellobuild/js/main.js'
//   ]
// }
const getArgs = () => {
  const args = process.argv
  // The first two arguments are the node instance and the script that's being run.
  const dict: Record<string, any> = {
    _node: args[0],
    _script: args[1],
    _ordered: [],
    includes: [],
  }

  args.slice(2).forEach((arg) => {  // Consume remaining arguments starting at 2.
    if (arg.startsWith('-I')) {
      dict.includes.push(arg.substring(2))
      return
    }
    if (!arg.startsWith('--')) {
      dict._ordered.push(arg)  // Argument doesn't have a name, add to ordered list.
      return
    }
    const eqsep = arg.substring(2).split('=')  // get arg without '--' and split on =
    if (eqsep.length === 1) {
      dict[eqsep[0]] = true  // There is no '= sign for a value; assign true.
      return
    }
    const val = eqsep.slice(1).join('=')  // Add = back to remaining components if needed.
    dict[eqsep[0]] = val === 'false' || val === '0' ? false : val
  })
  return dict
}

const mapAndFind = <T, E>(arr: T[], fn: (e: T) => E | null): E | null => {
  for (const e of arr) {
    const res = fn(e)
    if (res) {
      return res
    }
  }
  return null
}

type BeforeResolveData = {
  context: string
  request: string
  dependencies: {request: string}[]
}

// NOTE(christoph): This is supposed to do the same thing as NormalModuleReplacementPlugin
// but for some reason there were these resource.dependencies that were present that
// also needed to be rewritten for it to work.
const createRootResolverPlugin = (roots: string[]) => {
  const handleBeforeResolve = (resource: BeforeResolveData) => {
    if (!resource.request?.startsWith('@repo/')) {
      return
    }
    const importPath = resource.request.substring('@repo/'.length)
    const resolvedPath = mapAndFind(EXTENSIONS, ext => mapAndFind(roots, (root) => {
      const fullPath = path.join(root, importPath + ext)
      try {
        fs.lstatSync(fullPath)
        return path.join(root, importPath)
      } catch (err) {
        if (err.code !== 'ENOENT') {
          throw err
        }
        return null
      }
    }))

    if (resolvedPath) {
      resource.dependencies.forEach((dep) => {
        if (dep.request !== resource.request) {
          // eslint-disable-next-line no-console
          console.error(`[NiaResolverPlugin] Expected ${dep.request} to be ${resource.request}`)
          process.exit(1)
        }
        dep.request = resolvedPath
      })
      resource.request = resolvedPath
    } else {
      throw new Error(`Failed to resolve "${resource.request}"`)
    }
  }

  return {
    apply(compiler: Webpack.Compiler) {
      compiler.hooks.normalModuleFactory.tap(
        'RootResolverPlugin',
        (factory) => {
          factory.hooks.beforeResolve.tap('RootResolverPlugin', handleBeforeResolve)
        }
      )
    },
  }
}

type PostBuildAction = () => Promise<void>

const createWrapperPlugin = ({headerPath, footerPath}) => {
  const apply = (compiler) => {
    compiler.hooks.compilation.tap('WrapperPlugin', (compilation) => {
      compilation.hooks.afterOptimizeChunkAssets.tap('WrapperPlugin', (chunks) => {
        const headerContents = headerPath ? fs.readFileSync(headerPath, 'utf-8') : ''
        const footerContents = footerPath ? fs.readFileSync(footerPath, 'utf-8') : ''

        chunks.forEach((chunk) => {
          chunk.files.forEach((file) => {
            compilation.updateAsset(file,
              old => new ConcatSource(headerContents, old, footerContents))
          })
        })
      })
    })
  }

  return {
    apply,
  }
}

// This list was taken from https://github.com/webpack/node-libs-browser/blob/master/index.js
// then removing 'console' and 'process'.
const NATIVE_NODE_MODULES = [
  'assert', 'buffer', 'child_process', 'cluster', 'constants', 'crypto', 'dgram',
  'dns', 'domain', 'events', 'fs', 'http', 'https', 'module', 'net', 'os', 'path', 'perf_hooks',
  'punycode', 'querystring', 'readline', 'repl', 'stream', '_stream_duplex', '_stream_passthrough',
  '_stream_readable', '_stream_transform', '_stream_writable', 'string_decoder', 'sys', 'timers',
  'tls', 'tty', 'url', 'util', 'vm', 'worker_threads', 'zlib',
]

const getFallback = (target, polyfill) => {
  if (target === 'node') {
    return undefined
  }

  // NOTE(christoph): This is a conservative subset of the list defined here:
  //   https://webpack.js.org/configuration/resolve/#resolvefallback
  const availableFallbacks = {
    buffer: path.join(webpackBuildNodeModules, 'buffer'),
    path: resolveLib('path-browserify'),
  }

  if (polyfill === '<default>') {
    // NOTE(christoph): By default we'll set a sane default that will still fail the build if
    // an unexpected node dependency is specified, like child_process.
    return {path: resolveLib('path-browserify'), fs: false}
  }

  const fallback = {}

  // NOTE(christoph): By setting false on all native modules, we're ignoring any imports of those
  // modules, assuming that they are enclosed in something like:
  //   if (isNode) {
  //      require('child_process').doSomething()
  //   }
  NATIVE_NODE_MODULES.forEach((name) => {
    fallback[name] = false
  }, {})

  const includedModules = polyfill === 'none'
    ? []
    : polyfill.split(',').map(e => e.trim()).filter(Boolean)

  includedModules.forEach((name) => {
    const location = availableFallbacks[name]
    if (!location) {
      throw new Error(`Unexpected polyfill specified: ${name}`)
    }
    fallback[name] = location
  })

  return fallback
}

const resolveBuildPaths = async (npmRule, includes) => {
  // NOTE(cbartschat): To resolve transitive imports correctly, we first need to attempt to resolve
  // from any node_modules folder relative to the requester.
  // See bzl/examples/js/resolve/transitive.js to see this in practice.
  const targetNodeModules = ['node_modules']

  const topLevelImportPaths = ['*']  // Modules that can be imported at top level.
  includes.forEach((include) => { topLevelImportPaths.push(path.join(include, '*')) })

  if (npmRule) {
    const expectedFormatMatch = npmRule.match(/@(npm-.*)\/\/:(npm-.*)$/)
    if (!expectedFormatMatch || expectedFormatMatch[1] !== expectedFormatMatch[2]) {
      throw new Error(`npm_rule expected format: @npm-xyz//:npm-xyz but got: ${npmRule}`)
    }

    const sourceNodeModules = resolveNodeModulesFolder(
      path.join('external', expectedFormatMatch[1])
    )
    targetNodeModules.push(sourceNodeModules)
    topLevelImportPaths.push(path.join(sourceNodeModules, '*'))
  }

  // These dependencies are provided by npm-webpack-build, but we need to make them available to
  // the main build, so we use aliases.
  const alias = {
    'regenerator-runtime': resolveLib('regenerator-runtime'),
    'tslib': resolveLib('tslib'),
    '@babel/runtime': path.join(webpackBuildNodeModules, '@babel', 'runtime'),
  }

  // typescriptIncludes are used by ts-loader to resolve non-standard deps. It doesn't use
  // the same resolution logic specified in the normal webpack config.
  const typescriptIncludes = {
    'tslib': [resolveLib('tslib')],
    '@repo/*': topLevelImportPaths,
    '*': topLevelImportPaths,
  }

  return {
    typescriptIncludes,
    nodeModuleDirs: targetNodeModules,
    alias,
  }
}

type ExternalResolverData = {
  context?: string | undefined
  request?: string | undefined
  getResolve?: any
}

type ExternalsConfig = Webpack.Configuration['externals'] | undefined

const makeExternals = (target: string, externals: string): ExternalsConfig => {
  if (!externals) {
    return undefined
  }
  if (externals === '*') {
    return [async ({context, request, getResolve}: ExternalResolverData) => {
      if (!request || !context) {
        throw new Error('Expected request and context')
      }
      if (['../', './'].some(e => request.startsWith(e))) {
        return false
      }
      if (target === 'node' && NATIVE_NODE_MODULES.includes(request)) {
        return request
      }
      const resolver = getResolve()
      const resolved = await resolver(context, request)
      if (resolved.includes('/node_modules/')) {
        return request
      }
      return false
    }]
  }
  return externals.split(',').map(e => e.trim()).map((e) => {
    if (e.includes('*')) {
      // Turn a glob into a regex, for example '@aws-sdk/*' -> /^@aws-sdk\/.*/
      const parts = e.split('*')
      const escaped = parts.map(escapeStringRegexp)
      return new RegExp(`^${escaped.join('.*')}$`)
    } else {
      return e
    }
  })
}

// Generate a webpack config.
const genConfig = async ({
  outFile,
  mainFiles,
  cwdPath,
  target,
  includeWebTypes,
  includes,
  mode,
  commonjs,
  npmRule,
  analyze,
  sourceMap,
  headerPath,
  footerPath,
  mangle,
  polyfill,
  exportLibrary,
  externals,
  externalsType,
  stringReplacements,
  esnext,
  tsDeclaration,
  fullDts,
}) => {
  // Get the directory location and filename of the output file.
  const outFileDir = path.dirname(outFile)
  const outFileName = path.basename(outFile)
  const outFileBase = outFileName.replace(/\.js$/, '')

  const buildPaths = await resolveBuildPaths(npmRule, includes)

  if (!['node', 'web'].includes(target)) {
    throw new Error('Target must be "web" or "node"')
  }

  if (target === 'node' && polyfill !== '<default>') {
    throw new Error('Unexpected polyfill specified when target is "node"')
  }

  if (fullDts && !tsDeclaration) {
    throw new Error('fullDts can only be set when tsDeclaration is true')
  }

  // Since typescript doesn't know about our node_modules, directories, we have to tell it where to
  // look for type definitions.
  const typeRoots = buildPaths.nodeModuleDirs.map(dir => path.join(dir, '@types'))

  // We also need to include type definitions for node. For the browser, there are polyfills, so we
  // still want the types to be accessible.
  typeRoots.push(path.join(webpackBuildNodeModules, '@types'))

  const generatedPath = target === 'node'
    ? path.resolve(cwd, 'bzl/js/generated/node/')
    : path.resolve(cwd, 'bzl/js/generated/browser/')

  // ts-loader needs a tsconfig file, but we need to generate this dynamically based on the
  // environment. Particularly, we need to alias the different node_modules directories so that
  // typescript knows where to find them.
  const tsconfig = {
    compilerOptions: {
      esModuleInterop: true,
      noImplicitAny: false,
      target: esnext ? 'esnext' : 'es6',
      lib: [
        esnext ? 'esnext' : 'es2019',
      ].concat(includeWebTypes ? ['dom'] : []),
      jsx: 'react',
      module: esnext ? 'esnext' : undefined,
      moduleResolution: 'node',
      isolatedModules: false,
      experimentalDecorators: true,
      emitDecoratorMetadata: true,
      declaration: true,
      noImplicitUseStrict: false,
      // NOTE(christoph): We need to preserve comments like /* webpackIgnore: true */.
      removeComments: false,
      noLib: false,
      sourceMap,
      // NOTE(christoph): It seems like noEmitOnError can cause the builds to fail on weird edge
      // cases around emitting .d.ts files.
      // We only care about that when we plan to use the .d.ts files.
      // Normal errors will sill be detected, but not having types set up to output a proper .d.ts
      // will not crash the build in normal cases, unlike when tsDeclaration is true.
      noEmitOnError: tsDeclaration,
      noEmitHelpers: true,
      importHelpers: true,
      preserveConstEnums: true,
      preserveSymlinks: true,
      baseUrl: '.',
      typeRoots,
      paths: {
        'bzl/js/generated/*': [[generatedPath, '*'].join(path.sep)],
        ...buildPaths.typescriptIncludes,
      },
    },
  }

  const tsconfigPath = [cwd, 'tsconfig.json'].join(path.sep)
  fs.writeFileSync(tsconfigPath, JSON.stringify(tsconfig, null, 2))

  const babelPlugins = [
    '@babel/plugin-transform-runtime',
  ]

  if (commonjs) {
    babelPlugins.push('@babel/plugin-transform-modules-commonjs')
  }

  const extraLoaders: Webpack.RuleSetUseItem[] = []
  if (stringReplacements) {
    // eslint-disable-next-line import/no-dynamic-require, global-require
    const {replacements} = require(require.resolve(path.join(cwd, stringReplacements)))
    if (!replacements) {
      throw new Error('Missing exported `replacements` from your string_replacements module')
    } else {
      // replace 'local' with whether we are in prod or not
      const isLocal = process.env.C8VERSION !== 'true'
      const replacementsWithLocal = replacements.map(replacement => (
        {
          ...replacement,
          replace: replacement.replace === 'local' ? `${!!isLocal}` : `${!!replacement.replace}`,
        }
      ))

      extraLoaders.push({
        loader: resolveLib('string-replace-loader'),
        options: {multiple: replacementsWithLocal},
      })
    }
  }

  const rules: Webpack.RuleSetRule[] = [
    {
      test: /\.jsx?$/,
      use: [
        ...extraLoaders,
        {
          loader: resolveLib('babel-loader'),
          options: {
            presets: ['@babel/preset-react', '@babel/preset-env'].map(resolveLib),
            plugins: babelPlugins.map(resolveLib),
            targets: 'supports es6-class and defaults',
          },
        },
      ],
      exclude: /(node_modules|bower_components|-wasm\.js$|-asm\.js$)/,
    },
    {
      test: /\.tsx?$/,
      exclude: /node_modules/,
      use: [
        ...extraLoaders,
        {
          loader: resolveLib('ts-loader'),
          options: {
            configFile: tsconfigPath,
            context: cwdPath,
          },
        },
      ],
    },
    {test: /\.css$/, use: ['style-loader', 'css-loader'].map(resolveLib)},
    {test: /\.html$/, use: {loader: resolveLib('html-loader')}},
  ]

  const plugins: Webpack.Configuration['plugins'] = [
    // Allow only one chunk, as bazel is expecting a single output.
    new LimitChunkCountPlugin({maxChunks: 1}),
    createRootResolverPlugin([cwd, ...includes.map((e: string) => path.join(cwd, e))]),
  ]

  if (headerPath || footerPath) {
    plugins.push(createWrapperPlugin({headerPath, footerPath}))
  }

  if (sourceMap) {
    plugins.push(
      new SourceMapDevToolPlugin({
        filename: `${outFileName}.map`,
        append: `//# sourceMappingURL=${outFileName}.map`,
        exclude: /(node_modules|bower_components|wasm)/,
      })
    )
  }

  if (analyze) {
    plugins.push(
      new BundleAnalyzerPlugin({
        analyzerMode: 'static',
        reportFilename: `${outFileBase}-webpack-report.html`,
        openAnalyzer: false,
        generateStatsFile: true,
        statsFilename: `${outFileBase}-webpack-stats.json`,
        logLevel: 'warn',
      })
    )
  }

  const postBuildActions: PostBuildAction[] = []

  if (tsDeclaration) {
    const mainFile = mainFiles[0]
    const sourcePath = path.join(cwd, mainFile.replace(/\.ts$/, '.d.ts'))
    const outPath = path.join(cwd, outFileDir, `${outFileBase}.d.ts`)

    if (fullDts) {
      postBuildActions.push(async () => {
        try {
          const declarationFile = generateDtsBundle(
            [{
              filePath: mainFile,
              output: {
                noBanner: true,
                exportReferencedTypes: false,
                inlineDeclareGlobals: true,
              },
            }],
            {preferredConfigPath: tsconfigPath, followSymlinks: false}
          )[0]

          await fs.promises.writeFile(outPath, declarationFile)
        } catch (e) {
          // eslint-disable-next-line no-console
          console.error(e)
          throw new Error('Error generating .d.ts with full_dts=1')
        }
      })
    } else {
      postBuildActions.push(async () => {
        if (!fs.existsSync(sourcePath)) {
          throw new Error(`Expected ${sourcePath} to exist`)
        }
        await fs.promises.copyFile(sourcePath, outPath)
      })
    }
  }

  const output: Webpack.Configuration['output'] = {
    filename: outFileName,
    path: path.join(cwdPath, outFileDir),
  }

  const options: Webpack.Configuration = {
    devtool: false,
    entry: mainFiles.map(f => ['.', f].join(path.sep)),
    output,
    optimization: {
      minimize: mode === 'production',
      avoidEntryIife: false,
      minimizer: [
        new TerserPlugin({
          parallel: true,
          extractComments: false,
          exclude: /(node_modules|bower_components|-wasm.js$)/,
          terserOptions: {
            mangle: mangle && mode !== 'development',
            sourceMap,
          },
        }),
      ],
    },
    performance: {hints: false},
    plugins,
    externals: makeExternals(target, externals),
    externalsType,
    experiments: {
      topLevelAwait: true,
    },
    resolve: {
      extensions: EXTENSIONS,
      symlinks: false,  // Don't resolve symlinks as their real paths (important for bazel).
      modules: [
        cwd,  // Resolves JS code from the bazel exec root.
        ...includes,
        ...buildPaths.nodeModuleDirs,  // Has zero or one node_modules directories.
      ],
      alias: {...buildPaths.alias, 'bzl/js/generated': generatedPath},
      fallback: getFallback(target, polyfill),
    },
    module: {rules},
    mode,
    context: cwdPath,
    target,
    profile: analyze,
  }
  if (exportLibrary) {
    if (commonjs) {
      output.libraryTarget = 'commonjs'
    } else {
      output.libraryTarget = 'module'
      output.chunkFormat = 'module'
      options.experiments = Object.assign(options.experiments || {}, {outputModule: true})
    }
  } else if (esnext && target === 'browser') {
  // NOTE(christoph): There is more work involved if target is node because we'd
  // need to set a "mjs" extension or a type: "module" field in package.json.
    options.experiments = Object.assign(options.experiments || {}, {outputModule: true})
  }
  return [options, postBuildActions] as const
}

// Run webpack with the specified config.
const runWebpack = async (webpackConfigPromise: ReturnType<typeof genConfig>) => {
  const [webpackConfig, postBuildActions] = await webpackConfigPromise

  // Run webpack.
  let err = 0
  try {
    const stats = await webpack(webpackConfig)

    // Print errors, warnings or info from the build.
    const statsJson = stats.toJson({  // eslint-disable-line no-console
      all: false,
      errors: true,
      modules: true,
      maxModules: 0,
      warnings: true,
      timings: true,
    })
    statsJson.warnings.forEach(
      e => console.warn(  // eslint-disable-line no-console
        `${YELLOW}WARNING:${CLEAR} ${e.moduleName}:${e.loc}:\n${e.message}`
      )
    )
    statsJson.errors.forEach(
      e => console.error(  // eslint-disable-line no-console
        `${RED}ERROR:${CLEAR} ${e.moduleName}:${e.loc}:\n${e.message}`
      )
    )

    if (statsJson.errors.length) {
      err = 1
    } else {
      for (const postBuildAction of postBuildActions) {
        // eslint-disable-next-line no-await-in-loop
        await postBuildAction()
      }
    }
  } catch (e) {
    console.error('Webpack runtime error: ', e)  // eslint-disable-line no-console
    err = 1
  }

  if (err) {
    process.exit(err)
  }
}

// Get the command line arguments and run webpack.
const args = getArgs()

runWebpack(genConfig({
  outFile: args.outpattern,
  mainFiles: args.entries.split(','),
  cwdPath: cwd,
  target: args.target,
  includeWebTypes: args.includewebtypes,
  includes: args.includes,
  mode: args.mode,
  commonjs: args.commonjs,
  npmRule: args.npmrule,
  analyze: !!args.analyze,
  sourceMap: !!args.sourcemap,
  headerPath: args.headerpath,
  footerPath: args.footerpath,
  mangle: !!args.mangle,
  polyfill: args.polyfill,
  exportLibrary: args.exportLibrary,
  externals: args.externals,
  externalsType: args.externalsType,
  stringReplacements: args.stringReplacements,
  esnext: !!args.esnext,
  tsDeclaration: !!args.tsDeclaration,
  fullDts: !!args.fullDts,
}))
