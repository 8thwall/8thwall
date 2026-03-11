import {describe, it} from 'mocha'
import {assert} from 'chai'
import {exec} from 'child_process'
import path from 'path'

describe('XRHome Unit Tests', () => {
  it('All tests are at the top level of the test folder', async () => {
    const files = await new Promise<string[]>((resolve, reject) => {
      exec('find . -name "*-test.*"', {
        cwd: __dirname,
      }, (error, stdout) => {
        if (error) {
          reject(error)
        } else {
          resolve(stdout.split('\n').map(file => file.trim()).filter(Boolean))
        }
      })
    })

    const nestedFiles = files.filter(e => path.dirname(e) !== '.')
    assert.deepStrictEqual(nestedFiles, [])
  })

  it('All test files are typescript files', async () => {
    const files = await new Promise<string[]>((resolve, reject) => {
      exec('find . -name "*-test.*"', {
        cwd: __dirname,
      }, (error, stdout) => {
        if (error) {
          reject(error)
        } else {
          resolve(stdout.split('\n').map(file => file.trim()).filter(Boolean))
        }
      })
    })
    const nonTsFiles = files.filter(e => !e.endsWith('.ts') && !e.endsWith('.tsx'))
    assert.isEmpty(nonTsFiles)
  })
})
