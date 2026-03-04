// @rule(js_binary)
// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(export_library = 1)
// @attr(commonjs = 1)
// @attr(full_dts = 1)

import './set-three'

import type * as api from './api'

type Ecs = typeof api

export default api

export * from './api'

export type {
  Ecs,
}
