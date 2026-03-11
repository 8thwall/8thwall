import {describe, it} from 'mocha'
import {assert} from 'chai'

import {
  deriveEditorRouteParams,
  editorFileLocationEqual, extractFilePath, extractRepoId, extractScopedLocation,
  deriveLocationKey,
  resolveEditorFileLocation,
  stripPrimaryRepoId,
  deriveLocationFromKey,
} from '../src/client/editor/editor-file-location'

describe('extractFilePath', () => {
  it('Returns path directly for unscoped location', () => {
    assert.strictEqual(extractFilePath('my/path.ts'), 'my/path.ts')
  })
  it('Returns path for scoped location', () => {
    assert.strictEqual(extractFilePath({filePath: 'my/path.ts'}), 'my/path.ts')
  })
  it('Returns empty string for missing locations', () => {
    assert.strictEqual(extractFilePath(''), '')
    assert.strictEqual(extractFilePath({filePath: ''}), '')
    assert.strictEqual(extractFilePath(null), '')
  })
})

describe('extractRepoId', () => {
  it('Returns null for unscoped', () => {
    assert.strictEqual(extractRepoId('my/path.ts'), null)
  })
  it('Returns repo ID for scoped location with repo ID', () => {
    assert.strictEqual(extractRepoId({filePath: 'my/path.ts', repoId: 'my-repo-id'}), 'my-repo-id')
  })
  it('Returns null for scoped location without repo ID', () => {
    assert.strictEqual(extractRepoId({filePath: 'my/path.ts'}), null)
  })
  it('Returns null for missing repo ID', () => {
    assert.strictEqual(extractRepoId({filePath: 'my-file.ts'}), null)
    assert.strictEqual(extractRepoId('my-file.ts'), null)
    assert.strictEqual(extractRepoId(null), null)
  })
})

describe('editorFileLocationEqual', () => {
  it('Returns true for equal locations', () => {
    assert.isTrue(editorFileLocationEqual('my/path.ts', 'my/path.ts'))
  })
  it('Returns true for falsy locations', () => {
    assert.isTrue(editorFileLocationEqual('', null))
  })
  it('Returns true for equal scoped locations', () => {
    assert.isTrue(editorFileLocationEqual({filePath: 'my/path.ts'}, {filePath: 'my/path.ts'}))
  })
  it('Returns true for a scoped an unscoped location whe the scoped location is falsy', () => {
    assert.isTrue(editorFileLocationEqual('my/path.ts', {filePath: 'my/path.ts', repoId: null}))
  })
  it('Returns true for equal scoped locations with falsy repoIds', () => {
    assert.isTrue(editorFileLocationEqual(
      {filePath: 'my/path.ts', repoId: null},
      {filePath: 'my/path.ts'}
    ))
  })
  it('Returns true for falsy scoped locations', () => {
    assert.isTrue(editorFileLocationEqual({filePath: ''}, {filePath: null}))
    assert.isTrue(editorFileLocationEqual('', null))
  })
  it('Returns false for different file paths', () => {
    assert.isFalse(editorFileLocationEqual(
      {filePath: 'other/path.ts', repoId: null},
      {filePath: 'my/path.ts'}
    ))
    assert.isFalse(editorFileLocationEqual(
      'other/path.ts',
      {filePath: 'my/path.ts'}
    ))
  })
  it('Returns false for the same file in different scoped locations', () => {
    assert.isFalse(editorFileLocationEqual(
      'my/path.ts',
      {filePath: 'my/path.ts', repoId: 'my-repo-id'}
    ))
    assert.isFalse(editorFileLocationEqual(
      {filePath: 'my/path.ts', repoId: 'my-repo-id'},
      {filePath: 'my/path.ts'}
    ))
  })
})

describe('resolveEditorFileLocation', () => {
  it('Returns path directly for unscoped location', () => {
    assert.strictEqual(resolveEditorFileLocation('my/path.ts'), 'my/path.ts')
  })
  it('Returns empty path for undefined path', () => {
    assert.strictEqual(resolveEditorFileLocation(undefined), '')
  })
})

