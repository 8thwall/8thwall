import {setBuildIfMock, resetAllFlags} from './buildif-mock'
import {expectNoRegistrations} from '../src/shared/registry'

process.env.NODE_ENV = 'test'
process.env.CDN_PRIVATE_KEY = 'ABCDEF\n012345'

setBuildIfMock()

const expectOnlyDbRegistered = () => {
  try {
    expectNoRegistrations()
  } catch (e) {
    throw new Error(`Expected no registered dependencies before/after running test suite.
Ensure all dependencies are unregistered at the end of each test.
${e}`)
  }
}

const mochaHooks = {
  beforeAll(done) {
    expectOnlyDbRegistered()
    done()
  },
  beforeEach(done) {
    resetAllFlags()
    done()
  },
  afterAll(done) {
    expectOnlyDbRegistered()
    done()
  },
}

export {
  mochaHooks,
}
