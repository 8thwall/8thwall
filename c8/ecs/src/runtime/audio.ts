import type {World} from './world'
import type {Eid} from '../shared/schema'
import {getAudioManager} from './audio-manager'

const getSoundElement = (world: World, eid: Eid) => {
  const audioManager = getAudioManager(world)
  return audioManager.getSound(eid)?.element
}

const getCurrentTime = (world: World, eid: Eid) => {
  const element = getSoundElement(world, eid)
  if (!element) {
    throw new Error(`Audio for ${eid} does not exist, cannot get current time`)
  }
  return element.currentTime
}

const setCurrentTime = (world: World, eid: Eid, time: number) => {
  const element = getSoundElement(world, eid)
  if (!element) {
    throw new Error(`Audio for ${eid} does not exist, cannot set current time`)
  }
  element.currentTime = time
}

const audio = {
  getCurrentTime,
  setCurrentTime,
}

export {
  audio,
}
