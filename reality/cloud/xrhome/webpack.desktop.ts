import path from 'path'
import TerserPlugin from 'terser-webpack-plugin'
import {ProvidePlugin, type Configuration} from 'webpack'
import HtmlWebpackPlugin from 'html-webpack-plugin'

import {getWebpackAliases} from './alias-config'
import {makeNoExternalImportsPlugin} from './config/no-external-imports-plugin'

export default ({
  isLocalDev, isProduction, replacements, extraPlugins, allowSymlinkToExternal,
}): Configuration => ({
  entry: {
    desktop: './src/client/desktop/index.tsx',
  },
  output: {
    publicPath: (isLocalDev && process.env.HOT === '1')
      ? 'https://localhost:3602/'
      : 'desktop://dist/',
    path: path.join(__dirname, 'desktop-dist'),
    filename: path.join('[name].bundle.js'),
    hashFunction: 'sha512',
  },
  resolve: {
    extensions: ['.ts', '.tsx', '.js', '.jsx', '.json'],
    mainFields: ['module', 'browser', 'main'],
    symlinks: allowSymlinkToExternal,
    modules: allowSymlinkToExternal
      ? ['node_modules', path.resolve(__dirname, 'node_modules')]
      : ['node_modules'],
    alias: getWebpackAliases(),
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
        'test': /\.worker\.js$/,  // for unconify.worker.js
        'use': 'worker-loader',
      },
      {
        test: /\.(j|t)sx?$/,
        exclude: [/node_modules/],
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
                // eslint-disable-next-line local-rules/commonjs
                isLocalDev && process.env.HOT === '1' && require.resolve('react-refresh/babel'),
              ].filter(Boolean),
              presets: [
                '@babel/preset-env',
                '@babel/preset-react',
                ['@babel/preset-typescript', {allowNamespaces: true}],
              ],
              sourceMap: true,
            },
          },
        ],
      },
      {
        test: /\.scss$/,
        use: ['style-loader', 'css-loader', 'sass-loader'],
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader'],
      },
      {
        test: /\.md$/,
        use: 'raw-loader',
      },
      {
        test: /\.(png|webp|svg|jpg|gif|ico|woff|woff2|eot|ttf|otf|mp4|webm)$/,
        loader: 'file-loader',
        options: {
          name: 'static/asset/[contenthash:hex:8]-[name].[ext]',
        },
      },
    ],
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: './src/client/desktop/index.html',
      filename: 'index.html',
    }),
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
  experiments: {
    syncWebAssembly: true,
  },
})
