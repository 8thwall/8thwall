// @rule(js_binary)
// @attr(target = "node")
// @package(npm-webpack-build)
// @attr(externals = "webpack, @babel/*")

import * as webpack from 'webpack'
import * as babelCore from '@babel/core'

console.log(webpack, babelCore)
