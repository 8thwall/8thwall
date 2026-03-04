import {
  DEFAULT_VOLUME, DEFAULT_PITCH, DEFAULT_REF_DISTANCE, DEFAULT_DISTANCE_MODEL,
  DEFAULT_ROLLOFF_FACTOR, DEFAULT_MAX_DISTANCE, DEFAULT_COMPRESSOR_THRESHOLD,
  DEFAULT_COMPRESSOR_KNEE, DEFAULT_COMPRESSOR_RATIO, DEFAULT_COMPRESSOR_ATTACK,
  DEFAULT_COMPRESSOR_RELEASE,
} from '../shared/audio-constants'
import type {World} from './world'
import THREE from './three'
import type {Eid} from '../shared/schema'
import {assets} from './assets'
import type * as THREE_TYPES from './three-types'
import {events} from './event-ids'
import {createInstanced} from '../shared/instanced'
import {addChild} from './matrix-refresh'

type AudioSettings = {
  url: string
  volume?: number
  loop?: boolean
  pitch?: number
  paused?: boolean
  positional?: boolean
  refDistance?: number
  rolloffFactor?: number
  distanceModel?: string
  maxDistance?: number
}

type Sound = {
  id: Eid
  audioNode: THREE_TYPES.Audio | THREE_TYPES.PositionalAudio
  element: HTMLAudioElement
  controller: AbortController
  url: string
  paused: boolean
}

type InternalAudio = {
  listener: THREE_TYPES.AudioListener
  compressor: DynamicsCompressorNode
  addSound: (eid: Eid, audio: AudioSettings) => Sound | null
  getSound: (eid: Eid) => Sound | undefined
  removeSound: (eid: Eid) => void
  updateSound: (eid: Eid, newAudio: AudioSettings) => void
  resumeSounds: () => void
  pauseSounds: () => void
}

const DistanceModelValues = ['exponential', 'inverse', 'linear'] as const

const getValidDistanceModel = (distanceModel: any): string => (
  DistanceModelValues.includes(distanceModel) ? distanceModel : DEFAULT_DISTANCE_MODEL
)

const setOptions = (
  audioNode: THREE_TYPES.Audio | THREE_TYPES.PositionalAudio,
  element: HTMLMediaElement,
  options?: Omit<AudioSettings, 'url'>
) => {
  audioNode.setVolume(options?.volume ?? DEFAULT_VOLUME)
  element.loop = !!options?.loop

  if (audioNode instanceof THREE.PositionalAudio) {
    audioNode.setRefDistance(options?.refDistance ?? DEFAULT_REF_DISTANCE)
    audioNode.setDistanceModel(getValidDistanceModel(options?.distanceModel))
    audioNode.setRolloffFactor(options?.rolloffFactor ?? DEFAULT_ROLLOFF_FACTOR)
    audioNode.setMaxDistance(options?.maxDistance ?? DEFAULT_MAX_DISTANCE)
  }
}

