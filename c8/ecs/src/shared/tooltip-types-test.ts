// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)
// @attr[](data = "//c8/ecs/gen:runtime-tooltip-data.json")

import path from 'path'
import fs from 'fs'

import {describe, it, assert} from '@nia/bzl/js/chai-js'

import '../runtime/test-env'
import {ecs} from '../runtime/test-runtime-lib'
import type {RootAttribute} from '../runtime/world-attribute'
import type {RuntimeTooltipData} from './tooltip-types'

const sourcePath = path.join(
  process.env.RUNFILES_DIR!,
  '_main/c8/ecs/gen/runtime-tooltip-data.json'
)

const tooltipData: RuntimeTooltipData = JSON.parse(fs.readFileSync(sourcePath, 'utf8'))

const numberTypes = new Set<string>([
  ecs.f32,
  ecs.f64,
  ecs.i32,
  ecs.ui8,
  ecs.ui32,
])

const OVERRIDES: Record<string, [string, string]> = {
  'Collider.shape': [ecs.ui32, 'ecs.ColliderShape'],
  'Collider.type': [ecs.ui8, 'ecs.ColliderType'],
}

describe('Tooltip Types', () => {
  const documentedEntries = Object.values(tooltipData.propertiesByRuntime)

  documentedEntries.forEach((entry) => {
    const {accessor, name, type} = entry
    const component: RootAttribute<Record<string, any>> = (ecs as any)[accessor]
    const schemaEntry = component?.orderedSchema?.find(([key]) => key === name)

    it(`ecs.${accessor}.${name} has the correct type`, () => {
      const schemaType = schemaEntry![1]
      assert.isOk(schemaType)
      const override = OVERRIDES[`${accessor}.${name}`]
      if (override) {
        const [expectedType, expectedName] = override
        assert.strictEqual(schemaType, expectedType)
        assert.strictEqual(type, expectedName)
      } else if (numberTypes.has(schemaType)) {
        assert.include(['number', schemaType], type)
      } else {
        assert.strictEqual(schemaType, type)
      }
    })
  })
})
