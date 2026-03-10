// @attr[](data = ":type-only")

import {promises as fs} from 'fs'

import {DATA} from '@repo/bzl/examples/js/import/type-only-lib'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

describe('type only import', () => {
  it('Does not contain direct import of lib', async () => {
    const buildContents = await fs.readFile('bzl/examples/js/import/type-only.js', 'utf8')
    assert.isFalse(buildContents.includes(DATA))
  })
})
