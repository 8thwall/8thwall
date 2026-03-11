/* eslint-disable no-console */
import fs from 'fs'
import path from 'path'
// NOTE(christoph) Gulp doesn't handle child process correctly, so use the runChildProcess instead
import webpack from 'webpack'
import {BundleAnalyzerPlugin} from 'webpack-bundle-analyzer'
import {src, dest, series, parallel} from 'gulp'
import del from 'del'
import ReactRefreshWebpackPlugin from '@pmmmwh/react-refresh-webpack-plugin'
import WebpackDevServer from 'webpack-dev-server'

import workerConfig from './webpack.worker'
import desktopConfig from './webpack.desktop'
import {BuildIfEnv, getBuildIfReplacements} from './src/shared/buildif'
import type {Build8Replacements} from './src/shared/build8'
import {CODE8, runChildProcess} from './tools/gulp/process'
import {wrapBazel} from './tools/gulp/bazel'
import {createLiveBuildIfReplacements} from './tools/gulp/buildif-watch'

const DESKTOP_OUTPUT_DIR = 'desktop-dist'

const deploymentHash = Array.from({length: 8})
  .map(() => Math.floor(Math.random() * 16).toString(16))
  .join('')

const deploymentPath = `static/deployment/${deploymentHash}`

let desktopBundle
let desktopWorkerBundle

let isProduction = false
let isDesktop = false
const mainExtraPlugins = []
const workerExtraPlugins = []

const config = async () => {
  if (!isProduction) {
    console.log('Not in production. Source maps enabled.')
  }

  const isLocalDev = !isProduction
  const isRemoteDev = !!(process.env.XRHOME_ENV && process.env.XRHOME_ENV === 'Console-dev')

  // Get buildif from the enviornment if present.
  const envBuildIf = (process.env.BuildIf && JSON.parse(process.env.BuildIf)) || {}

  // Generate a list of textual string replacements to overwrite process.env.NODE_ENV and all
  // BuildIf references. This is needed due to limitations in fuse and is expected to
  // go away after switching to webpack.
  const replacements = []
  const setReplacement = (search, value, stringifyValue = true) => {
    const replace = stringifyValue ? JSON.stringify(value) : value
    replacements.push({search, replace, flags: 'g'})
  }

  const buildIf: BuildIfEnv = {
    isLocalDev,
    isRemoteDev,
    isTest: false,
    flagLevel: process.env.BUILDIF_FLAG_LEVEL as any || 'experimental',
  }

  if (isLocalDev) {
    replacements.push(...createLiveBuildIfReplacements(buildIf, envBuildIf))
  } else {
    getBuildIfReplacements(buildIf, envBuildIf).forEach(({flag, value}) => {
      setReplacement(`BuildIf.${flag}`, value)
    })
  }

  // Catch invalid usage of BuildIf by inserting a syntax error
  setReplacement('BuildIf', '# BuildIf Error: Missing/invalid flag #', false)

  setReplacement('process.env.NODE_ENV', isProduction ? 'production' : 'development')
  // NOTE: the Build8 replacements are for console-client only!
  // DO NOT USE IN SERVERS WHERE process.env IS AVAILABLE
  const Build8: Build8Replacements = {
    VERSION_ID: deploymentHash,
    EXECUTABLE_NAME: 'console-client',
    DEPLOYMENT_PATH: deploymentPath,
    PLATFORM_TARGET: isDesktop ? 'desktop' : 'web',
  }

  Object.keys(Build8).forEach((key) => {
    setReplacement(`Build8.${key}`, Build8[key])
  })

  // NOTE: DEPLOYMENT_STAGE is dynamically deduced and defined in src/client/common/index.ts

  const webpackEnv = {
    isProduction,
    isLocalDev,
    deploymentPath,
    replacements,
    // This option allows webpack to use the true location of files, for example resolving
    // @ecs/shared/something to c8/ecs/src/shared/something, rather than using the symlink
    // location of xrhome/src/shared/ecs/shared. This is good for local development as changes
    // to c8/ecs/src/shared/something will be picked up by the watcher. Otherwise the stale content
    // will be used.
    // If a production build were to run with this enabled, webpack would attempt to
    // resolve dependencies to c8/ecs/node_modules rather than xrhome's which would not work,
    // as the runner only has xrhome's node_modules installed.
    allowSymlinkToExternal: isLocalDev,
  }

  if (isLocalDev) {
    mainExtraPlugins.push(
      new webpack.EvalSourceMapDevToolPlugin({})
    )

    if (process.env.HOT === '1') {
      mainExtraPlugins.push(new ReactRefreshWebpackPlugin({
        overlay: false,
      }))
    }

    workerExtraPlugins.push(new webpack.EvalSourceMapDevToolPlugin({}))
  } else {
    mainExtraPlugins.push(new webpack.SourceMapDevToolPlugin({
      filename: `sourcemaps/console-client/${deploymentHash}/[name].bundle.js.map`,
      test: /\.(js|jsx|css|ts|tsx)($|\?)/i,
      exclude: /node_modules/,
      append: false,
    }))
    workerExtraPlugins.push(new webpack.SourceMapDevToolPlugin({
      filename: `sourcemaps/console-client/${deploymentHash}/[name].js.map`,
      test: /\.(js|jsx|css|ts|tsx)($|\?)/i,
      exclude: /node_modules/,
      append: false,
    }))
  }

  desktopBundle = webpack(desktopConfig({...webpackEnv, extraPlugins: mainExtraPlugins}))
  desktopWorkerBundle = webpack(workerConfig({
    ...webpackEnv,
    deploymentPath: '',
    distPath: `${DESKTOP_OUTPUT_DIR}/worker`,
    extraPlugins: workerExtraPlugins,
  }))
}

