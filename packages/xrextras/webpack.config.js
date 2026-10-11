const path = require('path')
const CopyPlugin = require('copy-webpack-plugin')

const TESTING = process.env.WEBPACK_SERVE === 'true'

module.exports = {
  entry: {
    xrextras: './src/index.js',
    'startflow-debug': './test/startflow/index.tsx',
  },
  output: {
    filename: '[name].js',
    path: path.resolve(__dirname, 'dist'),
    publicPath: 'auto',
  },
  module: {
    rules: [
      {
        test: /\.(png|svg|jpg|jpeg|gif|ico|woff|ttf|min\.js)$/,
        exclude: /node_modules/,
        type: 'asset/resource',
        generator: {
          filename: 'resources/[name]-[hash][ext]',
        },
      },
      {
        test: /\.module\.scss$/,
        use: [
          'style-loader',
          {
            loader: 'css-loader',
            options: {
              modules: {
                localIdentName: TESTING
                  ? 'xrextras_[local]_[hash:base64:8]'
                  : 'xrextras_[hash:base64:8]',
              },
            },
          },
          'sass-loader',
        ],
      },
      {
        test: /\.css$/,
        use: [
          'style-loader',
          'css-loader',
        ],
      }, {
        test: /\.html$/,
        use: {
          loader: 'html-loader',
        },
      },
      {
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
    ],
  },
  performance: {
    hints: false,
    maxEntrypointSize: 512000,
    maxAssetSize: 512000,
  },
  plugins: [
    new CopyPlugin({
      patterns: [
        {from: 'LICENSE', to: '.'},
        {from: 'test/index.html', to: 'test/index.html'},
        {from: 'test/index-aframe.html', to: 'test/index-aframe.html'},
        {from: 'test/index.js', to: 'test/index.js'},
        {from: 'test/startflow/index.html', to: 'test/startflow/index.html'},
      ],
    }),
  ],
  mode: 'production',
  resolve: {
    extensions: ['.ts', '.tsx', '.js'],
  },
  devServer: {
    static: path.join(__dirname, 'dist'),
    compress: true,
    port: 9000,
    https: true,
    hot: false,
    host: '0.0.0.0',
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
      'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
    },
  },
}
