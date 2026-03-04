import THREE from './three'
import {VIDEO_DEFAULTS} from '../shared/video-constants'
import type {World} from './world'
import type {Eid} from '../shared/schema'
import type * as THREE_TYPES from './three-types'
import {createInstanced} from '../shared/instanced'
import {getAudioManager, setOptions as setAudioOptions} from './audio-manager'
import {VideoControls} from './components'
import {events} from './event-ids'
import type {VideoControlsRuntimeSettings} from './video-types'
import {threeToEcsTextureKey, type EcsTextureKey} from './texture-types'
import {addChild} from './matrix-refresh'

type TextureId = string

// NOTE(chloe): Param is type 'any' because audio-related properties are browser-specific (i.e. not
// part of the standard HTMLVideoElement API).
function isVideoElementWithAudio(video: any) {
  // Firefox
  if (video.mozHasAudio !== undefined) {
    return video.mozHasAudio
  }
  // Chrome
  if (video.webkitAudioDecodedByteCount !== undefined) {
    return video.webkitAudioDecodedByteCount > 0
  }
  // Safari
  if (video.audioTracks && video.audioTracks.length) {
    return true
  }
  return false
}

type Video = {
  eid: Eid
  textureId: TextureId
  audioNode: THREE_TYPES.Audio | THREE_TYPES.PositionalAudio | null
  element: HTMLVideoElement
  url: string
  paused: boolean
  sourceNode: MediaElementAudioSourceNode | null
  textureKey: EcsTextureKey
}

type VideoManager = {
  addVideo: (eid: Eid, video: THREE_TYPES.VideoTexture | THREE_TYPES.VideoTexture[]) => void
  maybeRemoveVideo: (eid: Eid, textureId: string) => void
  clearVideos: (eid: Eid) => void
  applyVideoControls: (eid: Eid, videoControls: VideoControlsRuntimeSettings) => void
  pauseAllVideos: () => void
  resumeAllVideos: () => void
  muteAudibleVideos: () => void
  unmuteAudibleVideos: () => void
  getVideos: (eid: Eid) => Video[]
}

