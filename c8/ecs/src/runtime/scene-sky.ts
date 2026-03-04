import type {DeepReadonly} from 'ts-essentials'

import {isEqual} from 'lodash-es'

import type {SceneGraph, Sky} from '../shared/scene-graph'
import type {World} from './world'
import {getSkyFromSceneGraph} from '../shared/get-sky-from-scene-graph'
import {extractResourceUrl} from '../shared/resource'
import type {EffectsManagerSky} from './effects-manager-types'

const convertSceneSkyToEffectsManagerSky = (
  sky: DeepReadonly<Sky> | undefined
): DeepReadonly<EffectsManagerSky> | undefined => {
  if (!sky) {
    return undefined
  }

  if (sky.type === 'image') {
    return {
      type: 'image',
      src: sky.src ? extractResourceUrl(sky.src) : undefined,
    }
  }

  return sky
}

const setSky = (
  world: World,
  sceneGraph: DeepReadonly<SceneGraph>,
  activeSpace: string | undefined,
  previousSpaceSky?: DeepReadonly<Sky> | undefined
): DeepReadonly<Sky> | undefined => {
  const sky = getSkyFromSceneGraph(sceneGraph, activeSpace)

  if (!isEqual(sky, previousSpaceSky)) {
    world.effects.setSky(convertSceneSkyToEffectsManagerSky(sky))
  }

  return sky
}

export {
  convertSceneSkyToEffectsManagerSky,
  setSky,
}
