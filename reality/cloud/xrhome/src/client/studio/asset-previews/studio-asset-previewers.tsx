import React from 'react'

import {useTranslation} from 'react-i18next'

import type {AdditionalAssetData} from '../../editor/asset-preview-types'

import {IconButton} from '../../ui/components/icon-button'

import {
  VIDEO_FILES,
  AUDIO_FILES,
} from '../../common/editor-files'
import {fileExt} from '../../editor/editor-common'
import {useAppPreviewStyles} from '../../editor/app-preview/app-preview-utils'
import {createThemedStyles} from '../../ui/theme'
import {loadHdr} from '../../../third_party/rgbe-loader/load-hdr'
import {StaticBanner} from '../../ui/components/banner'

const useStyles = createThemedStyles(theme => ({
  progressBar: {
    flex: '1',
    width: '80%',
    height: '0.5rem',
    background: theme.subtleBorder,
    margin: '0 0.5rem',
    position: 'relative',
    borderRadius: '0.5rem', /* Adjust the value to get the desired curve */
    overflow: 'hidden',
    cursor: 'pointer',
  },
  progress: {
    height: '100%',
    background: theme.loaderSecondary,
    position: 'absolute',
    top: '0',
    left: '0',
    borderRadius: '0.5rem',
  },
  controls: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    marginTop: '0.5rem',
    gap: '0.25rem',
  },
  letterbox: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    minWidth: '10rem',
    minHeight: '5rem',
    padding: '0.1rem',
    boxSizing: 'border-box',
    backgroundColor: theme.studioAssetBorder,
    border: `0.25rem solid ${theme.studioAssetBorder}`,
    borderRadius: '0.5rem',
  },
  timeDisplay: {
    marginLeft: '0.5rem',
    color: theme.fgMuted,
  },
}))

