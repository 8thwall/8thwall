import React from 'react'
import type {Resource} from '@ecs/shared/scene-graph'

import {useResourceUrl} from './resource-url'

const createVideo = (url: string) => {
  const video = document.createElement('video')
  video.src = url
  video.crossOrigin = 'anonymous'
  video.loop = true
  video.muted = true
  video.playsInline = true
  video.oncanplaythrough = () => {
    video.play()
  }
  return video
}

const useVideo = (url: string | Resource): HTMLVideoElement | null => {
  const videoUrl = useResourceUrl(url)

  return React.useMemo(() => (
    videoUrl
      ? createVideo(videoUrl)
      : null
  ), [videoUrl])
}

export {
  useVideo,
}
