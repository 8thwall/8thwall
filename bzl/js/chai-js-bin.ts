// @rule(js_binary)
// @attr(testonly = 1)
// @attr(export_library = 1)
// @attr(commonjs = 1)
// @attr(target = "node")
// @package(npm-mocha)
// @visibility(//visibility:public)

import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
import chaiExclude from 'chai-exclude'
import chaiBytes from 'chai-bytes'
import chaiDatetime from 'chai-datetime'
import type {MochaGlobals} from 'mocha'
import sinon, {type SinonSpy} from 'sinon'

const {describe, it, beforeEach, before, afterEach, after}: MochaGlobals = globalThis

const {assert} = chai

export {
  chai, chaiAsPromised, chaiBytes, chaiDatetime,
  chaiExclude,
  before,
  beforeEach,
  assert, describe, it, afterEach, after,
  sinon,
}

export type {
  SinonSpy,
}
