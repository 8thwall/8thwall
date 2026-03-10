// @attr[](data = ":class-in-js")
// @attr[](data = ":class-example-es6")

import {promises as fs} from 'fs'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

describe('class statements in ts', () => {
  it('are left as-is', async () => {
    const buildContents = await fs.readFile('bzl/examples/js/syntax/class-example-es6.js', 'utf8')
    assert.isTrue(buildContents.includes('class MyClass'))
  })
})

describe('class statements in js', () => {
  it('are left as-is', async () => {
    const buildContents = await fs.readFile('bzl/examples/js/syntax/class-in-js.js', 'utf8')
    assert.isTrue(buildContents.includes('class MyClass'))
  })
})
