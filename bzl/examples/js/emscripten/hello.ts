// @rule(js_binary)
// @name(hello-emscripten)
// @dep(:hello-emscripten-wasm)

import HELLOMODULE from 'bzl/examples/js/emscripten/hello-emscripten-wasm'

const run = async () => {
  const module = await HELLOMODULE({})
  // eslint-disable-next-line no-console
  console.log('The answer is:', module._c8EmAsm_getAnswer())
}
run()
