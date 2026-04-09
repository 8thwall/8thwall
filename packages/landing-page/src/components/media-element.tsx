import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {MediaControlMode} from '../parameters'

import {MediaImage} from './media-image'
import {MediaVideo} from './media-video'
import {YoutubeMediaEmbed} from './youtube-media-embed'

const YOUTUBE_PREFIX = 'https://www.youtube.com/watch?v='

interface IMediaElement {
  src: string
  onSizeChange: (size: {width: number, height: number}) => void
  alt: string
  autoplay: boolean
  onError: () => void
  mediaControls: MediaControlMode
}

const MediaElement: FC<IMediaElement> = ({
  src, alt, onSizeChange, autoplay, onError, mediaControls,
}) => {
  if (src.startsWith(YOUTUBE_PREFIX)) {
    const videoId = src.substr(YOUTUBE_PREFIX.length)
    return (
      <YoutubeMediaEmbed videoId={videoId} autoplay={autoplay} onSizeChange={onSizeChange} />
    )
  } else if (src.endsWith('.mp4')) {
    return (
      <MediaVideo
        src={src}
        autoplay={autoplay}
        onSizeChange={onSizeChange}
        onError={onError}
        mediaControls={mediaControls}
      />
    )
  } else {
    return <MediaImage src={src} alt={alt} onSizeChange={onSizeChange} onError={onError} />
  }
}
export {
  MediaElement,
}
