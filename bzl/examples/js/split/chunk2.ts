// @rule(js_binary)
// @attr(esnext = 1)
// @attr(export_library = 1)

/* eslint-disable no-console */
// eslint-disable-next-line import/no-unresolved
// @dep(:chunk2-wasm)
import Module from 'bzl/examples/js/split/chunk2-wasm'

import {log} from './log'

log('chunk2 loaded')

const doSomething2 = async () => {
  const module = await Module({
    print: (...args) => console.log(...args),
  })
  const val = module._c8EmAsm_getValue()
  log(`doSomething2 got value from wasm: ${val}`)
}

export {
  doSomething2,
}
