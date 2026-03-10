// @rule(js_binary)
// @attr(target = "node")

// @dep(//bzl/examples/js/import:type-only-lib-declarations)

// @inliner-skip-next
import type * as Lib from '@repo/bzl/examples/js/import/type-only-lib'

const myFn = (argument: Lib.ExampleType) => {
  // eslint-disable-next-line no-console
  console.log(argument)
}

myFn('v1')
