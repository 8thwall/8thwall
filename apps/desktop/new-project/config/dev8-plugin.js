const HtmlWebpackPlugin = require('html-webpack-plugin')

const createDev8Plugin = ({src}) => ({
  apply: (compiler) => {
    if (!src) {
      throw new Error('createDev8Plugin called without src')
    }
    compiler.hooks.compilation.tap('Dev8Plugin', (compilation) => {
      HtmlWebpackPlugin.getCompilationHooks(compilation).beforeEmit.tap(
        'Dev8Plugin',
        (data) => {
          data.html = data.html.replace(
            '</head>',
            `  <script crossorigin="anonymous" src="${src}"></script>\n</head>`
          )
          return data
        }
      )
    })
  },
})

module.exports = createDev8Plugin