describe('deriveEditorRouteParams', () => {
  it('Returns path directly for unscoped location', () => {
    assert.strictEqual(deriveEditorRouteParams('my/path.ts'), 'my/path.ts')
  })
})

describe('extractScopedLocation', () => {
  it('Returns a scoped location out of an unscoped location', () => {
    assert.deepEqual(extractScopedLocation('my/path.ts'), {filePath: 'my/path.ts', repoId: null})
  })
  it('Returns scoped locations unchanged', () => {
    assert.deepEqual(
      extractScopedLocation({filePath: 'my/path.ts', repoId: 'my-repo-id'}),
      {filePath: 'my/path.ts', repoId: 'my-repo-id'}
    )
    assert.deepEqual(
      extractScopedLocation({filePath: 'my/path.ts', repoId: null}),
      {filePath: 'my/path.ts', repoId: null}
    )
  })
})

describe('stripPrimaryRepoId', () => {
  it('Turns an unscoped location into scoped', () => {
    assert.deepEqual(
      stripPrimaryRepoId('my/path.ts', 'primary-repo'),
      {filePath: 'my/path.ts', repoId: null}
    )
  })
  it('Returns scoped locations unchanged', () => {
    assert.deepEqual(
      stripPrimaryRepoId({filePath: 'my/path.ts', repoId: 'my-repo-id'}, 'primary-repo'),
      {filePath: 'my/path.ts', repoId: 'my-repo-id'}
    )
    assert.deepEqual(
      stripPrimaryRepoId({filePath: 'my/path.ts', repoId: null}, 'primary-repo'),
      {filePath: 'my/path.ts', repoId: null}
    )
  })
  it('Removes primary repo from any scoped location', () => {
    assert.deepEqual(
      stripPrimaryRepoId({filePath: 'my/path.ts', repoId: 'primary-repo'}, 'primary-repo'),
      {filePath: 'my/path.ts', repoId: null}
    )
  })
})

describe('deriveLocationKey', () => {
  it('Returns a id from filePath and repoId', () => {
    assert.strictEqual(deriveLocationKey({filePath: 'my/path.ts', repoId: 'my-repo-id'}),
      '.repos/my-repo-id/my/path.ts')
  })
  it('Returns a id from filePath', () => {
    assert.strictEqual(deriveLocationKey({filePath: 'my/path.ts', repoId: null}), 'my/path.ts')
  })
  it('Returns a id from filePath string', () => {
    assert.strictEqual(deriveLocationKey('my/path.ts'), 'my/path.ts')
  })
  it('Returns empty id from null', () => {
    assert.strictEqual(deriveLocationKey(null), '')
  })
})

describe('deriveLocationFromKey', () => {
  it('Returns empty filePath if string is null/undefined', () => {
    assert.strictEqual(deriveLocationFromKey(null), '')
    assert.strictEqual(deriveLocationFromKey(undefined), '')
  })
  it('Returns a scoped location from module file key', () => {
    assert.deepEqual(deriveLocationFromKey('.repos/test.module.1a-2b-3c/path.ts'),
      {filePath: 'path.ts', repoId: 'test.module.1a-2b-3c'})
  })
  it('Returns a scoped location from module file with folders key', () => {
    assert.deepEqual(deriveLocationFromKey('.repos/test.module.1a-2b-3c/folder1/folder2/path.ts'),
      {filePath: 'folder1/folder2/path.ts', repoId: 'test.module.1a-2b-3c'})
  })
  it('Returns a basic location from project file key', () => {
    assert.strictEqual(deriveLocationFromKey('path.ts'), 'path.ts')
  })
  it('Returns a basic location from project file with folders key', () => {
    assert.strictEqual(deriveLocationFromKey('folder1/folder2/path.ts'), 'folder1/folder2/path.ts')
  })
})
