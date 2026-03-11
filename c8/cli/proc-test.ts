// eslint-disable-next-line camelcase
import * as child_process from 'child_process'

import {
  describe, it, assert, afterEach, beforeEach, sinon,
} from '@repo/bzl/js/chai-js'

import {streamExec, streamSpawn} from './proc'

describe('streamExec', () => {
  let fakeProc: any

  beforeEach(() => {
    fakeProc = {
      stdout: {on: sinon.stub()},
      stderr: {on: sinon.stub()},
      on: sinon.stub(),
      kill: sinon.stub(),
    }
    sinon.stub(child_process, 'spawn').returns(fakeProc)
  })

  afterEach(() => {
    sinon.restore()
  })

  it('resolves on exit code 0', async () => {
    fakeProc.on.withArgs('exit').yields(0)
    await streamExec('echo test')

    assert.ok(true)
  })

  it('rejects on non-zero exit code', async () => {
    fakeProc.on.withArgs('exit').yields(1)
    try {
      await streamExec('fail test')
      assert.fail('Should have thrown')
    } catch (e) {
      assert.ok(true)
    }
  })

  it('logs stdout and stderr', async () => {
    const logSpy = sinon.spy(console, 'log')
    const errorSpy = sinon.spy(console, 'error')
    fakeProc.stdout.on.withArgs('data').yields(Buffer.from('hello'))
    fakeProc.stderr.on.withArgs('data').yields(Buffer.from('err'))
    fakeProc.on.withArgs('exit').yields(0)
    await streamExec('echo test')
    assert(logSpy.calledWith('', 'hello'))
    assert(errorSpy.calledWith('', 'err'))
    logSpy.restore()
    errorSpy.restore()
  })

  it('logs with logPrefix', async () => {
    const logSpy = sinon.spy(console, 'log')
    const errorSpy = sinon.spy(console, 'error')
    fakeProc.stdout.on.withArgs('data').yields(Buffer.from('hello'))
    fakeProc.stderr.on.withArgs('data').yields(Buffer.from('err'))
    fakeProc.on.withArgs('exit').yields(0)
    await streamExec('echo test', '[PREFIX]')
    assert(logSpy.calledWith('[PREFIX]', 'hello'))
    assert(errorSpy.calledWith('[PREFIX]', 'err'))
    logSpy.restore()
    errorSpy.restore()
  })

  it('passes cwd option to spawn', async () => {
    const spawnSpy = child_process.spawn as sinon.SinonStub
    fakeProc.on.withArgs('exit').yields(0)
    await streamExec('echo test', '', {cwd: '/tmp'})
    assert(spawnSpy.calledWith('echo', ['test'], sinon.match.has('cwd', '/tmp')))
  })

  it('handles stdio: inherit and SIGINT', async () => {
    const spawnSpy = child_process.spawn as sinon.SinonStub

    fakeProc.on.withArgs('exit').yields(0)
    const processOnSpy = sinon.spy(process, 'on')
    const processOffSpy = sinon.spy(process, 'off')
    await streamExec('echo test', '', {stdio: 'inherit'})
    assert(spawnSpy.calledWith('echo', ['test'], sinon.match.has('stdio', 'inherit')))
    assert(processOnSpy.calledWith('SIGINT'))
    assert(processOffSpy.calledWith('SIGINT'))
    processOnSpy.restore()
    processOffSpy.restore()
  })
})

describe('streamSpawn', () => {
  let fakeProc: any

  beforeEach(() => {
    fakeProc = {
      stdout: {on: sinon.stub()},
      stderr: {on: sinon.stub()},
      on: sinon.stub(),
      kill: sinon.stub(),
    }
    sinon.stub(child_process, 'spawn').returns(fakeProc)
  })

  afterEach(() => {
    sinon.restore()
  })

  it('resolves on exit code 0 (no capture)', async () => {
    fakeProc.on.withArgs('exit').yields(0)
    await streamSpawn('echo', ['test'], false)
    assert.ok(true)
  })

  it('resolves and returns output on exit code 0 (capture)', async () => {
    fakeProc.stdout.on.withArgs('data').yields(Buffer.from('hello'))
    fakeProc.stderr.on.withArgs('data').yields(Buffer.from('err'))
    fakeProc.on.withArgs('exit').yields(0)
    const result = await streamSpawn('echo', ['test'], true)
    assert(result && result.stdout.includes('hello'))
    assert(result && result.stderr.includes('err'))
  })

  it('rejects on non-zero exit code (no capture)', async () => {
    fakeProc.on.withArgs('exit').yields(1)
    try {
      await streamSpawn('fail', ['test'], false)
      assert.fail('Should have thrown')
    } catch (e) {
      assert.ok(true)
    }
  })

  it('rejects with error object on non-zero exit code (capture)', async () => {
    fakeProc.stdout.on.withArgs('data').yields(Buffer.from('hello'))
    fakeProc.stderr.on.withArgs('data').yields(Buffer.from('err'))
    fakeProc.on.withArgs('exit').yields(1)
    try {
      await streamSpawn('fail', ['test'], true)
      assert.fail('Should have thrown')
    } catch (e: any) {
      assert(e.error instanceof Error)
      assert(e.stdout.includes('hello'))
      assert(e.stderr.includes('err'))
    }
  })

  it('logs stdout and stderr (no capture)', async () => {
    const logSpy = sinon.spy(console, 'log')
    const errorSpy = sinon.spy(console, 'error')
    fakeProc.stdout.on.withArgs('data').yields(Buffer.from('hello'))
    fakeProc.stderr.on.withArgs('data').yields(Buffer.from('err'))
    fakeProc.on.withArgs('exit').yields(0)
    await streamSpawn('echo', ['test'], false)
    assert(logSpy.calledWith('hello'))
    assert(errorSpy.calledWith('err'))
    logSpy.restore()
    errorSpy.restore()
  })

  it('passes cwd option to spawn', async () => {
    const spawnSpy = child_process.spawn as sinon.SinonStub
    fakeProc.on.withArgs('exit').yields(0)
    await streamSpawn('echo', ['test'], false, {cwd: '/tmp'})
    assert(spawnSpy.calledWith('echo', ['test'], sinon.match.has('cwd', '/tmp')))
  })

  it('handles stdio: inherit and SIGINT', async () => {
    const spawnSpy = child_process.spawn as sinon.SinonStub
    fakeProc.on.withArgs('exit').yields(0)
    const processOnSpy = sinon.spy(process, 'on')
    await streamSpawn('echo', ['test'], false, {stdio: 'inherit'})
    assert(spawnSpy.calledWith('echo', ['test'], sinon.match.has('stdio', 'inherit')))
    assert(processOnSpy.calledWith('SIGINT'))
    processOnSpy.restore()
  })
})
