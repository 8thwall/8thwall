// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {describe, it, assert} from '@nia/bzl/js/chai-js'

import {NUMERIC_SCHEMA_TYPES} from './component-constants'
import {
  isBasicTypeNumber, getBasicType, hasAttributesWithPropertiesOfType,
} from './schema-utility'
import type {Schema} from './schema'

describe('Cloud Studio - Schema Utility', () => {
  describe('isBasicTypeNumber', () => {
    it('Returns true for any number', () => {
      NUMERIC_SCHEMA_TYPES.forEach(type => assert.isTrue(isBasicTypeNumber(type)))
    })
    it('Returns false for any non-number', () => {
      assert.isFalse(isBasicTypeNumber('string'))
      assert.isFalse(isBasicTypeNumber('boolean'))
      assert.isFalse(isBasicTypeNumber('eid'))
    })
  })
  describe('getBasicType', () => {
    it('Returns "number" for any number', () => {
      NUMERIC_SCHEMA_TYPES.forEach(type => assert.isTrue(getBasicType(type) === 'number'))
    })
    it('Returns same name for other types', () => {
      assert.isTrue(getBasicType('string') === 'string')
      assert.isTrue(getBasicType('boolean') === 'boolean')
      assert.isTrue(getBasicType('eid') === 'eid')
    })
  })
  describe('hasAttributesWithPropertiesOfType', () => {
    it('Returns true if any properties of type found', () => {
      const numSchema: Schema = {
        x: 'f32',
        y: 'f32',
        z: 'f32',
      }
      assert.isTrue(hasAttributesWithPropertiesOfType(numSchema, 'number'))
      const stringSchema: Schema = {
        s: 'string',
      }
      assert.isTrue(hasAttributesWithPropertiesOfType(stringSchema, 'string'))
      const boolSchema: Schema = {
        b: 'boolean',
      }
      assert.isTrue(hasAttributesWithPropertiesOfType(boolSchema, 'boolean'))
    })
    it('Returns false if no properties of type found', () => {
      const numSchema: Schema = {
        n: 'f32',
        b: 'boolean',
        e: 'eid',
      }
      assert.isFalse(hasAttributesWithPropertiesOfType(numSchema, 'string'))
      const stringSchema: Schema = {
        s: 'string',
        b: 'boolean',
        e: 'eid',
      }
      assert.isFalse(hasAttributesWithPropertiesOfType(stringSchema, 'number'))
      const boolSchema: Schema = {
        n: 'f32',
        b: 'boolean',
        e: 'eid',
      }
      assert.isFalse(hasAttributesWithPropertiesOfType(boolSchema, 'string'))
    })
  })
})
