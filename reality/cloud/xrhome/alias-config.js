const {resolve} = require('path')

const tsconfig = require('./tsconfig.json')

// path/to/file/* -> path/to/file
const trimTrailingWildcard = str => str.replace(/\/\*$/, '')

// https://www.npmjs.com/package/eslint-import-resolver-custom-alias#configuration
// e.g. the following config
//    "paths": {
//      "example/*": ["./src/example/*"]
//    },
// would map to [["example", "/path/to/src/example"]]
const getEslintMap = () => (
  Object.entries(tsconfig.compilerOptions.paths)
    .map(([alias, target]) => [
      trimTrailingWildcard(alias),
      resolve(__dirname, trimTrailingWildcard(target[0])),
    ])
)

// https://webpack.js.org/configuration/resolve/#resolvealias
// e.g. the following config
//    "paths": {
//      "example/*": ["./src/example/*"]
//    },
// would map to {"example", "/path/to/src/example"}
const getWebpackAliases = () => (
  Object.fromEntries(getEslintMap())
)

module.exports = {
  getEslintMap,
  getWebpackAliases,
}
