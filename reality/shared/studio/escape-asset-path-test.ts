import {describe, it, assert} from '@repo/bzl/js/chai-js'

import {escapeAssetPath} from './escape-asset-path'

describe('escapeAssetPath', () => {
  it('throws errors on invalid paths', () => {
    assert.throws(() => escapeAssetPath(''), 'Invalid asset path: ""')
    assert.throws(() => escapeAssetPath('foo/bar'), 'Invalid asset path: "foo/bar"')
    assert.throws(() => escapeAssetPath('assets/foo'), 'Invalid asset path: "assets/foo"')
  })

  it('Leaves normal paths unchanged', () => {
    assert.strictEqual(escapeAssetPath('/assets/foo'), '/assets/foo')
    assert.strictEqual(escapeAssetPath('/assets/foo-123-ABC.png'), '/assets/foo-123-ABC.png')
  })

  it('Handles space, plus, and special characters', () => {
    assert.strictEqual(escapeAssetPath('/assets/foo bar'), '/assets/foo%20bar')
    assert.strictEqual(escapeAssetPath('/assets/foo+bar'), '/assets/foo%2Bbar')
    assert.strictEqual(escapeAssetPath('/assets/foo%bar'), '/assets/foo%25bar')
    assert.strictEqual(escapeAssetPath('/assets/foo@bar'), '/assets/foo%40bar')
  })

  it('Leaves slashes unchanged', () => {
    assert.strictEqual(escapeAssetPath('/assets/foo/bar'), '/assets/foo/bar')
    assert.strictEqual(escapeAssetPath('/assets/foo/bar/baz'), '/assets/foo/bar/baz')
  })

  it('Handles unicode characters', () => {
    assert.strictEqual(escapeAssetPath('/assets/言葉.svg'), '/assets/%E8%A8%80%E8%91%89.svg')
    assert.strictEqual(escapeAssetPath('/assets/foo/emoji/😀'), '/assets/foo/emoji/%F0%9F%98%80')
  })
})
