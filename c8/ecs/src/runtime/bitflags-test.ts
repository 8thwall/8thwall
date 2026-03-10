import {describe, it, beforeEach, assert} from '@repo/bzl/js/chai-js'

import {setBitFlag, enableBitFlag, disableBitFlag, isBitFlagEnabled} from './bitflags'

describe('BitFlags', () => {
  let bitFlags: number

  beforeEach(() => {
    bitFlags = 0
  })

  it('should be able to enable and disabled 32 flags and check if they are set', () => {
    for (let i = 0; i < 32; i++) {
      bitFlags = enableBitFlag(bitFlags, i)
      assert.isTrue(isBitFlagEnabled(bitFlags, i), `Flag ${i} should be enabled`)

      bitFlags = disableBitFlag(bitFlags, i)
      assert.isFalse(isBitFlagEnabled(bitFlags, i), `Flag ${i} should be disabled`)
    }
  })

  it('should correctly set a specific flag', () => {
    bitFlags = setBitFlag(5)
    assert.isTrue(isBitFlagEnabled(bitFlags, 5), 'Flag 5 should be enabled after set')
    for (let i = 0; i < 32; i++) {
      if (i !== 5) {
        assert.isFalse(isBitFlagEnabled(bitFlags, i),
          `Flag ${i} should not be enabled after setting flag 5`)
      }
    }
  })
})
