// @rule(js_binary)
// @package(npm-eslint)
// @attr(export_library = 1)
// @attr(externals = "eslint")
// @attr(target = "node")
// @attr(commonjs = 1)

import Eslint from 'eslint'

const doThing = () => {
  console.log('Eslint is version: ', Eslint.CLIEngine.version)
}

export {
  doThing,
}
