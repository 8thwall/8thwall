import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useRef, useEffect} from 'preact/hooks'

import type {StartFlowContentProps} from '../../types'
import styles from '../styles.module.scss'

import {LogoSection} from './logo-section'

import {MediaElement} from './media-element'
import {UrlPromptSection} from './url-prompt-section'
import {ViewContainer} from './view-container'

interface IMediaView extends StartFlowContentProps {
  onError: () => void
}

const MediaView: FC<IMediaView> = ({options, strings, onError}) => {
  const [needsTransition, setNeedsTransition] = useState(false)
  const [mediaDimensions, setMediaDimensions] = useState<{width: number, height: number}>(null)
  const timeoutRef = useRef(null)

  const loadStartRef = useRef<number>(0)

  useEffect(() => {
    loadStartRef.current = Date.now()

    return () => {
      setMediaDimensions(null)
    }
  }, [options.mediaSrc])

  const handleSizeChange = (size: {width: number, height: number}) => {
    setNeedsTransition(Date.now() - loadStartRef.current > 1000)
    setMediaDimensions(size)
  }

  useEffect(() => () => {
    clearTimeout(timeoutRef.current)
  })

  const isLoaded = !!mediaDimensions
  const isPortrait = !!(mediaDimensions && mediaDimensions.width < mediaDimensions.height)
  const {mediaSrc} = options

  const classes = [
    styles.mediaView,
    !isLoaded && styles.viewLoading,
    isLoaded && needsTransition && styles.viewTransition,
    isPortrait ? styles.portraitLayout : styles.landscapeLayout,
  ].filter(Boolean).join(' ')

  const style = {
    '--landing8-media-width': mediaDimensions ? mediaDimensions.width : undefined,
    '--landing8-media-height': mediaDimensions ? mediaDimensions.height : undefined,
  }

  return (
    <ViewContainer>
      <div className={classes} style={style}>
        <MediaElement
          src={mediaSrc}
          alt={strings.landingMediaAlt}
          autoplay={options.mediaAutoplay}
          onSizeChange={handleSizeChange}
          onError={onError}
          mediaControls={options.mediaControls}
          videoTitle={strings.landingVideoTitle}
          playLabel={strings.landingPlayLabel}
          pauseLabel={strings.landingPauseLabel}
          muteLabel={strings.landingMuteLabel}
          unmuteLabel={strings.landingUnmuteLabel}
        />
        <UrlPromptSection options={options} strings={strings} />
        <LogoSection options={options} strings={strings} />
      </div>
    </ViewContainer>
  )
}

export {
  MediaView,
}