const getTimestamp = () => {
  const d = new Date()
  return `[${d.toTimeString().split(' ')[0]}]`
}

const logUnexpectedError = (err) => {
  console.error('An unexpected error occured (not a webpack error):\n', err)
}

const logWebpackError = (stats) => {
  console.error('Webpack encountered an error:\n', stats.toString())
}

const hotReloadBundle = (bundle: webpack.Compiler, port: number) => new Promise<void>((resolve) => {
  const server = new WebpackDevServer({
    port,
    host: 'localhost',
    liveReload: false,
    hot: true,
    server: 'https',
    historyApiFallback: true,
    client: {
      overlay: false,
    },
    allowedHosts: 'all',
    devMiddleware: {
      writeToDisk: true,
    },
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
      'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
    },
  }, bundle)

  server.start()
  resolve()
})

const watchBundle = bundle => new Promise<void>((resolve, reject) => {
  let alreadyResolved = false
  bundle.watch(undefined, (err, stats) => {
    if (err) {
      logUnexpectedError(err)
      reject(err)
      return
    }

    if (stats.hasErrors()) {
      logWebpackError(stats)
      if (!alreadyResolved) {
        reject()
      }
      return
    }

    if (stats.hasWarnings()) {
      console.warn('Webpack built with warnings:\n', stats.toString())
    } else if (alreadyResolved) {
      console.log(`${getTimestamp()} Webpack rebuilt successfully.`)
    }

    if (!alreadyResolved) {
      resolve()
      alreadyResolved = true
    }
  })
})

const buildSemantic = async () => {
  await runChildProcess('npm ci', {cwd: './semantic', stdio: 'ignore'})
  await runChildProcess('gulp build', {cwd: './semantic', stdio: 'ignore'})
}

const throwIfNotInSparseCheckout = (pathInCode8: string) => {
  if (!fs.existsSync(path.join(CODE8, pathInCode8))) {
    throw new Error(`You need to add ${pathInCode8} to your sparse checkout`)
  }
}

// TODO(J8W-2057) Remove this after switching to remote debugging.
const prepareEcs = async () => {
  throwIfNotInSparseCheckout('/c8/ecs/')
  await wrapBazel(() => runChildProcess('npm run ecs:prepare', {cwd: path.join(CODE8, '/c8/ecs')}))
}

const copySemantic = () => (
  src('./semantic/dist/**').pipe(dest('./src/client/static/semantic/dist'))
)

const clean = () => del([
  DESKTOP_OUTPUT_DIR,
  'src/client/static/semantic',
])

const prodEnv = async () => {
  isProduction = true
}

const desktopEnv = async () => {
  isDesktop = true
}

const runBundle = bundle => new Promise<void>((resolve, reject) => {
  bundle.run((err, stats) => {
    if (err) {
      logUnexpectedError(err)
      reject(err)
      return
    }

    if (stats.hasErrors()) {
      logWebpackError(stats)
      reject()
    } else {
      resolve()
    }
  })
})

const buildFromBazel = series(
  prepareEcs
)

const buildDesktop = () => runBundle(desktopBundle)
const buildDesktopWorker = () => runBundle(desktopWorkerBundle)

const hotReloadDesktop = () => hotReloadBundle(desktopBundle, 3602)
const watchDesktop = () => watchBundle(desktopBundle)

const copyEcsResources = () => (
  // TODO(christoph): Add additional resources as needed.
  src('../../../c8/ecs/resources/fonts/*/**')
    .pipe(dest(`${DESKTOP_OUTPUT_DIR}/ecs-resources/fonts`))
)

const desktop = series(
  clean,
  desktopEnv,
  config,
  parallel(
    buildFromBazel,
    series(buildSemantic, copySemantic)
  ),
  copyEcsResources,
  buildDesktopWorker,
  process.env.HOT === '1' ? hotReloadDesktop : watchDesktop
)

const distDesktop = series(
  clean,
  desktopEnv,
  prodEnv,
  config,
  parallel(
    buildFromBazel,
    series(buildSemantic, copySemantic)
  ),
  copyEcsResources,
  buildDesktopWorker,
  buildDesktop
)

const injectBundleAnalyzer = (extraPluginTarget: unknown[]) => async () => {
  const bundleAnalyzerPlugin = new BundleAnalyzerPlugin()
  extraPluginTarget.push(bundleAnalyzerPlugin)
}

const bundleAnalyzeDesktop = series(injectBundleAnalyzer(mainExtraPlugins), distDesktop)
const bundleAnalyzeDesktopWorker = series(injectBundleAnalyzer(workerExtraPlugins), distDesktop)

// Public tasks available to our cli users
export {
  clean,
  buildFromBazel,
  bundleAnalyzeDesktop,
  bundleAnalyzeDesktopWorker,
  desktop,
  distDesktop,
}
