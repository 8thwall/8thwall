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

const OVERRIDES: Record<string, [any, string]> = {
  'Collider.shape': [ecs.ColliderShape.Box, 'ecs.ColliderShape.Box'],
  'Collider.type': [ecs.ColliderType.Static, 'ecs.ColliderType.Static'],
  'Light.angle': [Math.PI / 3, 'Math.PI / 3'],
  'MapPoint.meters': [100 / 3, '33.33'],
}

describe('Tooltip Defaults', () => {
  const documentedEntries = Object.values(tooltipData.propertiesByRuntime)

  documentedEntries.forEach((entry) => {
    const {accessor, name, defaultValue} = entry
    const component: RootAttribute<Record<string, any>> = (ecs as any)[accessor]

    const stringifyValue = (v: any) => {
      if (typeof v === 'string') {
        return `'${v}'`
      }
      if (v === 0n) {
        return 'undefined'
      }
      return String(v)
    }

    if (defaultValue) {
      it(`ecs.${accessor}.${name} has the correct default`, () => {
        const schemaDefault = component.defaults?.[name]
        assert.isDefined(schemaDefault, 'Runtime default value should be defined')
        const override = OVERRIDES[`${accessor}.${name}`]
        if (override) {
          assert.strictEqual(defaultValue, override[1])
          assert.strictEqual(schemaDefault, override[0])
        } else {
          const defaultAsString = stringifyValue(schemaDefault)
          assert.strictEqual(
            defaultValue,
            defaultAsString,
            `The tooltip data has: ${defaultValue}, runtime has: ${defaultAsString}`
          )
        }
      })
    }
  })
})
