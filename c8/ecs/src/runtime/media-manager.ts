import type {World} from './world'
import {createInstanced} from '../shared/instanced'
import type {AudioControls} from './audio-controls-types'
import {getAudioManager} from './audio-manager'
import {getVideoManager} from './video-manager'

type InternalMedia = {
  destroy: () => void
  setStudioPause: (paused: boolean) => void
}

type InternalMediaManager = {
  audioControls: AudioControls
  internalMedia: InternalMedia
}

const createMediaManager = (world: World): InternalMediaManager => {
  const audioManager = getAudioManager(world)
  const videoManager = getVideoManager(world)

  // note: studioPaused is for saving the state internally, audioPaused is set externally by world
  let audioPaused = false
  let studioPaused = false
  let volume = 1

  const start = () => {
    if (studioPaused || audioPaused) {
      return
    }
    audioManager.listener.context.resume().then(() => {
      audioManager.resumeSounds()
      videoManager.unmuteAudibleVideos()
    })
  }

  const setStudioPause = (paused: boolean) => {
    studioPaused = paused
    if (paused) {
      audioManager.pauseSounds()
      videoManager.pauseAllVideos()
    } else {
      audioManager.listener.context.resume().then(() => {
        // note: resume world if the user hasn't paused the audio
        if (!audioPaused) {
          audioManager.resumeSounds()
          videoManager.resumeAllVideos()
        }
      })
    }
  }

  // NOTE(chloe): The following functions are exposed as world.audio and control audio playback for
  // both audio and audible videos.
  const play = () => {
    audioPaused = false
    if (studioPaused) {
      return
    }
    audioManager.listener.context.resume().then(() => {
      audioManager.resumeSounds()
      videoManager.unmuteAudibleVideos()
    })
  }

  const pause = () => {
    audioPaused = true
    if (studioPaused) {
      return
    }
    audioManager.pauseSounds()
    videoManager.muteAudibleVideos()
  }

  const setVolume = (newVolume: number) => {
    const normalizedVolume = Math.max(0, Math.min(1, newVolume))
    audioManager.listener.gain.gain.value = normalizedVolume
    // note: if volume is muted, we don't save the current value
    if (normalizedVolume > 0) {
      volume = normalizedVolume
    }
  }

  const mute = () => {
    setVolume(0)
  }

  const unmute = () => {
    setVolume(volume)
  }

  // TODO (Julie): Think of a better way to handle this
  document.addEventListener('click', start, {once: true})

  const destroy = () => {
    document.removeEventListener('click', start)
  }

  const audioControls = {
    mute,
    unmute,
    pause,
    play,
    setVolume,
  }

  const internalMedia = {
    destroy,
    setStudioPause,
  }

  return {audioControls, internalMedia}
}

const getMediaManager = createInstanced<World, InternalMediaManager>(createMediaManager)

export {
  getMediaManager,
}

export type {
  InternalMediaManager,
}
