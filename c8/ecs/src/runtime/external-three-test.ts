// @attr[](data = "//c8/ecs/src/runtime")
// @attr[](data = "//c8/ecs/src/runtime:plugin")

import {describe, it, assert} from '@nia/bzl/js/chai-js'
import path from 'path'
import {promises as fs} from 'fs'

const pluginPath = path.resolve(process.env.RUNFILES_DIR!, '_main/c8/ecs/src/runtime/plugin.js')
const runtimePath = path.resolve(process.env.RUNFILES_DIR!, '_main/c8/ecs/src/runtime/runtime.js')

const assertIncludesThree = (content: string, expected: boolean) => {
  // NOTE(christoph): This is some of text that would appear in builds that include threejs
  const actual = content.includes('__THREE_DEVTOOLS__')
  assert.equal(actual, expected)
}

describe('plugin binary', () => {
  it('does not contain threejs', async () => {
    const pluginContent = await fs.readFile(pluginPath, 'utf-8')
    assertIncludesThree(pluginContent, false)
  })
})

describe('runtime binary', () => {
  it('contains threejs', async () => {
    const runtimeContent = await fs.readFile(runtimePath, 'utf-8')
    assertIncludesThree(runtimeContent, true)
  })
})
