import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {VideoControls} from '../components'
import {makeSystemHelper} from './system-helper'
import {getVideoManager} from '../video-manager'

// NOTE(chloe): This system is only responsible for applying entity-level video playback control.
// It does not create the video textures themselves or hook them into the audio context as these
// will be done on a per-texture basis.
const makeVideoControlsSystem = (world: World) => {
  const {enter, changed} = makeSystemHelper(VideoControls)

  const applyControls = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const videoControls = VideoControls.get(world, eid)
    getVideoManager(world).applyVideoControls(eid, videoControls)
  }

  return () => {
    enter(world).forEach(applyControls)
    changed(world).forEach(applyControls)
  }
}

export {
  makeVideoControlsSystem,
}
