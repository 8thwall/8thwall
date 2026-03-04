/* eslint-disable */
// @ts-ignore TS2307
// @attr[](srcs = ":ecs-asm")
// @dep(//c8/ecs:ecs-asm)
// @inliner-skip-next
import ECS from '@nia/c8/ecs/src/runtime/ecs-asm'
/* eslint-enable */

import type {EcsAsm} from '@nia/c8/ecs/gen/asm-types'

let verbose = false
let loaded = false

// @ts-expect-error (Late initialized)
const asm: EcsAsm = {}

const ecsPromise = ECS({
  print: (...args: unknown[]) => {
    if (verbose) {
      // eslint-disable-next-line no-console
      console.log(...args)
    }
  },
})

const PREFIX_TO_STRIP = '_c8EmAsm_'
const fixFunctions = (mod: any) => {
  Object.entries(mod).forEach(([key, value]) => {
    if (key.startsWith(PREFIX_TO_STRIP) && typeof value === 'function') {
      mod[key.slice(PREFIX_TO_STRIP.length)] = value
    }
  })
}

ecsPromise.then((mod: any) => {
  Object.assign(asm, mod)
  fixFunctions(asm)
  asm.dataView = new DataView(asm.HEAPU8.buffer)
  loaded = true
})

const asmReady = async () => {
  await ecsPromise
}

const asmVerbose = (v: boolean) => {
  verbose = v
}

const isReady = () => loaded

export {
  asm,
  asmReady,
  isReady as isAsmReady,
  asmVerbose,
}
