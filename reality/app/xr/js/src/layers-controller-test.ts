// @inliner-off
import {chai} from 'bzl/js/chai-js'

import {LayersControllerFactory} from './layers-controller'

const {describe, it} = globalThis as any
const {expect} = chai

describe('LayersController Test', () => {
  it('Can get a LayersController', () => {
    const layersController = LayersControllerFactory(null, null)
    expect(layersController).to.not.be.null
  })
})
