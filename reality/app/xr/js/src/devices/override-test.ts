// @inliner-off
import {chai} from 'bzl/js/chai-js'

import {pickFromRenderer} from './override'

const {describe, it} = globalThis as any
const {assert} = chai

describe('Override Test', () => {
  it('pickFromRenderer() fallback when no renderer match', () => {
    const pick = pickFromRenderer(
      'Apple GPU',
      'Apple A10 GPU | Apple A11 GPU|Apple A13 GPU|Apple A15GPU',
      [
        ['A12', '10,1'],
        ['A14', '10,2'],
      ],
      'TheFallback'
    )
    assert.equal('TheFallback', pick)
  })

  it('pickFromRenderer() fallback when no renderer is available', () => {
    const pick = pickFromRenderer(
      'Apple GPU',
      '',
      [
        ['A12', '10,1'],
        ['A14', '10,2'],
      ],
      'TheFallback'
    )
    assert.equal('TheFallback', pick)
  })

  it('pickFromRenderer() fallback when no priority list', () => {
    const pick = pickFromRenderer(
      'Apple A8 GPU',
      'Apple A10 GPU|Apple A9 GPU',
      [],
      'TheFallback'
    )
    assert.equal('TheFallback', pick)
  })

  it('pickFromRenderer() match against unmasked webgl renderer', () => {
    const pick = pickFromRenderer(
      'Apple A8 GPU',
      'Apple A10 GPU|Apple A9 GPU',
      [
        ['A8', 'FOUND'],
        ['A7', 'TEST'],
      ],
      'TheFallback'
    )
    assert.equal('iPhoneFOUND', pick)
  })

  it('pickFromRenderer() match follow priority', () => {
    const pick = pickFromRenderer(
      'Apple A8 GPU',
      'Apple A10 GPU|Apple A9 GPU',
      [
        ['A11', 'NOT_THIS'],
        ['A10', 'FOUND'],
        ['A8', 'ALSO_MATCH'],
      ],
      'TheFallback'
    )
    assert.equal('iPhoneFOUND', pick)
  })
})
