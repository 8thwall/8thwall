/* eslint-disable react/jsx-fragments */
import {Fragment, h} from 'preact'
import type {FunctionComponent as FC, TargetedEvent} from 'preact'
import {useEffect, useRef, useState} from 'preact/hooks'

import {useElementSize} from '../hooks/element-size'

import type {LandingMediaControlMode as MediaControlMode} from '../../types'
import styles from '../styles.module.scss'

import PLAY_SRC from '../resources/play.svg'
import SOUND_ON_SRC from '../resources/sound-on.svg'
import SOUND_OFF_SRC from '../resources/sound-off.svg'

interface IMediaVideo {
  src: string
  autoplay: boolean
  mediaControls: MediaControlMode
  onSizeChange: (size: {width: number, height: number}) => void
  onError: () => void
  playLabel: string
  pauseLabel: string
  muteLabel: string
  unmuteLabel: string
}

const MediaVideo: FC<IMediaVideo> = ({
  src, onSizeChange, autoplay, mediaControls, onError,
  playLabel, pauseLabel, muteLabel, unmuteLabel,
}) => {
  const containerRef = useRef<HTMLDivElement>()
  const videoRef = useRef<HTMLVideoElement>()
  const [isMuted, setIsMuted] = useState(autoplay)
  const [isPlaying, setIsPlaying] = useState(false)
  useEffect(() => setIsMuted(autoplay), [autoplay, src])
  const containerSize = useElementSize(containerRef)
  const videoSize = useElementSize(videoRef)

  const handleLoad = (e: TargetedEvent<HTMLVideoElement>) => {
    const newSize = {width: e.currentTarget.videoWidth, height: e.currentTarget.videoHeight}
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
    <div
      ref={containerRef}
      className={styles.mediaVideoContainer}
      style={sizeVariables}
    >
      {mediaControls === 'minimal' &&
        <Fragment>
          {isPlaying

            ? (
              <button
                type='button'
                aria-label={pauseLabel}
                onClick={() => videoRef.current.pause()}
                className={`${styles.mediaControlButton} ${
                  styles.mediaPauseButton
                }`}
              />
            )
            : (
              <button
                type='button'
                aria-label={playLabel}
                onClick={() => videoRef.current.play()}
                className={`${styles.mediaControlButton} ${
                  styles.mediaPlayButton
                } ${styles.shadow}`}
              >
                <img alt='' src={PLAY_SRC} className={styles.mediaControlIcon} />
              </button>
            )
          }
          <button
            type='button'
            aria-label={isMuted ? unmuteLabel : muteLabel}
            onClick={() => setIsMuted(s => !s)}
            className={`${styles.mediaControlButton} ${
              styles.mediaMuteButton
            } ${styles.shadow} ${
              (!isMuted && isPlaying) ? styles.playingWithSound : ''
            }`}
          >
            {isMuted
              ? <img alt='' src={SOUND_OFF_SRC} className={styles.mediaControlIcon} />
              : <img alt='' src={SOUND_ON_SRC} className={styles.mediaControlIcon} />
            }
          </button>
        </Fragment>
      }
      {/* eslint-disable-next-line jsx-a11y/media-has-caption */}
      <video
        ref={videoRef}
        className={styles.mediaVideo}
        src={src}
        autoPlay={autoplay}
        muted={isMuted}
        loop
        controls={mediaControls === 'browser'}
        playsInline
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
