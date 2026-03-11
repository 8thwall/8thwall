import React from 'react'
import type {Resource} from '@ecs/shared/scene-graph'
import {CanvasTexture} from 'three'
import {extractResourceUrl} from '@ecs/shared/resource'

import {useCanvasPool} from '../../common/resource-pool'
import {useResourceUrl} from './resource-url'
import {ASSET_EXT_TO_KIND} from '../common/studio-files'
import {fileExt} from '../../editor/editor-common'

const CANVAS_MAX_DIMENSION = 256

const useVideoThumbnail = (url: string | Resource, seekTime: number = 0): CanvasTexture | null => {
  const videoUrl = useResourceUrl(url)
  const canvasPool = useCanvasPool()
  const [texture, setTexture] = React.useState<CanvasTexture | null>(null)

  React.useEffect(() => {
    if (!videoUrl || ASSET_EXT_TO_KIND[fileExt(videoUrl)] !== 'video') {
      return undefined
    }

    let isAborted = false

    let video: HTMLVideoElement | null = document.createElement('video')
    video.crossOrigin = 'anonymous'
    video.src = videoUrl
    video.muted = true
    video.autoplay = false
    video.playsInline = true
    video.preload = 'metadata'

    const cleanup = () => {
      if (video) {
        video.onloadedmetadata = null
        video.onseeked = null
        video.pause()
        video.removeAttribute('src')
        video.load()
        video = null
      }
    }

    video.onloadedmetadata = () => {
      if (isAborted) {
        return
      }
      video.currentTime = seekTime
    }

    video.onseeked = () => {
      if (isAborted) {
        return
      }

      const canvas = canvasPool.get()
      const vWidth = video.videoWidth
      const vHeight = video.videoHeight
      const isLandscape = vWidth > vHeight
      canvas.width = isLandscape ? CANVAS_MAX_DIMENSION : CANVAS_MAX_DIMENSION * (vWidth / vHeight)
      canvas.height = isLandscape ? CANVAS_MAX_DIMENSION * (vHeight / vWidth) : CANVAS_MAX_DIMENSION

      const ctx = canvas.getContext('2d')
      ctx.drawImage(video, 0, 0, canvas.width, canvas.height)
      canvas.release()

      const thumbnail = new CanvasTexture(canvas)
      thumbnail.colorSpace = 'srgb'
      thumbnail.userData.videoSrc = {url: extractResourceUrl(url), localUrl: videoUrl}

      setTexture(thumbnail)
      cleanup()
    }

    return () => {
      isAborted = true

      cleanup()

      if (texture) {
        texture.image = null
        texture.dispose()
      }
      setTexture(null)
    }
  }, [videoUrl, seekTime])

  return texture
}

export {useVideoThumbnail}
