const path = require('path')
const TerserPlugin = require('terser-webpack-plugin')
const webpack = require('webpack')

const banner = `
8th Wall Developer Tools

This script connects development (unpublished) versions of your app to the editor interface on your
8thwall.com account. It enables rapid development, debugging, and iteration of immersive content.
It is for development purposes only and it is not included in published versions of your app.
`

const rules = [
  {
    test: /\.(j|t)s$/,
    exclude: /(node_modules|bower_components)/,
    use: {
      loader: 'babel-loader',
      options: {
        presets: [
          '@babel/preset-env',
          '@babel/preset-typescript',
        ],
      },
    },
  },
  {test: /\.css$/, use: ['style-loader', 'css-loader']},
  {test: /\.html$/, use: {loader: 'html-loader'}},
]

module.exports = {
  entry: './src/dev8.ts',
  output: {
    filename: 'dev8.js',
    path: path.resolve(__dirname, 'dist'),
  },
  plugins: [
    new webpack.BannerPlugin(banner),
    new webpack.SourceMapDevToolPlugin({
      append: '//# sourceURL=dev8.js',
    }),
    new webpack.ProvidePlugin({
      process: 'process/browser',
    }),
  ],
  devServer: {
    compress: true,
    port: 9000,
    https: true,
    hot: false,
    liveReload: false,
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
      'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
    },
  },
  externals: ['fs'],
  optimization: {
    minimize: true,
    minimizer: [
      new TerserPlugin({
        extractComments: false,
        terserOptions: {
          format: {
            comments: 'some',
          },
        },
      }),
    ],
  },
  resolve: {
    extensions: ['.ts', '.js', '.json'],
    fallback: {
      'path': require.resolve('path-browserify'),
    },
    alias: {
      '@repo/reality/app/xr/js/src': path.resolve(__dirname, './src/shared/xr/'),
      '@repo/c8/ecs/src': path.resolve(__dirname, './src/shared/ecs/'),
    },
    symlinks: false,
  },
  module: {rules},
  mode: 'production',
  experiments: {
    syncWebAssembly: true,
  },
}
