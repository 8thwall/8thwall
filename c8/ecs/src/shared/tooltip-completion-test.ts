// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)
// @attr[](data = "//c8/ecs/gen:runtime-tooltip-data.json")
// @attr[](tags = "manual")

import path from 'path'
import fs from 'fs'

import {describe, it, assert} from '@repo/bzl/js/chai-js'

import '../runtime/test-env'
import {ecs} from '../runtime/test-runtime-lib'
import type {RootAttribute} from '../runtime/world-attribute'
import type {RuntimeTooltipData} from './tooltip-types'

const sourcePath = path.join(
  process.env.RUNFILES_DIR!,
  '_main/c8/ecs/gen/runtime-tooltip-data.json'
)

const tooltipData: RuntimeTooltipData = JSON.parse(fs.readFileSync(sourcePath, 'utf8'))

const allComponents = Object.entries(ecs)
  .filter((e: [any, any]): e is [string, RootAttribute<{}>] => (
    typeof e[1] === 'object' && 'orderedSchema' in e[1]
  ))

describe('Tooltip Completion', () => {
  allComponents.forEach(([componentName, component]) => {
    it(`Runtime component: ${componentName} should have a doc entry`, () => {
      assert.isOk(tooltipData.components[componentName])
    })

    it(`Runtime component: ${componentName} should have a doc link`, () => {
      assert.isOk(tooltipData.components[componentName]?.link)
    })

    component.orderedSchema.forEach(([key]) => {
      const runtimeKey = `${componentName}.${key}`
      it(`Runtime property: ${runtimeKey} has a doc entry`, () => {
        const propData = tooltipData.propertiesByRuntime[runtimeKey]
        assert.isOk(propData)
      })
    })
  })

  const documentedEntries = Object.values(tooltipData.propertiesByRuntime)

  documentedEntries.forEach((entry) => {
    const {accessor, name} = entry
    const component: RootAttribute<Record<string, any>> = (ecs as any)[accessor]
    const schemaEntry = component?.orderedSchema?.find(([key]) => key === name)

    it(`ecs.${accessor}.${name} exists in the runtime`, () => {
      assert.isObject(component, `Component ${accessor} should exist`)
      assert.isOk(component.forWorld)
      assert.isOk(schemaEntry)
    })
  })
})
