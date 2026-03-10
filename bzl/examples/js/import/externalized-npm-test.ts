// @package(npm-webpack-build)
// @attr(externalize_npm = True)

import webpack from 'webpack'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

describe('webpack', () => {
  it('exposes the expected API', async () => {
    assert.containsAllKeys(webpack, [
      'HotModuleReplacementPlugin',
      'NormalModuleReplacementPlugin',
      'SourceMapDevToolPlugin',
      'version',
    ])
  })
})
