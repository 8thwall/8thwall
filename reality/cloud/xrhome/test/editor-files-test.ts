import {should, assert} from 'chai'

import {getDraggedFilePath, getRenamedFilePath} from '../src/client/common/editor-files'

import {
  isFolderPath, isAssetPath,
} from '../src/client/common/editor-files'

should()

describe('isFolderPath', () => {
  it('Should be true for folders', () => {
    assert.isTrue(isFolderPath('folder'))
    assert.isTrue(isFolderPath('path/to/folder'))
    assert.isTrue(isFolderPath('/absolute-path/to/folder'))
  })

  it('Should be false for non-folders', () => {
    assert.isFalse(isFolderPath(''))
    assert.isFalse(isFolderPath('LICENSE'))
    assert.isFalse(isFolderPath('file.txt'))
    assert.isFalse(isFolderPath('path/to/file.txt'))
    assert.isFalse(isFolderPath('/absolute-path/to/file.txt'))
  })
})

describe('isAssetPath/isTextPath', () => {
  const mains = ['head.html', 'body.html', 'app.js']
  const assets = ['assets/folder', 'assets', 'assets/file.glb']
  const texts = ['LICENSE', 'file.js', 'frag.txt', 'folder/file.css', 'random/folder']

  it('Main files should only be main paths', () => {
    mains.forEach((filename) => {
      assert.isFalse(isAssetPath(filename))
    })
  })

  it('Asset files should only be assets paths', () => {
    assets.forEach((filename) => {
      assert.isTrue(isAssetPath(filename))
    })
  })

  it('Text files should only be text paths', () => {
    texts.forEach((filename) => {
      assert.isFalse(isAssetPath(filename))
    })
  })
})

describe('getDraggedFilePath', () => {
  it('Can move a file into a folder', () => {
    assert.strictEqual(getDraggedFilePath('example.txt', 'folder'), 'folder/example.txt')
  })
  it('Can move a file between folders', () => {
    assert.strictEqual(getDraggedFilePath('in-folder/example.txt', 'folder'), 'folder/example.txt')
  })
  it('Can move a file out a folder', () => {
    assert.strictEqual(getDraggedFilePath('in-folder/example.txt', ''), 'example.txt')
  })
  it('Keeps asset files within the assets folder when dragging to the top level', () => {
    assert.strictEqual(getDraggedFilePath('assets/folder/example.jpg', ''), 'assets/example.jpg')
  })
  it('Can move to the top level', () => {
    assert.strictEqual(getDraggedFilePath('in-folder/example.txt'), 'example.txt')
  })
  it('Can move folders', () => {
    assert.strictEqual(getDraggedFilePath('in-folder/test', 'other/folder'), 'other/folder/test')
  })
  it('Interprets false folderPath as top level', () => {
    assert.strictEqual(getDraggedFilePath('in-folder/example.txt', null), 'example.txt')
  })
})

describe('getRenamedFilePath', () => {
  it('Can rename a file', () => {
    assert.strictEqual(getRenamedFilePath('after.txt', 'before.txt'), 'after.txt')
  })
  it('Should preserve the enclosing folder', () => {
    assert.strictEqual(getRenamedFilePath('after.txt', 'folder/before.txt'), 'folder/after.txt')
  })
  // NOTE(christoph): This is kind of a secret "pro" behavior but it's the current state.
  it('Handle a subfolder in the new name', () => {
    assert.strictEqual(
      getRenamedFilePath('extra/after.txt', 'folder/before.txt'),
      'folder/extra/after.txt'
    )
  })
})