const createVideoManager = (world: World): VideoManager => {
  const audioManager = getAudioManager(world)
  const {context} = audioManager.listener

  const videosByEid: Map<Eid, Map<TextureId, Video>> = new Map()

  // NOTE(chloe): Audible entities are entities that have at least one video with audio.
  const audibleEntities: Set<Eid> = new Set()

  // NOTE(chloe): Videos added in a batch are queued in a queuedBatch until all videos in the batch
  // are ready to play through (without buffering). Once a video is ready, it is added to the
  // readyBatch. When all videos in its batch are ready, the batch attempts to play together.
  const queuedBatches: Map<Eid, Set<TextureId>> = new Map()
  const readyBatches: Map<Eid, Set<TextureId>> = new Map()

  const listenerCleanup: Map<TextureId, () => void> = new Map()

  let audioMuted = false
  let studioPaused = false

  const getVideoAudioMuted = () => context.state !== 'running' || audioMuted

  const maybeAddEndedEventListener = (video: Video) => {
    if (video.element.loop) {
      return null
    }
    const endedEventListener = () => {
      world.events.dispatch(video.eid, events.VIDEO_END, {src: video.url})
    }
    video.element.addEventListener('ended', endedEventListener)
    return endedEventListener
  }

  const playVideo = (video: Video) => {
    video.paused = false

    if (studioPaused) {
      return
    }

    const shouldMuteVideo = audibleEntities.has(video.eid) && getVideoAudioMuted()
    if (shouldMuteVideo && !video.element.muted) {
      video.element.muted = true
    }

    video.element.play()
      .catch((error) => {
        world.events.dispatch(video.eid, 'video-error', {error})
      })
  }

  const pauseVideo = (video: Video) => {
    video.paused = true
    video.element.pause()
  }

  const resumeVideos = (eid: Eid) => {
    const videos = videosByEid.get(eid)
    const queuedBatch = queuedBatches.get(eid)
    if (!videos) {
      return
    }

    videos.forEach((video) => {
      if (!video.paused && !video.element.ended && !queuedBatch?.has(video.textureId)) {
        playVideo(video)
      }
    })
  }

  const removeAudioNode = (video: Video) => {
    if (!video.audioNode) {
      return
    }

    video.audioNode.removeFromParent()
    video.audioNode.disconnect()
    video.audioNode = null
  }

  const createAudioNode = (video: Video, settings: VideoControlsRuntimeSettings) => {
    const audioNode = settings.positional
      ? new THREE.PositionalAudio(audioManager.listener)
      : new THREE.Audio(audioManager.listener)
    audioNode.gain.disconnect()
    audioNode.gain.connect(audioManager.compressor)
    // NOTE(chloe): Code used from https://github.com/mrdoob/three.js/blob/master/src/audio/Audio.js
    // from setMediaElementSource(), but we reuse source node rather than creating a new one.
    audioNode.hasPlaybackControl = false
    audioNode.sourceType = 'mediaNode'
    audioNode.source = video.sourceNode as any
    audioNode.connect()

    setAudioOptions(audioNode, video.element, settings)

    // Set parent of audio node to support positional
    if (settings.positional) {
      const object = world.three.entityToObject.get(video.eid)
      if (object) {
        addChild(object, audioNode)
      }
    }

    return audioNode
  }

  const maybeAddPlayEventListener = (video: Video) => {
    const onCanPlay = () => {
      video.element.autoplay = false

      const settings = VideoControls.has(world, video.eid)
        ? VideoControls.get(world, video.eid)
        : VIDEO_DEFAULTS

      const hasAudio = isVideoElementWithAudio(video.element)
      if (hasAudio) {
        audibleEntities.add(video.eid)
        video.sourceNode = context.createMediaElementSource(video.element)
        video.audioNode = createAudioNode(video, settings)
      } else {
        // Mute audio-less videos to avoid autoplay issues.
        video.element.muted = true
        video.element.loop = settings?.loop ?? VIDEO_DEFAULTS.loop
      }

      video.element.preservesPitch = false
      video.element.playbackRate = settings.speed ?? VIDEO_DEFAULTS.speed

      const queuedBatch = queuedBatches.get(video.eid)
      const readyBatch = readyBatches.get(video.eid)
      const isBatched = queuedBatch?.has(video.textureId)

      world.events.dispatch(video.eid, events.VIDEO_CAN_PLAY_THROUGH, {src: video.url})

      if (!queuedBatch || !readyBatch || !isBatched) {
        if (!video.paused) {
          playVideo(video)
        }
        return
      }

      if (queuedBatch.has(video.textureId)) {
        readyBatch.add(video.textureId)

        if (readyBatch.size === queuedBatch.size) {
          readyBatch.forEach((id) => {
            const readyVideo = videosByEid.get(video.eid)?.get(id)
            if (readyVideo) {
              if (!readyVideo.paused) {
                playVideo(readyVideo)
              }
            }
          })
          queuedBatches.delete(video.eid)
          readyBatches.delete(video.eid)
        }
      }
    }

    if (video.element.readyState === HTMLMediaElement.HAVE_ENOUGH_DATA) {
      onCanPlay()
      return null
    } else {
      video.element.addEventListener('canplaythrough', onCanPlay, {once: true})
      // NOTE(chloe): Enable autoplay initially to kickstart video loading on mobile devices so the
      // `canplaythrough` event can be fired. Autoplay will be disabled in the onCanPlay handler,
      // and the video is initially paused to override the autoplay behavior.
      video.element.autoplay = true
      video.element.pause()
      return onCanPlay
    }
  }

  const maybeRemoveVideo = (eid: Eid, textureId: string) => {
    const videos = videosByEid.get(eid)
    const video = videos?.get(textureId)
    if (!videos || !video) {
      return
    }

    removeAudioNode(video)
    listenerCleanup.get(textureId)?.()
    listenerCleanup.delete(textureId)
    videos.delete(textureId)
    queuedBatches.get(eid)?.delete(textureId)
    readyBatches.get(eid)?.delete(textureId)

    const isEntityNoLongerAudible = audibleEntities.has(eid) &&
      Array.from(videos.values()).every((v) => {
        const hasAudio = isVideoElementWithAudio(v.element)
        return !hasAudio
      })

    if (isEntityNoLongerAudible) {
      audibleEntities.delete(eid)
      resumeVideos(eid)
    }
  }

  const clearVideos = (eid: Eid) => {
    const videos = videosByEid.get(eid)
    if (!videos) {
      return
    }

    videos.forEach((video) => {
      listenerCleanup.get(video.textureId)?.()
      listenerCleanup.delete(video.textureId)
    })

    queuedBatches.delete(eid)
    readyBatches.delete(eid)
    videosByEid.delete(eid)
    audibleEntities.delete(eid)
  }

  const updateVideo = (video: Video, videoControls: VideoControlsRuntimeSettings) => {
    const {element, audioNode} = video
    if (audioNode) {
      // NOTE(chloe): Volume and loop are set in setAudioOptions

      const prevPositional = audioNode instanceof THREE.PositionalAudio
      const positionalSame = !!videoControls.positional === prevPositional
      if (!positionalSame) {
        removeAudioNode(video)
        if (video.sourceNode) {
          video.audioNode = createAudioNode(video, videoControls)
        }
      } else {
        setAudioOptions(audioNode, element, videoControls)
      }
    } else {
      element.loop = videoControls?.loop ?? VIDEO_DEFAULTS.loop
    }

    // NOTE(chloe): In accordance with audio, pitch affects playback rate.
    // For videos, setting speed will affect pitch for audible videos.
    video.element.playbackRate = videoControls.speed ?? VIDEO_DEFAULTS.speed
    const isQueued = queuedBatches.get(video.eid)?.has(video.textureId)

    if (videoControls.paused) {
      pauseVideo(video)
    } else if (!isQueued && !video.element.ended) {
      playVideo(video)
    }
  }

  const createVideo = (eid: Eid, texture: THREE_TYPES.VideoTexture) => {
    const videoElement = texture.image

    if (!videoElement) {
      return null
    }

    // Apply video controls if they have been previously set for this entity
    const settings = VideoControls.has(world, eid)
      ? VideoControls.get(world, eid)
      : VIDEO_DEFAULTS

    const video: Video = {
      eid,
      textureId: texture.uuid,
      audioNode: null,
      element: videoElement,
      url: texture.userData.src,
      paused: settings.paused,
      sourceNode: null,
      textureKey: threeToEcsTextureKey(texture.userData.textureKey),
    }

    if (!videosByEid.has(eid)) {
      videosByEid.set(eid, new Map())
    }
    const entity = videosByEid.get(eid)
    entity?.set(video.textureId, video)
    const playEventListener = maybeAddPlayEventListener(video)
    const endedEventListener = maybeAddEndedEventListener(video)

    listenerCleanup.set(video.textureId, () => {
      if (endedEventListener) {
        video.element.removeEventListener('ended', endedEventListener)
      }
      if (playEventListener) {
        video.element.removeEventListener('canplaythrough', playEventListener)
      }
    })

    return video
  }

  // NOTE(chloe): Videos added in a batch will be synced. This means that:
  // (1) If all videos have no audio and are not paused, they will be played once all are loaded.
  // (2) If some or all videos have audio, all videos in the batch will be played once all are
  // loaded AND if the audio manager and/or context is not paused.
  const addVideoBatch = (eid: Eid, textures: THREE_TYPES.VideoTexture[]) => {
    if (!queuedBatches.has(eid)) {
      queuedBatches.set(eid, new Set())
    }
    if (!readyBatches.has(eid)) {
      readyBatches.set(eid, new Set())
    }

    const batch = queuedBatches.get(eid)
    // First, initialize complete batch of queued videos before creating/attempting to play them.
    textures.forEach(v => batch?.add(v.uuid))
    textures.forEach(v => createVideo(eid, v))
  }

  const applyVideoControls = (eid: Eid, videoControls: VideoControlsRuntimeSettings) => {
    const videos = videosByEid.get(eid)
    if (!videos) {
      return
    }

    videos.forEach((video) => {
      updateVideo(video, videoControls)
    })
  }

  const addVideo = (eid: Eid, video: THREE_TYPES.VideoTexture | THREE_TYPES.VideoTexture[]) => {
    if (Array.isArray(video)) {
      if (video.length > 1) {
        addVideoBatch(eid, video)
      } else if (video.length === 1) {
        createVideo(eid, video[0])
      }
    } else {
      createVideo(eid, video)
    }
  }

  const resumeAllVideos = () => {
    studioPaused = false

    for (const eid of videosByEid.keys()) {
      resumeVideos(eid)
    }
  }

  const pauseAllVideos = () => {
    studioPaused = true

    for (const [, videos] of videosByEid) {
      for (const [, video] of videos) {
        video.element.pause()
      }
    }
  }

  const unmuteAudibleVideos = () => {
    audioMuted = false
    audibleEntities.forEach((eid) => {
      const videos = videosByEid.get(eid)
      videos?.forEach((video) => {
        video.element.muted = false
      })
    })
  }

  const muteAudibleVideos = () => {
    audioMuted = true

    audibleEntities.forEach((eid) => {
      const videos = videosByEid.get(eid)
      videos?.forEach((video) => {
        video.element.muted = true
      })
    })
  }

  const getVideos = (eid: Eid) => {
    const videos = videosByEid.get(eid)
    return videos ? Array.from(videos.values()) : []
  }

  return {
    getVideos,
    addVideo,
    maybeRemoveVideo,
    clearVideos,
    applyVideoControls,
    unmuteAudibleVideos,
    muteAudibleVideos,
    resumeAllVideos,
    pauseAllVideos,
  }
}

const getVideoManager = createInstanced<World, VideoManager>(createVideoManager)

export {
  getVideoManager,
}

export type {
  Video as VideoData,
  VideoManager,
}
