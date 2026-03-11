import path from 'path'
import TerserPlugin from 'terser-webpack-plugin'
import {ProvidePlugin, type Configuration} from 'webpack'

import {makeNoExternalImportsPlugin} from './config/no-external-imports-plugin'

export default ({
  isProduction, deploymentPath, distPath = 'dist', replacements, extraPlugins = [],
  allowSymlinkToExternal,
}): Configuration => ({
  entry: {
    analysis: './src/client/studio/worker/analysis-worker.ts',
    gltfTransform: './src/client/studio/worker/gltf-transform-worker.ts',
  },
  output: {
    publicPath: '/',
    path: path.join(__dirname, distPath),
    filename: path.join(deploymentPath, 'client/[name]-worker.js'),
    hashFunction: 'sha512',
  },
  resolve: {
    extensions: ['.ts', '.js', '.wasm', '.cjs', '.mjs'],
    symlinks: allowSymlinkToExternal,
    modules: allowSymlinkToExternal
      ? ['node_modules', path.resolve(__dirname, 'node_modules')]
      : ['node_modules'],
    alias: {
      esquery: path.resolve(__dirname, 'node_modules/esquery/dist/esquery.js'),
    },
    fallback: {
      /* eslint-disable local-rules/commonjs */
      assert: require.resolve('assert'),
      buffer: require.resolve('buffer'),
      console: require.resolve('console-browserify'),
      constants: require.resolve('constants-browserify'),
      crypto: require.resolve('crypto-browserify'),
      domain: require.resolve('domain-browser'),
      events: require.resolve('events'),
      http: require.resolve('stream-http'),
      https: require.resolve('https-browserify'),
      os: require.resolve('os-browserify/browser'),
      path: require.resolve('path-browserify'),
      punycode: require.resolve('punycode'),
      process: require.resolve('process/browser'),
      querystring: require.resolve('querystring-es3'),
      stream: require.resolve('stream-browserify'),
      string_decoder: require.resolve('string_decoder'),
      sys: require.resolve('util'),
      timers: require.resolve('timers-browserify'),
      tty: require.resolve('tty-browserify'),
      url: require.resolve('url'),
      util: require.resolve('util'),
      vm: require.resolve('vm-browserify'),
      zlib: require.resolve('browserify-zlib'),
      /* eslint-enable local-rules/commonjs */
    },
  },
  module: {
    rules: [
      {
        test: /\.(j|t)s$/,
        exclude: /node_modules/,
        use: [
          {
            loader: 'string-replace-loader',
            options: {multiple: replacements},
          },
          {
            loader: 'babel-loader',
            options: {
              cacheDirectory: true,
              plugins: [
                '@babel/plugin-proposal-export-default-from',
                '@babel/plugin-proposal-class-properties',
                '@babel/plugin-proposal-export-namespace-from',
                '@babel/plugin-transform-modules-commonjs',
                '@babel/plugin-transform-runtime',
                '@babel/plugin-transform-object-assign',
                '@babel/plugin-syntax-dynamic-import',
                '@babel/plugin-proposal-object-rest-spread',
                '@babel/plugin-proposal-optional-chaining',
              ],
              presets: [
                '@babel/preset-env',
                '@babel/preset-typescript',
              ],
              sourceMap: true,
            },
          },
        ],
      },
      {
        test: /\.wasm$/,
        loader: 'file-loader',
        options: {
          name: path.join(deploymentPath, '[name].[ext]'),
        },
        type: 'javascript/auto',
      },
    ],
  },
  plugins: [
    new ProvidePlugin({
      process: 'process/browser',
    }),
    new ProvidePlugin({
      Buffer: ['buffer', 'Buffer'],
    }),
    allowSymlinkToExternal ? null : makeNoExternalImportsPlugin(__dirname),
    ...extraPlugins,
  ].filter(Boolean),
  mode: isProduction ? 'production' : 'development',
  optimization: {
    minimize: isProduction,
    minimizer: [new TerserPlugin({
      terserOptions: {
        sourceMap: true,
      },
    })],
  },
})
