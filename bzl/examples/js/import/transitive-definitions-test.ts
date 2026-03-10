// @attr[](data = ":transitive-definitions")

import {promises as fs} from 'fs'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

describe('transitive definitions with full_dts', () => {
  it('Contains type declarations from transitive deps', async () => {
    const buildContents = await fs.readFile(
      'bzl/examples/js/import/transitive-definitions.d.ts', 'utf8'
    )
    assert.include(buildContents, 'interface TypeImportedFromDep')
    assert.include(buildContents, 'exampleField: string')
  })

  it('Contains variable declarations from transitive deps', async () => {
    const buildContents = await fs.readFile(
      'bzl/examples/js/import/transitive-definitions.d.ts', 'utf8'
    )
    assert.include(buildContents, 'dataImportedFromDep: string')
  })

  it('Contains global declarations', async () => {
    const buildContents = await fs.readFile(
      'bzl/examples/js/import/transitive-definitions.d.ts', 'utf8'
    )
    assert.include(buildContents, 'declare global {')
    assert.include(buildContents, 'interface MySharedInterface')
    assert.include(buildContents, 'sharedProperty1: string')
    assert.include(buildContents, 'sharedProperty2: string')
  })
})
