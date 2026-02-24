// @inliner-off
import type * as Chai from 'chai'
import type * as Mocha from 'mocha'
import chaiAsPromised from 'chai-as-promised'
import chaiExclude from 'chai-exclude'
import chaiBytes from 'chai-bytes'
import chaiDatetime from 'chai-datetime'
import sinon, {SinonSpy} from 'sinon'

declare const describe: Mocha.SuiteFunction,
  it: Mocha.TestFunction, beforeEach: Mocha.HookFunction,
  before: Mocha.HookFunction,
  afterEach: Mocha.HookFunction,
  after: Mocha.HookFunction

declare const assert: Chai.AssertStatic

declare const chai: typeof Chai

export {
  chai,
  chaiAsPromised,
  chaiBytes,
  chaiDatetime,
  chaiExclude,
  before,
  beforeEach,
  assert,
  describe,
  it,
  afterEach,
  after,
  sinon,
  SinonSpy,
}
