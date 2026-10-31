import {Fragment, h} from 'preact'
import type {FunctionComponent} from 'preact'
import {useEffect, useRef, useState} from 'preact/hooks'

import styles from '../styles.module.scss'
import type {StartFlowScreenOptions, StartFlowStrings} from '../../types'

interface MediaProps {
  onError: () => void
  onSize: (size: {width: number, height: number}) => void
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}

const PlayIcon = () => (
  <svg viewBox='0 0 32 32' aria-hidden='true'>
    <circle cx='16' cy='16' r='15' fill='rgba(0,0,0,.55)' />
    <path d='m13 10 10 6-10 6Z' fill='white' />
  </svg>
)

const SoundIcon = ({muted}: {muted: boolean}) => (
  <svg viewBox='0 0 32 32' aria-hidden='true'>
    <path d='M5 13h5l6-5v16l-6-5H5Z' fill='white' />
    {muted
      ? <path d='m21 12 7 8m0-8-7 8' stroke='white' strokeWidth='2' />
      : (
        <path
          d='M21 11c4 3 4 7 0 10m3-13c7 5 7 11 0 16'
          fill='none'
          stroke='white'
          strokeWidth='2'
        />
      )}
  </svg>
)

const YoutubeMedia: FunctionComponent<MediaProps & {id: string}> = ({
  id, options, strings, onSize,
}) => {
  useEffect(() => onSize({width: 1600, height: 900}), [id])
  const autoplay = options.mediaAutoplay
  return (
    <div className={styles.mediaEmbed}><iframe
      title={strings.landingVideoTitle}
      src={`https://www.youtube.com/embed/${id}?modestbranding=1&loop=1&playlist=${id}${autoplay
        ? '&autoplay=1&mute=1'
        : ''}`}
      allow='autoplay; fullscreen; picture-in-picture'
      allowFullScreen
    />
    </div>
  )
}

const LandingMedia: FunctionComponent<MediaProps> = (props) => {
  const {options, strings, onSize, onError} = props
  const src = options.mediaSrc
  const autoplay = options.mediaAutoplay
  const controls = options.mediaControls
  const videoRef = useRef<HTMLVideoElement>()
  const [playing, setPlaying] = useState(false)
  const [muted, setMuted] = useState(autoplay)
  useEffect(() => setMuted(autoplay), [autoplay, src])
  const youtubePrefix = 'https://www.youtube.com/watch?v='
  if (!src) {
    return null
  }
  if (src.startsWith(youtubePrefix)) {
    return <YoutubeMedia {...props} id={src.slice(youtubePrefix.length)} />
  }
  if (/\.mp4(?:[?#]|$)/i.test(src)) {
    return (
      <div className={styles.videoContainer}>
        {controls === 'minimal' && (
          <Fragment>
            {!playing && (
              <button
                className={`${styles.mediaControl} ${styles.playButton}`}
                type='button'
                aria-label={strings.landingPlayLabel}
                onClick={() => videoRef.current?.play()}
              >
                <PlayIcon />
              </button>
            )}
            {playing && (
              <button
                className={`${styles.mediaControl} ${styles.pauseButton}`}
                type='button'
                aria-label={strings.landingPauseLabel}
                onClick={() => videoRef.current?.pause()}
              />
            )}
            <button
              className={`${styles.mediaControl} ${styles.muteButton}`}
              type='button'
              aria-label={muted
                ? strings.landingUnmuteLabel
                : strings.landingMuteLabel}
              onClick={() => setMuted(value => !value)}
            >
              <SoundIcon muted={muted} />
            </button>
          </Fragment>
        )}
        {/* User-provided preview media does not include a captions-track option. */}
        {/* eslint-disable-next-line jsx-a11y/media-has-caption */}
        <video
          ref={videoRef}
          className={styles.media}
          src={src}
          autoPlay={autoplay}
          muted={muted}
          loop
          controls={controls === 'browser'}
          playsInline
          onCanPlayThrough={(event) => {
            const el = event.currentTarget
            onSize({width: el.videoWidth, height: el.videoHeight})
            setPlaying(!el.paused)
          }}
          onPlay={() => setPlaying(true)}
          onPause={() => setPlaying(false)}
          onError={onError}
        />
      </div>
    )
  }
  return (
    <img
      className={styles.media}
      src={src}
      alt={strings.landingMediaAlt}
      onLoad={event => onSize({
        width: event.currentTarget.naturalWidth,
        height: event.currentTarget.naturalHeight,
      })}
      onError={onError}
    />
  )
}

export {LandingMedia}
