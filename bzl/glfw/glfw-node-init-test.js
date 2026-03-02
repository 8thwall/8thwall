/* globals describe it */

/* eslint-disable import/no-unresolved */
import {chai, chaiAsPromised} from 'bzl/js/chai-js'
import {glfw} from 'third_party/glfw-raub/glfw-raub'
/* eslint-enable import/no-unresolved */

chai.use(chaiAsPromised)
chai.should()

describe('testInitGlfw', () => {
  it('successfully initialize GLFW', () => glfw.init()
    .should.be.true)

  glfw.terminate()
})