const formatTime = (time: number) => {
  const minutes = Math.floor(time / 60)
  const seconds = Math.floor(time % 60)
  return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`
}

interface IPlayableMediaPreview {
  src: string
  onMoreData:
  (data: Pick<AdditionalAssetData, 'duration'> | Pick<AdditionalAssetData, 'dimension'>) => void
  onAssetLoad: () => void
  audioState?: {audioCheckDone: boolean, hasAudio: boolean}
  setAudioState?: (value: {audioCheckDone: boolean, hasAudio: boolean}) => void
}

const PlayableMediaPreview: React.FunctionComponent<IPlayableMediaPreview> = ({
  src,
  onMoreData,
  onAssetLoad,
  audioState,
  setAudioState,
}) => {
  const mediaRef = React.useRef(null)
  const progressBarRef = React.useRef(null)
  const [isPlaying, setIsPlaying] = React.useState<boolean>(false)
  const shouldResumeAfterScrub = React.useRef(false)
  const [isMuted, setIsMuted] = React.useState<boolean>(false)
  const shouldMuteAfterScrub = React.useRef(false)
  const [progress, setProgress] = React.useState(0)
  const [currentTime, setCurrentTime] = React.useState('00:00')
  const [duration, setDuration] = React.useState('00:00')
  const isDragging = React.useRef(false)
  const appPreviewStyles = useAppPreviewStyles()

  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const ext = src && fileExt(src)

  const handleFullscreenChange = () => {
    // The document has exited fullscreen mode
    if (!document.fullscreenElement) {
      if (mediaRef.current) {
        setIsPlaying(!mediaRef.current.paused)
        setIsMuted(mediaRef.current.muted)
        mediaRef.current.removeEventListener('fullscreenchange', handleFullscreenChange)
      }
    }
  }

  const handleFullScreen = () => {
    if (mediaRef.current) {
      if (!document.fullscreenElement) {
        mediaRef.current.requestFullscreen()
          .then(() => {
            mediaRef.current.addEventListener('fullscreenchange', handleFullscreenChange)
          })
      } else {
        document.exitFullscreen()
      }
    }
  }

  const togglePlayPause = (shouldPlay: boolean) => {
    if (mediaRef.current) {
      if (shouldPlay) {
        mediaRef.current.play()
      } else {
        mediaRef.current.pause()
      }
      setIsPlaying(shouldPlay)
    }
  }

  const toggleMute = (shouldMute: boolean) => {
    if (mediaRef.current) {
      mediaRef.current.muted = shouldMute
      setIsMuted(shouldMute)
    }
  }

  const updateProgressAndTime = (time: number) => {
    if (mediaRef.current) {
      const currentProgress = (time / mediaRef.current.duration) * 100
      setProgress(currentProgress)
      setCurrentTime(formatTime(time))
    }
  }
  // Used to resynchronize the progress bar with the time one more time
  const updateProgressBar = () => {
    if (mediaRef.current) {
      updateProgressAndTime(mediaRef.current.currentTime)
    }
    if (isPlaying) {
      requestAnimationFrame(updateProgressBar)
    }
  }

  const handleProgressChange = (e) => {
    if (progressBarRef.current && mediaRef.current) {
      const rect = progressBarRef.current.getBoundingClientRect()
      const offsetX = e.clientX - rect.left
      const newTime = (offsetX / rect.width) * mediaRef.current.duration
      mediaRef.current.currentTime = newTime
      updateProgressAndTime(newTime)
    }
  }

  const handleMouseMove = (e) => {
    if (isDragging.current) {
      handleProgressChange(e)
    }
  }

  const handleMouseUp = () => {
    isDragging.current = false
    togglePlayPause(shouldResumeAfterScrub.current)
    toggleMute(shouldMuteAfterScrub.current)
    document.removeEventListener('mousemove', handleMouseMove)
    document.removeEventListener('mouseup', handleMouseUp)
  }

  const handleMouseDown = (e) => {
    isDragging.current = true
    shouldResumeAfterScrub.current = isPlaying
    shouldMuteAfterScrub.current = isMuted
    setIsPlaying(false)
    mediaRef.current.pause()
    mediaRef.current.muted = true
    handleProgressChange(e)

    document.addEventListener('mousemove', handleMouseMove)
    document.addEventListener('mouseup', handleMouseUp)
  }

  const handleMediaEnd = () => {
    setIsPlaying(false)
    setProgress(100)
    setCurrentTime(formatTime(mediaRef.current.duration))
  }

  const isVideo = VIDEO_FILES.includes(ext)

  let mediaElement

  if (VIDEO_FILES.includes(ext)) {
    const onLoadedMetadata = (e) => {
      onMoreData({
        dimension: {
          width: e.target.videoWidth,
          height: e.target.videoHeight,
        },
        duration: e.target.duration,
      })
      setDuration(formatTime(mediaRef.current.duration))

      // put hasAudio check in a timeout to ensure webkitAudioDecodedByteCount is decoded
      setTimeout(() => {
        const video = e.target
        const hasAudio = video.mozHasAudio || video.webkitAudioDecodedByteCount > 0 ||
          video.audioTracks?.length > 0
        setAudioState({audioCheckDone: true, hasAudio})
      }, 50)
    }

    mediaElement = (
      <div className={classes.letterbox}>
        <video
          ref={mediaRef}
          onTimeUpdate={updateProgressBar}
          onEnded={handleMediaEnd}
          controls={false}
          src={src}
          onLoadedMetadata={onLoadedMetadata}
          onLoadedData={onAssetLoad}
        >
          <track kind='captions' src='' srcLang='en' default />
        </video>
      </div>
    )
  } else if (AUDIO_FILES.includes(ext)) {
    const onLoadedMetadata = (e) => {
      onMoreData({
        duration: e.target.duration,
      })
      setDuration(formatTime(mediaRef.current.duration))
    }

    mediaElement = (
      <audio
        ref={mediaRef}
        onTimeUpdate={updateProgressBar}
        onEnded={handleMediaEnd}
        controls={false}
        src={src}
        onLoadedMetadata={onLoadedMetadata}
        onLoadedData={onAssetLoad}
      >
        <track kind='captions' src='' srcLang='en' default />
      </audio>
    )
  } else {
    return <p> {t('asset_configurator.asset_previewers.not_supported')} </p>
  }

  return (
    <>
      {mediaElement}
      <div className={classes.controls}>
        <div className={appPreviewStyles.actionButton}>
          <IconButton
            stroke={isPlaying ? 'pause' : 'play'}
            onClick={() => { togglePlayPause(!isPlaying) }}
            text={isPlaying
              ? t('asset_configurator.studio_asset_preview.playable_media.pause_button')
              : t('asset_configurator.studio_asset_preview.playable_media.play_button')
            }
          />
        </div>
        <div
          className={classes.progressBar}
          ref={progressBarRef}
          onMouseDown={handleMouseDown}
          role='slider'
          tabIndex={0}
          aria-valuenow={progress}
        >
          <div className={classes.progress} style={{width: `${progress}%`}} />
        </div>
        <div className={classes.timeDisplay}>
          {currentTime} / {duration}
        </div>
        {isVideo &&
          <>
            {audioState.hasAudio &&
              <div className={appPreviewStyles.actionButton}>
                <IconButton
                  stroke={isMuted ? 'audioMuted' : 'audioFilled'}
                  onClick={() => { toggleMute(!isMuted) }}
                  text={isMuted
                    ? t('asset_configurator.studio_asset_preview.video_sound.unmute_button')
                    : t('asset_configurator.studio_asset_preview.video_sound.mute_button')}
                />
              </div>
            }
            <div className={appPreviewStyles.actionButton}>
              <IconButton
                stroke='fullscreen'
                onClick={handleFullScreen}
                text={t('asset_configurator.studio_asset_preview.video.fullscreen_button')}
              />
            </div>
          </>
        }
      </div>
    </>
  )
}

interface IImagePreview {
  src: string
  alt: string
  onMoreData: (data: Pick<AdditionalAssetData, 'dimension'>) => void
  onAssetLoad: () => void
}

const ImagePreview: React.FunctionComponent<IImagePreview> = ({
  src, alt, onMoreData, onAssetLoad,
}) => {
  const classes = useStyles()
  const onLoad = (e) => {
    onMoreData({
      dimension: {
        width: e.target.naturalWidth,
        height: e.target.naturalHeight,
      },
    })
    onAssetLoad()
  }

  return (
    <div className={classes.letterbox}>
      <img src={src} onLoad={onLoad} alt={alt} />
    </div>
  )
}

interface IVideoPreview {
  src: string
  onMoreData: (data: Pick<AdditionalAssetData, 'dimension' |'duration'>) => void
  onAssetLoad: () => void
  audioState: {audioCheckDone: boolean, hasAudio: boolean}
  setAudioState: (value: {audioCheckDone: boolean, hasAudio: boolean}) => void
}

const VideoPreview: React.FunctionComponent<IVideoPreview> = ({
  src, onMoreData, onAssetLoad, audioState, setAudioState,
}) => (
  <PlayableMediaPreview
    src={src}
    onMoreData={onMoreData}
    onAssetLoad={onAssetLoad}
    audioState={audioState}
    setAudioState={setAudioState}
  />
)

interface IAudioPreview {
  src: string
  onMoreData: (data: Pick<AdditionalAssetData, 'duration'>) => void
  onAssetLoad: () => void
}

const AudioPreview: React.FunctionComponent<IAudioPreview> = ({src, onMoreData, onAssetLoad}) => (
  <PlayableMediaPreview src={src} onMoreData={onMoreData} onAssetLoad={onAssetLoad} />
)

interface ImagePreviewData {
  width: number
  height: number
  image?: ImageData
}

const NO_DATA: ImagePreviewData = {width: 0, height: 0, image: null}
const isValidImageData = (data: ImagePreviewData) => data.width > 0 && data.height > 0 && data.image

// eslint-disable-next-line local-rules/hardcoded-copy
const INVALID_IMAGE_DATA_ERROR = 'Invalid image data'
// eslint-disable-next-line local-rules/hardcoded-copy
const UNSUPPORTED_IMAGE_TYPE_ERROR = 'Unsupported image type'

// Call different loaders for custom images to image data based on file extension.
const loadCustomImage = async (url: string): Promise<ImagePreviewData> => {
  const ext = fileExt(url)
  if (ext === 'hdr') {
    const data = await loadHdr(url)
    if (isValidImageData(data)) {
      return data
    } else {
      throw new Error(INVALID_IMAGE_DATA_ERROR)
    }
  } else {
    throw new Error(UNSUPPORTED_IMAGE_TYPE_ERROR)
  }
}

interface ICustomImagePreview {
  src: string
  onMoreData: (data: Pick<AdditionalAssetData, 'dimension'>) => void
  onAssetLoad?: () => void
}

const CustomImagePreview: React.FC<ICustomImagePreview> = ({src, onMoreData, onAssetLoad}) => {
  const [imageData, setImageData] = React.useState<ImagePreviewData>(NO_DATA)
  const [errorKey, setErrorKey] = React.useState('')
  const {t} = useTranslation(['cloud-studio-pages'])

  const classes = useStyles()

  const canvasRef = React.useCallback((node) => {
    if (node === null) {
      return
    }
    node.width = imageData.width
    node.height = imageData.height
    if (imageData.image) {
      const ctx = node.getContext('2d')
      ctx.putImageData(imageData.image, 0, 0)
    }
    if (imageData.width && imageData.height) {
      onMoreData({
        dimension: {
          width: imageData.width,
          height: imageData.height,
        },
      })
    }
    onAssetLoad?.()
  }, [imageData])

  React.useEffect(() => {
    const handleLoadCustomImage = async () => {
      try {
        const data = await loadCustomImage(src)
        setImageData(data)
      } catch (error) {
        switch (error.message) {
          case INVALID_IMAGE_DATA_ERROR:
            setErrorKey('asset_configurator.asset_previewers.special_image.invalid_data_error')
            break
          case UNSUPPORTED_IMAGE_TYPE_ERROR:
            setErrorKey('asset_configurator.asset_previewers.special_image.unsupported_type_error')
            break
          default:
            setErrorKey('asset_configurator.asset_previewers.special_image.load_error')
            break
        }
      }
    }
    handleLoadCustomImage()
  }, [src])

  return (
    errorKey
      ? (
        <StaticBanner
          type='danger'
          message={t(errorKey)}
        />)
      : (
        <div className={classes.letterbox}>
          <canvas ref={canvasRef} />
        </div>
      )
  )
}

export {
  ImagePreview,
  VideoPreview,
  AudioPreview,
  CustomImagePreview,
}
