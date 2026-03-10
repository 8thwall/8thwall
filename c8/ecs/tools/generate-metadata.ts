// @rule(js_cli)
// @package(npm-ecs)
// @attr(esnext = 1)
// @dep(//c8/ecs/tools:generated-features-list)
// @attr[](data = "//c8/ecs/src/runtime:animation.ts")
// @attr[](data = "//c8/ecs/src/runtime:particles.ts")
// @attr[](data = "//c8/ecs/src/runtime:orbit-controls.ts")
// @attr[](data = "//c8/ecs/src/runtime/xr:xr-face-components.ts")
// @attr[](data = "//c8/ecs/src/runtime:fly-controller.ts")
import {promises as fs} from 'fs'
import path from 'path'

import * as FEATURES from '@repo/c8/ecs/tools/generated-features-list'

import {EDITIONS} from '@repo/c8/ecs/src/shared/features/edition'

import type {RuntimeMetadata} from '@repo/c8/ecs/src/shared/runtime-version'
import {parseComponentAst} from '@repo/c8/ecs/src/shared/parse-component-ast'

const FILES_TO_INCLUDE = [
  'src/runtime/animation.ts',
  'src/runtime/particles.ts',
  'src/runtime/orbit-controls.ts',
  'src/runtime/xr/xr-face-components.ts',
  'src/runtime/fly-controller.ts',
].map(e => path.join(process.env.RUNFILES_DIR!, '_main/c8/ecs/', e))

const [buildTime, commitId, version] = process.argv.slice(2)
if (!buildTime || !commitId || !version) {
  // eslint-disable-next-line no-console
  console.warn('Generating metadata.json without buildTime/commitId/version')
}

const files = await Promise.all(FILES_TO_INCLUDE.map(file => fs.readFile(file, 'utf8')))
const componentSchema = files.flatMap((file) => {
  const {componentData, errors} = parseComponentAst(file)
  if (errors.length) {
    throw new Error(errors.map(error => error.message).join('\n'))
  }
  return componentData.map(
    (data) => {
      delete data.location
      return data
    }
  )
})

const EDITION = EDITIONS.length

const features: RuntimeMetadata['features'] = Object.fromEntries(Object.values(FEATURES)
  .filter(e => typeof e === 'object' && !Array.isArray(e))
  .filter(e => e.edition >= EDITION)
  .filter(e => ('testing' in e ? !e.testing : true))
  .map(e => [e.name, true]))

if (EDITION) {
  features.edition = EDITION
}

const metadata: RuntimeMetadata = {
  version,
  buildTime,
  commitId,
  features,
  componentSchema,
  cloneableComponents: [],
}

// eslint-disable-next-line no-console
console.log(JSON.stringify(metadata, null, 2))
