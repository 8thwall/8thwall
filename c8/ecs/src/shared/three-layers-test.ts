// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {assert, it, describe} from '@nia/bzl/js/chai-js'

import {THREE_LAYERS} from './three-layers'

describe('THREE_LAYERS', () => {
  it('should have no repeat layers', () => {
    assert.isTrue(Object.values(THREE_LAYERS).length === new Set(Object.values(THREE_LAYERS)).size)
  })

  it('should only have values from 0 to 31', () => {
    assert.isTrue(Object.values(THREE_LAYERS).every(layer => layer >= 0 && layer <= 31))
  })
})
