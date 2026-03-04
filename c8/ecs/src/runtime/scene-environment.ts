import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '../shared/scene-graph'
import type {World} from './world'
import type {SceneHandle} from './scene-types'
import {assets} from './assets'
import {extractResourceUrl} from '../shared/resource'
import THREE from './three'
import {getRGBELoader, getTextureLoader} from './loaders'

const loadEnvironment = (
  world: World, sceneGraph: DeepReadonly<SceneGraph>, sceneHandle: SceneHandle, activeSpace?: string
) => {
  if (world.three.scene.environment) {
    world.three.scene.environment.dispose()
  }
  const reflections = activeSpace
    ? sceneGraph?.spaces?.[activeSpace]?.reflections
    : sceneGraph?.reflections
  const url = reflections ? extractResourceUrl(reflections) : ''

  if (url) {
    world.three.scene.environment = null
    assets.load({url}).then((asset) => {
      if (activeSpace !== sceneHandle.getActiveSpace()?.id ||
         !!world.three.scene.environment) {
        return
      }
      const ext = url.split('.').pop()
      const isSpecial = ext === 'hdr' || ext === 'exr'
      const loader = isSpecial ? getRGBELoader() : getTextureLoader()
      loader.load(asset.localUrl, (texture) => {
        // TODO: add loading promises map to prevent race condition instead of this.
        if (activeSpace !== sceneHandle.getActiveSpace()?.id ||
         !!world.three.scene.environment) {
          texture.dispose()
          return
        }
        texture.mapping = THREE.EquirectangularReflectionMapping
        world.three.scene.environment = texture
      })
    })
  } else {
    world.three.scene.environment = null
  }
}

export {
  loadEnvironment,
}
