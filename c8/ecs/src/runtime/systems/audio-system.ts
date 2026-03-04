import type {World} from '../world'
import type {Eid} from '../../shared/schema'
import {Audio} from '../components'
import {makeSystemHelper} from './system-helper'
import {getAudioManager} from '../audio-manager'
import {addChild} from '../matrix-refresh'

const makeAudioSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Audio)
  const audioManager = getAudioManager(world)

  const clearAudio = (eid: Eid) => {
    audioManager.removeSound(eid)
  }

  const applyAudio = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const audio = Audio.get(world, eid)
    if (audio.url) {
      const sound = audioManager.addSound(eid, audio)
      if (sound) {
        addChild(object, sound.audioNode)
      }
    }
  }

  const handleAudioChanged = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    audioManager.updateSound(eid, Audio.get(world, eid))
  }

  return () => {
    exit(world).forEach(clearAudio)
    enter(world).forEach(applyAudio)
    changed(world).forEach(handleAudioChanged)
  }
}

export {
  makeAudioSystem,
}