const createAudioManager = (world: World): InternalAudio => {
  const listener = new THREE.AudioListener()
  const context = listener.context as AudioContext

  // use a compressor to avoid clipping and crackling sounds
  const compressor = context.createDynamicsCompressor()
  compressor.threshold.setValueAtTime(DEFAULT_COMPRESSOR_THRESHOLD, context.currentTime)
  compressor.knee.setValueAtTime(DEFAULT_COMPRESSOR_KNEE, context.currentTime)
  compressor.ratio.setValueAtTime(DEFAULT_COMPRESSOR_RATIO, context.currentTime)
  compressor.attack.setValueAtTime(DEFAULT_COMPRESSOR_ATTACK, context.currentTime)
  compressor.release.setValueAtTime(DEFAULT_COMPRESSOR_RELEASE, context.currentTime)
  compressor.connect(listener.gain)

  const sounds: Map<Eid, Sound> = new Map()
  const endedEventCallbacks: Map<Eid, (event: Event) => void> = new Map()
  let paused = false

  const maybeAddEndedEventListener = (sound: Sound) => {
    if (endedEventCallbacks.get(sound.id) || sound.element.loop) {
      return
    }
    const endedEventListener = () => {
      world.events.dispatch(sound.id, events.AUDIO_END)
    }
    sound.element.addEventListener('ended', endedEventListener)
    endedEventCallbacks.set(sound.id, endedEventListener)
  }

  const maybeRemoveEndedEventListener = (sound: Sound) => {
    const endedEventCallback = endedEventCallbacks.get(sound.id)
    if (endedEventCallback) {
      sound.element.removeEventListener('ended', endedEventCallback)
      endedEventCallbacks.delete(sound.id)
    }
  }

  const playSound = (sound: Sound) => {
    sound.paused = false

    sound.element.autoplay = true
    if (paused) {
      return
    }

    sound.element.play()
      .catch((error) => {
        world.events.dispatch(sound.id, 'audio-error', {error})
      })
  }

  const pauseSound = (sound: Sound) => {
    sound.paused = true
    sound.element.pause()
  }

  const addSound = (eid: Eid, audio: AudioSettings) => {
    if (!audio.url) {
      return null
    }

    const controller = new AbortController()
    const {signal} = controller
    const audioNode = audio.positional
      ? new THREE.PositionalAudio(listener)
      : new THREE.Audio(listener)

    // connect all audio sources to output compressor
    audioNode.gain.disconnect()
    audioNode.gain.connect(compressor)

    const audioElement = new Audio()

    audioElement.addEventListener('canplaythrough', () => {
      world.events.dispatch(eid, events.AUDIO_CAN_PLAY_THROUGH)
    })

    assets.load({url: audio.url}).then((asset) => {
      if (signal.aborted) {
        return
      }
      audioElement.src = asset.localUrl
      // playbackRate directly affects the pitch of the audio (sample rate conversion)
      // https://developer.mozilla.org/en-US/docs/Web/API/HTMLMediaElement/playbackRate
      // Need to set defaultPlaybackRate rather than playbackRate or else the playbackRate/pitch
      // will be reset to default after load.
      audioElement.defaultPlaybackRate = audio.pitch ?? DEFAULT_PITCH
      ;(audioElement as any).preservesPitch = false
      audioElement.load()
    })

    audioElement.crossOrigin = 'anonymous'
    setOptions(audioNode, audioElement, audio)
    audioNode.setMediaElementSource(audioElement)

    const sound = {
      id: eid,
      audioNode,
      element: audioElement,
      controller,
      url: audio.url,
      paused: !!audio.paused,
    }
    sounds.set(eid, sound)
    maybeAddEndedEventListener(sound)
    if (!audio.paused) {
      playSound(sound)
    }
    return sound
  }

  const removeSound = (eid: Eid) => {
    const sound = sounds.get(eid)
    if (!sound) {
      return
    }

    sound.controller.abort()

    sound.element.pause()
    maybeRemoveEndedEventListener(sound)
    sound.audioNode.removeFromParent()
    sounds.delete(eid)
  }

  const updateSound = (eid: Eid, newAudio: AudioSettings) => {
    const sound = sounds.get(eid)
    if (!sound) {
      return
    }

    const {element, audioNode, url: oldAudioUrl} = sound
    const positionalSame = !!newAudio.positional === (audioNode instanceof THREE.PositionalAudio)
    const audioUrlSame = oldAudioUrl && newAudio && newAudio.url === oldAudioUrl

    if (audioUrlSame && positionalSame) {
      element.playbackRate = newAudio.pitch ?? DEFAULT_PITCH
      setOptions(audioNode, element, newAudio)
      if (sound.element.loop) {
        maybeRemoveEndedEventListener(sound)
      }
      maybeAddEndedEventListener(sound)
      if (newAudio.paused) {
        pauseSound(sound)
      } else {
        playSound(sound)
      }
    } else {
      const {parent} = audioNode
      removeSound(eid)
      const newSound = addSound(eid, newAudio)
      if (parent && newSound) {
        addChild(parent, newSound.audioNode)
      }
    }
  }

  const pauseSounds = () => {
    paused = true
    sounds.forEach((sound) => {
      sound.element.pause()
    })
  }

  const resumeSounds = () => {
    paused = false
    sounds.forEach((sound) => {
      if (sound.paused || sound.element.ended) {
        return
      }

      sound.element.play()
        .catch((error: Error) => {
          world.events.dispatch(sound.id, 'audio-error', {error})
        })
    })
  }

  const getSound = (eid: Eid) => sounds.get(eid)

  const internalAudio = {
    listener,
    compressor,
    addSound,
    getSound,
    removeSound,
    resumeSounds,
    pauseSounds,
    updateSound,
  }

  return internalAudio
}

const getAudioManager = createInstanced<World, InternalAudio>(createAudioManager)

export {
  getAudioManager,
  setOptions,
}

export type {
  InternalAudio,
}
