import {isEqual} from 'lodash-es'

import type {DeepReadonly} from 'ts-essentials'

import type {Fog} from '../shared/scene-graph'
import type {World} from './world'
import THREE from './three'
import type {InternalEffectsManager, EffectsManagerSky} from './effects-manager-types'
import {disposeObject} from './dispose'
import {addChild} from './matrix-refresh'
import {createSceneBackground, updateSkyBoxMaterial} from './create-scene-background'
import type {Mesh} from './three-types'

const createEffectsManager = (world: World): InternalEffectsManager => {
  let fog_: DeepReadonly<Fog> | undefined
  let sky_: DeepReadonly<EffectsManagerSky> | undefined
  let skyObj_: Mesh | undefined
  let skyHiddenByXr_ = false

  let attached_ = false

  const updateFog = () => {
    if (!fog_ || fog_.type === 'none') {
      world.three.scene.fog = null
    } else if (fog_.type === 'linear') {
      world.three.scene.fog = new THREE.Fog(fog_.color, fog_.near, fog_.far)
    } else if (fog_.type === 'exponential') {
      world.three.scene.fog = new THREE.FogExp2(fog_.color, fog_.density)
    }
  }

  const setFog = (newFog: DeepReadonly<Fog> | undefined) => {
    const oldFog = fog_
    fog_ = newFog
    if (!attached_) {
      return
    }
    if (isEqual(newFog, oldFog)) {
      return
    }

    updateFog()
  }

  const getFog = (): DeepReadonly<Fog> | undefined => fog_

  const updateSky = async () => {
    if (!skyObj_) {
      skyObj_ = await createSceneBackground(sky_)
      if (skyObj_ && !skyHiddenByXr_) {
        addChild(world.three.scene, skyObj_)
      }
      return
    }

    const material = await updateSkyBoxMaterial(skyObj_, sky_)
    if (!material) {
      disposeObject(skyObj_)
      world.three.scene.remove(skyObj_)
      skyObj_ = undefined
    }
  }

  const setSky = (newSky: DeepReadonly<EffectsManagerSky> | undefined) => {
    const oldSky = sky_
    sky_ = newSky
    if (!attached_) {
      return
    }
    if (isEqual(newSky, oldSky)) {
      return
    }

    updateSky()
  }

  const getSky = (): DeepReadonly<EffectsManagerSky> | undefined => sky_

  // Note(Dale): Internal methods
  const _hideSkyForXr = () => {
    skyHiddenByXr_ = true
    if (skyObj_) {
      world.three.scene.remove(skyObj_)
    }
  }

  const _showSkyForXr = () => {
    skyHiddenByXr_ = false
    if (skyObj_) {
      addChild(world.three.scene, skyObj_)
    }
  }

  const attach = () => {
    if (attached_) {
      return
    }
    attached_ = true
    updateFog()
    updateSky()
    if (skyObj_ && !skyHiddenByXr_) {
      addChild(world.three.scene, skyObj_)
    }
  }

  const detach = () => {
    if (!attached_) {
      return
    }
    attached_ = false
    world.three.scene.fog = null
    if (skyObj_) {
      world.three.scene.remove(skyObj_)
    }
  }

  return {
    setFog,
    getFog,
    setSky,
    getSky,
    _hideSkyForXr,
    _showSkyForXr,
    attach,
    detach,
  }
}

export {
  createEffectsManager,
}
