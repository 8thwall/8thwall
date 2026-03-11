import {useThree} from '@react-three/fiber'
import {PerspectiveCamera} from 'three'

const useCamera = (): PerspectiveCamera | null => {
  const three = useThree()
  return (three.camera instanceof PerspectiveCamera) ? three.camera : null
}

export {
  useCamera,
}
