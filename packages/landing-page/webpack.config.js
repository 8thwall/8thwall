const path = require('path')
const {BundleAnalyzerPlugin} = require('webpack-bundle-analyzer')
const CopyPlugin = require('copy-webpack-plugin')

const ANALYZE_BUNDLE = false

module.exports = {
  entry: {
    'test': './test/index.tsx',
    'landing-page': './src/index.ts',
  },
  output: {
    filename: 'external/landing-page/[name].js',
    path: path.resolve(__dirname, 'dist'),
  },
  resolve: {
    extensions: ['.ts', '.tsx', '.js', '.jsx', '.json'],
  },
  module: {
    rules: [
      {
        test: /\.(j|t)sx?$/,
        exclude: [/node_modules/],
        use: [
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
                ['transform-react-jsx', {'pragma': 'React.h', 'pragmaFrag': 'React.Fragment'}],
              ],
              presets: [
                '@babel/preset-env',
                '@babel/preset-typescript',
              ],
            },
          },
        ],
      },
      {
        test: /\.s?css$/,
        use: [
          'style-loader',
          {loader: 'css-loader', options: {url: false}},
          'sass-loader',
        ],
      },
      {
        test: /\.html$/,
        use: {
          loader: 'html-loader',
          options: {sources: false},
        },
      },
    ],
  },
  performance: {
    hints: false,
    maxEntrypointSize: 512000,
    maxAssetSize: 512000,
  },
  plugins: ANALYZE_BUNDLE
    ? [new BundleAnalyzerPlugin()]
    : [
      new CopyPlugin({
        patterns: [
          {from: 'resources', to: 'external/landing-page/resources'},
          {from: '../xrextras/xrextras-shared-resources', to: 'external/xrextras-shared-resources'},
          {from: 'test/index.html', to: 'index.html'},
          {from: 'LICENSE', to: 'external/landing-page/'},
        ],
      }),
    ],
  mode: 'production',
  devServer: {
    port: 9002,
    server: 'https',
    // Add your own IP here for local development.
    // host: 'x.x.x.x',
    hot: true,
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
      'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
    },
    allowedHosts: ['.8thwall.app'],
  },
}
