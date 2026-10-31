import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useRef, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'
import styles from '../styles.module.scss'

import {LogoSection} from './logo-section'

import {MediaElement} from './media-element'
import {UrlPromptSection} from './url-prompt-section'
import {ViewContainer} from './view-container'

interface IMediaView extends LandingParameters {
  onError: () => void
}

const MediaView: FC<IMediaView> = (props) => {
  const [needsTransition, setNeedsTransition] = useState(false)
  const [mediaDimensions, setMediaDimensions] = useState<{width: number, height: number}>(null)
  const timeoutRef = useRef(null)

  const loadStartRef = useRef<number>(0)

  useEffect(() => {
    loadStartRef.current = Date.now()

    return () => {
      setMediaDimensions(null)
    }
  }, [props.mediaSrc])

  const handleSizeChange = (size: {width: number, height: number}) => {
    setNeedsTransition(Date.now() - loadStartRef.current > 1000)
    setMediaDimensions(size)
  }

  useEffect(() => () => {
    clearTimeout(timeoutRef.current)
  })

  const isLoaded = !!mediaDimensions
  const isPortrait = !!(mediaDimensions && mediaDimensions.width < mediaDimensions.height)

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
          src={props.mediaSrc}
          alt={props.mediaAlt}
          autoplay={props.mediaAutoplay}
          onSizeChange={handleSizeChange}
          onError={props.onError}
          mediaControls={props.mediaControls}
          videoTitle={props.videoTitle}
          playLabel={props.playLabel}
          pauseLabel={props.pauseLabel}
          muteLabel={props.muteLabel}
          unmuteLabel={props.unmuteLabel}
        />
        <UrlPromptSection
          url={props.url}
          promptPrefix={props.promptPrefix}
          promptSuffix={props.promptSuffix}
          vrPromptPrefix={props.vrPromptPrefix}
          qrAlt={props.qrAlt}
          messengerLabel={props.messengerLabel}
        />
        <LogoSection {...props} />
      </div>
    </ViewContainer>
  )
}

export {
  MediaView,
}
