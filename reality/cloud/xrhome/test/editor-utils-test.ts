import chai from 'chai'

import {updateToShortFilePaths} from '../src/client/editor/editor-utils'

chai.should()
const {expect} = chai

describe('Update to short file paths', () => {
  it('Use file name only for files with unique file name', () => {
    const files = [
      {filePath: 'a/b/d/doc.js'},
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/c/d/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('doc.js')
    expect(files[1].shortFilePath).to.not.equal('config.ts')
    expect(files[2].shortFilePath).to.not.equal('config.ts')
  })

  it('Use the first unique sub path to show the file paths', () => {
    const files = [
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/c/d/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('.../b/.../config.ts')
    expect(files[1].shortFilePath).to.equal('.../c/.../config.ts')
  })

  it('Use the first unique sub path to show the file paths for different file path depths', () => {
    const files = [
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/aa/c/d/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('.../b/.../config.ts')
    expect(files[1].shortFilePath).to.equal('.../c/.../config.ts')
  })

  it('Use the first unique sub path - first dir from the end to show the file paths', () => {
    const files = [
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/c/d/config.ts'},
      {filePath: 'a/c/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('.../b/.../config.ts')
    expect(files[1].shortFilePath).to.equal('.../c/.../config.ts')
    expect(files[2].shortFilePath).to.equal('.../c/config.ts')
  })

  it('Use the first unique sub path - root dir to show the file paths', () => {
    const files = [
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/c/d/config.ts'},
      {filePath: 'a/d/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('.../b/.../config.ts')
    expect(files[1].shortFilePath).to.equal('.../c/.../config.ts')
    expect(files[2].shortFilePath).to.equal('a/.../config.ts')
  })

  it('Do not create short file path because of having no unique sub path', () => {
    const files = [
      {filePath: 'a/b/d/config.ts'},
      {filePath: 'a/c/e/config.ts'},
      {filePath: 'a/c/d/config.ts'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('.../b/.../config.ts')
    expect(files[1].shortFilePath).to.equal('.../e/config.ts')
    expect(files[2].shortFilePath).to.not.exist
  })

  it('Uses preset shortFilePaths', () => {
    const files = [
      {filePath: 'a/c/d/my-config.ts', shortFilePath: 'my-config'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('my-config')
  })

  it('Uses preset shortFilePaths among same base names', () => {
    const files = [
      {filePath: 'a/my-config.ts', shortFilePath: 'my-config'},
      {filePath: 'b/my-config.ts', shortFilePath: 'my-other-config'},
    ]
    updateToShortFilePaths(files)
    expect(files[0].shortFilePath).to.equal('my-config')
    expect(files[1].shortFilePath).to.equal('my-other-config')
  })
})
