const path = require('path')
const TerserPlugin = require('terser-webpack-plugin')

const rules = [
  {
    test: /\.js$/,
    exclude: /(node_modules)/,
    use: {
      loader: 'babel-loader',
      options: {
        presets: [
          '@babel/preset-env',
        ],
      },
    },
  },
  {
    test: /\.ts$/,
    exclude: /node_modules/,
    use: [{
      loader: 'ts-loader',
      options: {onlyCompileBundledFiles: true},
    }],
  },
]

module.exports = (ctx) => {
  const useSourcemaps = !!(ctx.dev || ctx.WEBPACK_SERVE)

  return {
    entry: {
    // A version of the runtime that can be integrated into existing threejs scenes
      plugin: './src/runtime/plugin-entry.ts',
      // The standalone version of the runtime that can be used in Studio projects
      runtime: './src/runtime/runtime-entry.ts',
    },
    output: {
      filename: '[name].js',
      path: path.resolve(__dirname, 'dist'),
    },
    devServer: {
      compress: true,
      port: 8004,
      https: true,
      hot: true,
      liveReload: false,
      headers: {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
        'Access-Control-Allow-Headers': 'X-Requested-With, content-type, Authorization',
      },
    },
    devtool: useSourcemaps ? 'source-map' : false,
    externals: ['fs'],
    optimization: {
      minimize: !useSourcemaps,
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
      alias: {
        '@nia/c8/ecs': __dirname,
      },
    },
    module: {rules},
    mode: 'production',
  }
}
