/* globals describe it */

/* eslint-disable import/no-unresolved */
import {chai, chaiAsPromised} from 'bzl/js/chai-js'
import addon from 'bzl/examples/example-node-addon'
/* eslint-enable import/no-unresolved */

chai.use(chaiAsPromised)
chai.should()

describe('testNativeIntMethod', () => {
  it('should return 42', () => addon.exampleIntMethod()
    .should.equal(42))
})

describe('testNativeStringMethod', () => {
  it('should return \'This is a native msg\'', () => addon.exampleStringMethod()
    .should.equal('This is a native msg'))
})
