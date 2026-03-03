// @attr(npm_rule = "@npm-jsxr//:npm-jsxr")
import {chai} from 'bzl/js/chai-js'

import {normalizeColors} from './pixel-transforms'

const {describe, it} = globalThis as any
const {expect} = chai

describe('Pixel Transforms Test', () => {
  it('normalizeColors()', () => {
    const colors = new Float32Array([0, 1, 10, 255, 1000])
    normalizeColors(colors)
    expect(colors[0]).to.be.closeTo(0 / 255, 1e-6)
    expect(colors[1]).to.be.closeTo(1 / 255, 1e-6)
    expect(colors[2]).to.be.closeTo(10 / 255, 1e-6)
    expect(colors[3]).to.be.closeTo(255 / 255, 1e-6)
    expect(colors[4]).to.be.closeTo(1000 / 255, 1e-6)
  })
})
