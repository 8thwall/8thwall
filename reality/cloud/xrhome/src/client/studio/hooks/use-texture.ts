import React from 'react'
import {TextureLoader} from 'three'

import type {Resource} from '@ecs/shared/scene-graph'

import {useResourceUrl} from './resource-url'

const useTexture = (url: string | Resource) => {
  const textureUrl = useResourceUrl(url)

  return React.useMemo(() => (textureUrl
    ? new TextureLoader().load(textureUrl)
    : null
  ), [textureUrl])
}

export {
  useTexture,
}
