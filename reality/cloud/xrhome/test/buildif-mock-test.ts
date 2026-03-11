import {should, assert} from 'chai'

import {enableFlag, resetFlag, resetAllFlags, disableFlag, failSoft} from './buildif-mock'

should()

describe('BuildIf Mock Test', () => {
  it('ALL_QA is false by default', () => {
    assert.equal(false, BuildIf.ALL_QA)
  })

  it('ALL_QA is true after enabling the flag', () => {
    enableFlag('ALL_QA')
    assert.equal(true, BuildIf.ALL_QA)
  })

  it('ALL_QA is reset to false before running this test', () => {
    assert.equal(false, BuildIf.ALL_QA)
  })

  it('resetFlag resets a single flag', () => {
    enableFlag('ALL_QA')
    enableFlag('LOCAL_DEV')
    assert.equal(true, BuildIf.ALL_QA)
    assert.equal(true, BuildIf.LOCAL_DEV)

    resetFlag('ALL_QA')
    assert.equal(false, BuildIf.ALL_QA)
    assert.equal(true, BuildIf.LOCAL_DEV)
  })

  it('resetAllFlags resets all self enabled flags', () => {
    enableFlag('ALL_QA')
    enableFlag('LOCAL_DEV')
    assert.equal(true, BuildIf.ALL_QA)
    assert.equal(true, BuildIf.LOCAL_DEV)

    resetAllFlags()
    assert.equal(false, BuildIf.ALL_QA)
    assert.equal(false, BuildIf.LOCAL_DEV)
  })

  it('disableFlag always resolves to false', () => {
    // NOTE(kyle): This test will fail if we delete FULL_ROLLOUT
    disableFlag('FULL_ROLLOUT')
    assert.equal(false, BuildIf.FULL_ROLLOUT)

    enableFlag('ALL_QA')
    assert.equal(true, BuildIf.ALL_QA)

    disableFlag('ALL_QA')
    assert.equal(false, BuildIf.ALL_QA)
  })

  it('Modifying BuildIf directly throws an error', () => {
    failSoft()
    assert.throws(() => { BuildIf.ALL_QA = true })
  })

  it('Reassigning globalThis.BuildIf throws an error', () => {
    failSoft()
    assert.throws(() => { globalThis.BuildIf = {new: 'object'} })
  })

  it('Accessing an unset flag throws an error', () => {
    failSoft()
    // eslint-disable-next-line @typescript-eslint/no-unused-expressions
    assert.throws(() => { BuildIf.Example })
  })
})
