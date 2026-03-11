// @attr(npm_rule = "@npm-lambda//:npm-lambda")
import {assert} from 'chai'
import {describe, it} from 'mocha'

import type {RuntimeVersionTarget} from '@ecs/shared/runtime-version'

import {
  isSameBaseVersionAtLevel,
  isGreaterPatchVersion,
} from '../src/client/studio/runtime-version/compare-runtime-target'

describe('isSameBaseVersionAtLevel', () => {
  const target1: RuntimeVersionTarget = {
    type: 'version',
    level: 'patch',
    major: 1,
    minor: 2,
    patch: 3,
  }

  const target2: RuntimeVersionTarget = {
    type: 'version',
    level: 'patch',
    major: 1,
    minor: 2,
    patch: 4,
  }

  const target3: RuntimeVersionTarget = {
    type: 'version',
    level: 'patch',
    major: 2,
    minor: 0,
    patch: 0,
  }

  it('Returns true for same major version at major level', () => {
    assert.equal(isSameBaseVersionAtLevel(target1, target2, 'major'), true)
    assert.equal(isSameBaseVersionAtLevel(target1, target3, 'major'), false)
  })

  it('Returns true for same minor version at minor level', () => {
    assert.equal(isSameBaseVersionAtLevel(target1, target2, 'minor'), true)
    assert.equal(isSameBaseVersionAtLevel(target1, target3, 'minor'), false)
  })

  it('Returns true for same patch version at patch level', () => {
    assert.equal(isSameBaseVersionAtLevel(target1, target1, 'patch'), true)
    assert.equal(isSameBaseVersionAtLevel(target1, target2, 'patch'), false)
  })
})

describe('isGreaterPatchVersion', () => {
  it('Returns true when first version is greater', () => {
    const newer: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 2,
      minor: 0,
      patch: 0,
    }
    const older: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 9,
      patch: 9,
    }
    assert.equal(isGreaterPatchVersion(newer, older), true)
    assert.equal(isGreaterPatchVersion(older, newer), false)
  })

  it('Returns false for equal versions', () => {
    const target: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 2,
      patch: 3,
    }
    assert.equal(isGreaterPatchVersion(target, target), false)
  })

  it('Returns true for greater major version', () => {
    const newer: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 2,
      minor: 0,
      patch: 0,
    }
    const older: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 9,
      patch: 9,
    }
    assert.equal(isGreaterPatchVersion(newer, older), true)
  })

  it('Returns true for greater minor version with same major', () => {
    const newer: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 3,
      patch: 0,
    }
    const older: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 2,
      patch: 9,
    }
    assert.equal(isGreaterPatchVersion(newer, older), true)
  })

  it('Returns true for greater patch version with same major and minor', () => {
    const newer: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 2,
      patch: 4,
    }
    const older: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 2,
      patch: 3,
    }
    assert.equal(isGreaterPatchVersion(newer, older), true)
  })

  it('Throws error for non-patch level targets', () => {
    const majorTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'major',
      major: 1,
    }
    const patchTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 1,
      minor: 2,
      patch: 3,
    }
    assert.throws(() => {
      isGreaterPatchVersion(majorTarget, patchTarget)
    }, 'Encountered unexpected level in isGreaterPatchVersion')
  })
})
