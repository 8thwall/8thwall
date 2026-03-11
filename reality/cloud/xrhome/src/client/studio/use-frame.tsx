import type React from 'react'
import {RenderCallback, useFrame} from '@react-three/fiber'

interface UseFrameProps {
  callback: RenderCallback
  renderPriority?: number
}

const UseFrame: React.FC<UseFrameProps> = ({callback, renderPriority = 0}) => {
  useFrame(callback, renderPriority)
  return null
}

export {
  UseFrame,
}
