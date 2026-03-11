import {useThree} from '@react-three/fiber'
import React from 'react'
import {EquirectangularReflectionMapping, TextureLoader} from 'three'
import {RGBELoader} from 'three/examples/jsm/loaders/RGBELoader'

interface SceneEnvironmentProps {
  reflectionsUrl: string
}

const SceneEnvironment: React.FC<SceneEnvironmentProps> = ({reflectionsUrl}) => {
  const {scene} = useThree()
  React.useLayoutEffect(() => {
    if (reflectionsUrl) {
      const ext = reflectionsUrl.split('.').pop()
      const isSpecial = ext === 'hdr' || ext === 'exr'
      const loader = isSpecial ? new RGBELoader() : new TextureLoader()
      loader.load(reflectionsUrl, (texture) => {
        texture.mapping = EquirectangularReflectionMapping
        scene.environment = texture
      })
    } else if (scene.environment) {
      scene.environment.dispose()
      scene.environment = null
    }
  },
  [scene, reflectionsUrl])

  return null
}

export default SceneEnvironment
