// @rule(js_binary)
// @attr(esnext = 1)

import {log} from './log'

log('Initial load')

const run = async () => {
  // @ts-ignore
  const {doSomething1} = await import(/* webpackIgnore: true */'./chunk1.js')

  doSomething1()

  // @ts-ignore
  await import(/* webpackIgnore: true */'./chunk2.js')
    .then(chunk2 => chunk2.doSomething2())
}

run()
