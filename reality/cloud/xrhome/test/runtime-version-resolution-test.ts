// @attr(npm_rule = "@npm-lambda//:npm-lambda")
import {assert} from 'chai'
import {describe, it} from 'mocha'

import type {RuntimeVersionInfo, RuntimeVersionTarget} from '@ecs/shared/runtime-version'

import {
  resolveSelectedVersion,
  filterResolvableVersions,
} from '../src/client/studio/runtime-version/version-resolution'

const makePatch = (
  major: number, minor: number, patch: number
): RuntimeVersionTarget => ({
  type: 'version', level: 'patch', major, minor, patch,
})

const makeMinor = (major: number, minor: number): RuntimeVersionTarget => ({
  type: 'version', level: 'minor', major, minor,
})

const makeMajor = (major: number): RuntimeVersionTarget => ({
  type: 'version', level: 'major', major,
})

const makeVersionInfo = (target: RuntimeVersionTarget): RuntimeVersionInfo => ({
  patchTarget: target,
  publishTime: 1,
})

const VERSIONS = [
  makeVersionInfo(makePatch(3, 0, 1)),
  makeVersionInfo(makePatch(3, 0, 0)),
  makeVersionInfo(makePatch(2, 2, 1)),
  makeVersionInfo(makePatch(2, 2, 0)),
  makeVersionInfo(makePatch(2, 1, 0)),
  makeVersionInfo(makePatch(2, 0, 1)),
  makeVersionInfo(makePatch(2, 0, 0)),
  makeVersionInfo(makePatch(1, 0, 1)),
  makeVersionInfo(makePatch(1, 0, 0)),
]

describe('resolveSelectedVersion', () => {
  it('Returns an exact version for patch', () => {
    const resolved = resolveSelectedVersion(makePatch(2, 0, 1), VERSIONS)
    assert.deepEqual(resolved?.patchTarget, makePatch(2, 0, 1))
  })

  it('Returns the latest on a minor', () => {
    const resolved = resolveSelectedVersion(makeMinor(2, 0), VERSIONS)
    assert.deepEqual(resolved?.patchTarget, makePatch(2, 0, 1))
  })

  it('Returns the latest on a major', () => {
    const resolved = resolveSelectedVersion(makeMajor(2), VERSIONS)
    assert.deepEqual(resolved?.patchTarget, makePatch(2, 2, 1))
  })

  it('Returns null for non-existent version', () => {
    const resolved = resolveSelectedVersion(makePatch(5, 0, 0), VERSIONS)
    assert.equal(resolved, null)
  })

  it('Returns null for empty version list', () => {
    const resolved = resolveSelectedVersion(makePatch(1, 0, 0), [])
    assert.equal(resolved, null)
  })

  it('Versions are optional', () => {
    const resolved = resolveSelectedVersion(makePatch(2, 0, 1), undefined)
    assert.equal(resolved, null)
  })
})

describe('filterResolvableVersions', () => {
  it('Returns exact array if patch', () => {
    assert.strictEqual(filterResolvableVersions('patch', VERSIONS), VERSIONS)
  })

  it('Leaves out old patches for minor', () => {
    assert.deepEqual(filterResolvableVersions('minor', VERSIONS), [
      makeVersionInfo(makePatch(3, 0, 1)),
      makeVersionInfo(makePatch(2, 2, 1)),
      makeVersionInfo(makePatch(2, 1, 0)),
      makeVersionInfo(makePatch(2, 0, 1)),
      makeVersionInfo(makePatch(1, 0, 1)),
    ])
  })

  it('Only includes latest on major for major', () => {
    assert.deepEqual(filterResolvableVersions('major', VERSIONS), [
      makeVersionInfo(makePatch(3, 0, 1)),
      makeVersionInfo(makePatch(2, 2, 1)),
      makeVersionInfo(makePatch(1, 0, 1)),
    ])
  })

  it('Versions are optional', () => {
    assert.deepEqual(filterResolvableVersions('major', undefined), null)
  })
})
