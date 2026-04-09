/* eslint-disable react/jsx-fragments */
import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useRef, useState} from 'preact/hooks'

import {useElementSize} from '../hooks/element-size'

import type {MediaControlMode} from '../parameters'

const PLAY_SRC = './external/landing-page/resources/img/mediarecorder/play-v1.svg'
const SOUND_ON_SRC = './external/xrextras-shared-resources/img/mediarecorder/sound-on-v1.svg'
const SOUND_OFF_SRC = './external/xrextras-shared-resources/img/mediarecorder/sound-off-v1.svg'

interface IMediaVideo {
  src: string
  autoplay: boolean
  mediaControls: MediaControlMode
  onSizeChange: (size: {width: number, height: number}) => void
  onError: () => void
}

const MediaVideo: FC<IMediaVideo> = ({src, onSizeChange, autoplay, mediaControls, onError}) => {
  const containerRef = useRef<HTMLDivElement>()
  const videoRef = useRef<HTMLVideoElement>()
  const [isMuted, setIsMuted] = useState(autoplay)
  const [isPlaying, setIsPlaying] = useState(false)
  const containerSize = useElementSize(containerRef)
  const videoSize = useElementSize(videoRef)

  const handleLoad = (e) => {
    const newSize = {width: e.target.videoWidth, height: e.target.videoHeight}
    onSizeChange(newSize)
    setIsMuted(videoRef.current.muted)
    setIsPlaying(!videoRef.current.paused)
  }

  const sizeVariables = (containerSize && videoSize)
    ? {
      '--landing8-video-center-x': `${containerSize.width / 2}px`,
      '--landing8-video-center-y': `${videoSize.height / 2}px`,
      '--landing8-video-right': `${containerSize.width / 2 - videoSize.width / 2}px`,
    }
    : undefined

  return (
    <div ref={containerRef} className='landing8-media-video-container' style={sizeVariables}>
      {mediaControls === 'minimal' &&
        <React.Fragment>
          {isPlaying

            ? (
              <button
                type='button'
                onClick={() => videoRef.current.pause()}
                className='landing8-media-control-button landing8-media-pause-button'
              >Pause
              </button>
            )
            : (
              <button
                type='button'
                onClick={() => videoRef.current.play()}
                className='landing8-media-control-button landing8-media-play-button landing8-shadow'
              >
                <img alt='Play' src={PLAY_SRC} className='landing8-media-control-icon' />
              </button>
            )
          }
          <button
            type='button'
            onClick={() => setIsMuted(s => !s)}
            className={`landing8-media-control-button landing8-media-mute-button \
landing8-shadow ${(!isMuted && isPlaying) ? 'landing8-playing-with-sound' : ''}`}
          >
            {isMuted
              ? <img alt='Unmute' src={SOUND_OFF_SRC} className='landing8-media-control-icon' />
              : <img alt='Mute' src={SOUND_ON_SRC} className='landing8-media-control-icon' />
            }
          </button>
        </React.Fragment>
      }
      {/* eslint-disable-next-line jsx-a11y/media-has-caption */}
      <video
        ref={videoRef}
        className='landing8-media-video'
        src={src}
        autoPlay={autoplay}
        muted={isMuted}
        loop
        controls={mediaControls === 'browser'}
        onCanPlayThrough={handleLoad}
        onError={onError}
        onPlay={() => setIsPlaying(true)}
        onPause={() => setIsPlaying(false)}
      />
    </div>
  )
}

export {
  MediaVideo,
}
