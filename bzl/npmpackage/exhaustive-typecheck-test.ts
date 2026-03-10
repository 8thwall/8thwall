// @attr[](data = "//:WORKSPACE")
// @attr[](data = "//bzl/npmpackage:BUILD")
// @attr(esnext = True)

import {promises as fs} from 'fs'
import path from 'path'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

const runfiles = process.env.RUNFILES_DIR!

describe('Typecheck rules`', () => {
  it('covers all npm-xxx rules present in WORKSPACE', async () => {
    const WORKSPACE = await fs.readFile(path.join(runfiles, '_main/WORKSPACE'), 'utf8')
    const BUILD = await fs.readFile(path.join(runfiles, '_main/bzl/npmpackage/BUILD'), 'utf8')
    const names = [...WORKSPACE.matchAll(/name = "npm-(.*)",/g)].map(([, name]) => name)
    assert.isNotEmpty(names)

    for (const name of names) {
      assert.include(BUILD, `typechecked("${name}")`)
    }
  })
})
