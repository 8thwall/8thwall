// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, assert} from '@repo/bzl/js/chai-js'

import './test-env'
import {ecs} from './test-runtime-lib'

describe('registerAttribute', () => {
  it('Allows a component to be registered', () => {
    const testComponent = ecs.registerComponent({
      name: 'test-component',
      schema: {value: 'string'},
    })

    assert.isDefined(testComponent)
  })

  it('Does not error if the same component is registered a second time', () => {
    const testComponent = ecs.registerComponent({
      name: 'test-component',
      schema: {value: 'string'},
    })

    assert.isDefined(testComponent)
  })

  it('Does not error if a component is registered that collides with a built-in attribute', () => {
    const MyPosition = ecs.registerComponent({
      name: 'position',
      schema: {value: 'string'},
    })

    assert.isDefined(MyPosition)

    assert.notStrictEqual<any>(ecs.Position, MyPosition)
  })
})

describe('getAttribute', () => {
  it('Returns built-in attributes', () => {
    const scale = ecs.getAttribute('scale')
    assert.isDefined(scale)
    assert.strictEqual<any>(scale, ecs.Scale)
  })

  it('Returns custom attributes', () => {
    const registered = ecs.registerComponent({
      name: 'test-component2',
      schema: {value: 'string'},
    })

    const returned = ecs.getAttribute('test-component2')
    assert.isDefined(registered)
    assert.strictEqual<any>(registered, returned)
  })

  it('Returns the most recently registered attribute', () => {
    const first = ecs.registerComponent({
      name: 'test-component3',
    })

    const second = ecs.registerComponent({
      name: 'test-component3',
    })

    const returned = ecs.getAttribute('test-component3')
    assert.isDefined(returned)
    assert.notEqual<any>(first, returned)
    assert.strictEqual<any>(returned, second)
  })
})

describe('listAttributes', () => {
  it('Returns all registered attributes', () => {
    const attributes = ecs.listAttributes()
    assert.isArray(attributes)
    assert.include(attributes, 'position')
    assert.include(attributes, 'scale')
    assert.include(attributes, 'test-component')
    assert.include(attributes, 'test-component2')
    assert.include(attributes, 'test-component3')
  })
})
