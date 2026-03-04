import {asmReady, isAsmReady} from './asm'
import {yogaReady, isYogaReady} from './yoga'

const ready = async () => {
  await Promise.all([asmReady(), yogaReady()])
}

const isReady = () => isAsmReady() && isYogaReady()

export {
  ready,
  isReady,
}
