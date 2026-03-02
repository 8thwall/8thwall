// @rule(js_binary)
// @attr(esnext = 1)
// @attr(export_library = 1)

import {log} from './log'

log('chunk1 loaded')

const doSomething1 = async () => {
  log('doSomething1')
}

export {
  doSomething1,
}
