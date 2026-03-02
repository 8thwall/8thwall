/* globals describe it */

import {chai, chaiAsPromised} from 'bzl/js/chai-js'

chai.use(chaiAsPromised)
chai.should()

const answerToLife = () => 42

describe('answerToLife', () => {
  it('should return 42', () => answerToLife()
    .should.equal(42))
})
