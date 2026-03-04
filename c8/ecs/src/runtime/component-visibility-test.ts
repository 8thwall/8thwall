// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, assert} from '@nia/bzl/js/chai-js'

import './test-env'
import {ecs} from './test-runtime-lib'

describe('Component visibility', () => {
  const registeredComponents = ecs.listAttributes().map(e => [e, ecs.getAttribute(e)] as const)
  const exposedComponents = Object.values(ecs)
    .filter(e => typeof e === 'object')
    .filter(e => 'orderedSchema' in e)

  registeredComponents.forEach(([name, component]) => {
    it(`Component ${name} is exposed as an ecs property`, () => {
      assert.include(exposedComponents, component)
    })
  })
})
