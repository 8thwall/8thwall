// @attr(testonly = 1)
// @attr[](srcs = "test-runtime-lib-inner.d.ts")
// @attr[](srcs = "test-runtime-lib-inner.js")
// @dep(:test-runtime-types)
// @attr[](data = ":test-runtime-bin")

// @inliner-skip-next
import * as ecs from './test-runtime-lib-inner'

export {
  ecs,
}
