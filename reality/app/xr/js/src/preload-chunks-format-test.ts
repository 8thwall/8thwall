// @package(npm-ecs)
// @attr(externalize_npm = 1)

import {describe, it, assert} from '@repo/bzl/js/chai-js'

import {
  parsePreloadChunks,
} from './preload-chunks-format'

describe('parsePreloadChunks', () => {
  it('returns empty if unset', () => {
    assert.deepEqual(parsePreloadChunks(undefined as any as string), [])
    assert.deepEqual(parsePreloadChunks(null as any as string), [])
    assert.deepEqual(parsePreloadChunks(''), [])
  })

  it('parses chunks by name', () => {
    assert.deepEqual(parsePreloadChunks('slam'), [{name: 'slam', url: null}])
    assert.deepEqual(parsePreloadChunks('slam '), [{name: 'slam', url: null}])
    assert.deepEqual(parsePreloadChunks(' slam '), [{name: 'slam', url: null}])
  })

  it('parses multiple chunks by name', () => {
    assert.deepEqual(parsePreloadChunks('slam, face'),
      [{name: 'slam', url: null}, {name: 'face', url: null}])
    assert.deepEqual(parsePreloadChunks('slam , face'),
      [{name: 'slam', url: null}, {name: 'face', url: null}])
    assert.deepEqual(parsePreloadChunks(' slam , face\n'),
      [{name: 'slam', url: null}, {name: 'face', url: null}])
  })

  it('does not remove invalid names', () => {
    assert.deepEqual(parsePreloadChunks('slam, other'),
      [{name: 'slam', url: null}, {name: 'other', url: null}])
  })

  it('includes urls if provided', () => {
    assert.deepEqual(parsePreloadChunks('slam:/slam.js'),
      [{name: 'slam', url: '/slam.js'}])
    assert.deepEqual(parsePreloadChunks('slam , face:   /face.js '),
      [{name: 'slam', url: null}, {name: 'face', url: '/face.js'}])
    assert.deepEqual(parsePreloadChunks(' slam: https://example.com/slam.js\n'),
      [{name: 'slam', url: 'https://example.com/slam.js'}])
  })
})
