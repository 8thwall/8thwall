const path = require('path')
const {BundleAnalyzerPlugin} = require('webpack-bundle-analyzer')

const ANALYZE_BUNDLE = false
const DEVELOPMENT = false

const devBuildMode = DEVELOPMENT
  ? ({
    mode: 'development',
    devtool: 'eval-source-map',
  }) : ({
    mode: 'production',
  })

module.exports = {
  entry: {
    'coaching-overlay': './src/index.ts',
  },
  output: {
    filename: '[name].js',
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
          'css-loader',
          'sass-loader',
        ],
      },
      {
        test: /\.html$/,
        use: 'html-loader',
      },
      // Needed for loading the webm/mp4 file of the coaching animation.  This may be switched
      // out for an SVG later.
      {
        test: /\.(png|webm|mp4)$/i,
        use: [
          {
            loader: 'url-loader',
            options: {
              limit: 100000,
            },
          },
        ],
      },
    ],
  },
  plugins: ANALYZE_BUNDLE ? [new BundleAnalyzerPlugin()] : [],
  devServer: {
    port: 9003,
    // Add your own IP here for local development.
    host: '192.168.1.211',
    https: true,
    hot: true,
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
      'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
    },
    allowedHosts: ['.8thwall.app'],
  },
  ...devBuildMode,
}
