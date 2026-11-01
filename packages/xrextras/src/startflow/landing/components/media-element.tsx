import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {LandingMediaControlMode as MediaControlMode} from '../../types'

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
  videoTitle: string
  playLabel: string
  pauseLabel: string
  muteLabel: string
  unmuteLabel: string
}

const MediaElement: FC<IMediaElement> = ({
  src, alt, onSizeChange, autoplay, onError, mediaControls, videoTitle,
  playLabel, pauseLabel, muteLabel, unmuteLabel,
}) => {
  if (src.startsWith(YOUTUBE_PREFIX)) {
    const videoId = src.substr(YOUTUBE_PREFIX.length)
    return (
      <YoutubeMediaEmbed
        videoId={videoId}
        autoplay={autoplay}
        onSizeChange={onSizeChange}
        title={videoTitle}
      />
    )
  } else if (src.endsWith('.mp4')) {
    return (
      <MediaVideo
        src={src}
        autoplay={autoplay}
        onSizeChange={onSizeChange}
        onError={onError}
        mediaControls={mediaControls}
        playLabel={playLabel}
        pauseLabel={pauseLabel}
        muteLabel={muteLabel}
        unmuteLabel={unmuteLabel}
      />
    )
  } else {
    return <MediaImage src={src} alt={alt} onSizeChange={onSizeChange} onError={onError} />
  }
}
export {
  MediaElement,
}
