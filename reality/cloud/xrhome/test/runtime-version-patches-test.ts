// @attr(npm_rule = "@npm-lambda//:npm-lambda")
import {assert} from 'chai'
import {describe, it} from 'mocha'

import type {RuntimeVersionInfo, RuntimeVersionTarget} from '@ecs/shared/runtime-version'

import {
  getVersionSpecifier,
  getNewestVersionInfo,
  createNewVersionTarget,
  getVersionSpecifierAtLevel,
} from '../src/client/studio/runtime-version/runtime-version-patches'

const dummyTarget: RuntimeVersionTarget = {
  type: 'version',
  level: 'patch',
  major: 4,
  minor: 2,
  patch: 13,
}

describe('getNewestVersionInfo', () => {
  it('Returns null with undefined version list', () => {
    assert.deepEqual(getNewestVersionInfo(null), null)
  })
  it('Returns null with undefined version list', () => {
    assert.deepEqual(getNewestVersionInfo(undefined), null)
  })
  it('Returns null with empty version list', () => {
    assert.deepEqual(getNewestVersionInfo([]), null)
  })
  it('Returns expected version with one version in list', () => {
    const versionInfo: RuntimeVersionInfo = {
      publishTime: 1633036800000,
      patchTarget: dummyTarget,
    }
    assert.deepEqual(getNewestVersionInfo([versionInfo]), versionInfo)
  })
  it('Returns expected version with multiple versions in list', () => {
    const expectedTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 5,
      minor: 0,
      patch: 0,
    }
    const expectedVersionInfo: RuntimeVersionInfo = {
      publishTime: 1633036800000,
      patchTarget: expectedTarget,
    }
    const versionList: RuntimeVersionInfo[] = [
      expectedVersionInfo,
      {
        publishTime: 1633036800000,
        patchTarget: {
          type: 'version',
          level: 'patch',
          major: 0,
          minor: 0,
          patch: 100,
        },
      },
      {
        publishTime: 1633036800000,
        patchTarget: {
          type: 'version',
          level: 'patch',
          major: 0,
          minor: 100,
          patch: 0,
        },
      },
      {
        publishTime: 1633036800000,
        patchTarget: {
          type: 'version',
          level: 'patch',
          major: 4,
          minor: 99,
          patch: 999,
        },
      },
    ]
    assert.deepEqual(getNewestVersionInfo(versionList), expectedVersionInfo)
  })
})

describe('getVersionSpecifier', () => {
  it('Returns expected string with major level', () => {
    assert.equal(getVersionSpecifier({...dummyTarget, level: 'major'}), '4.2.13')
  })
  it('Returns expected string with minor level', () => {
    assert.equal(getVersionSpecifier({...dummyTarget, level: 'minor'}), '4.2.13')
  })
  it('Returns expected string with patch level', () => {
    assert.equal(getVersionSpecifier(dummyTarget), '4.2.13')
  })
  it('Returns expected string with only major number', () => {
    const target: RuntimeVersionTarget = {
      type: 'version',
      level: 'major',
      major: 0,
    }
    assert.equal(getVersionSpecifier(target), '0.x.x')
  })
  it('Returns expected string with only minor numbers', () => {
    const target: RuntimeVersionTarget = {
      type: 'version',
      level: 'minor',
      major: 5,
      minor: 0,
    }
    assert.equal(getVersionSpecifier(target), '5.0.x')
  })
})

describe('createNewVersionTarget', () => {
  it('Returns expected target with major level', () => {
    const expectedTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 5,
      minor: 0,
      patch: 0,
    }
    assert.deepEqual(createNewVersionTarget(dummyTarget, 'major'), expectedTarget)
  })
  it('Returns expected target with minor level', () => {
    const expectedTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 4,
      minor: 3,
      patch: 0,
    }
    assert.deepEqual(createNewVersionTarget(dummyTarget, 'minor'), expectedTarget)
  })
  it('Returns expected target with patch level', () => {
    const expectedTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 4,
      minor: 2,
      patch: 14,
    }
    assert.deepEqual(createNewVersionTarget(dummyTarget, 'patch'), expectedTarget)
  })
  it('Returns expected target with bad level', () => {
    const expectedTarget: RuntimeVersionTarget = {
      type: 'version',
      level: 'patch',
      major: 4,
      minor: 2,
      patch: 13,
    }
    assert.deepEqual(createNewVersionTarget(dummyTarget, 'bad'), expectedTarget)
  })
})

describe('getVersionSpecifierAtLevel', () => {
  it('Returns normal specifier for patch', () => {
    assert.equal(getVersionSpecifierAtLevel(dummyTarget, 'patch'), '4.2.13')
  })
  it('Returns truncated specifier for minor', () => {
    assert.equal(getVersionSpecifierAtLevel(dummyTarget, 'minor'), '4.2.x')
  })
  it('Returns more truncated specifier for major', () => {
    assert.equal(getVersionSpecifierAtLevel(dummyTarget, 'major'), '4.x.x')
  })
  it('Requires that target is a patch', () => {
    const target: RuntimeVersionTarget = {
      type: 'version',
      level: 'major',
      major: 4,
    }
    assert.throws(() => {
      getVersionSpecifierAtLevel(target, 'patch')
    }, 'getVersionSpecifierAtLevel expected patch target')
  })
})
