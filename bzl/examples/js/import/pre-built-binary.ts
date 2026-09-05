// @rule(js_binary)
// @package(npm-ecs)
// @attr(export_library = 1)
// @attr(externals = "three")
// @attr(target = "node")
// @attr(commonjs = 1)

import * as THREE from 'three'

const doThing = () => {
  // eslint-disable-next-line no-console
  console.log('Eslint is version: ', THREE.UnsignedByteType)
}

export {
  doThing,
}
