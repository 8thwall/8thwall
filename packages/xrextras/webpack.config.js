const path = require('path')
const CopyPlugin = require('copy-webpack-plugin')

module.exports = {
  entry: {
    xrextras: './src/index.js',
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
        test: /\.ts$/,
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
      ],
    }),
  ],
  mode: 'production',
  resolve: {
    extensions: ['.ts', '.js'],
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
