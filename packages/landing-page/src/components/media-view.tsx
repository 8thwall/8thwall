import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useRef, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'

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

  const loadStartRef = useRef<number>(Date.now())

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
    'landing8-media-view',
    !isLoaded && 'landing8-view-loading',
    isLoaded && needsTransition && 'landing8-view-transition',
    isPortrait ? 'landing8-portrait-layout' : 'landing8-landscape-layout',
    props.textShadow && 'landing8-text-shadow',
  ].filter(Boolean).join(' ')

  const style = {
    'color': props.textColor,
    'fontFamily': props.font,
    '--landing8-media-width': mediaDimensions ? mediaDimensions.width : undefined,
    '--landing8-media-height': mediaDimensions ? mediaDimensions.height : undefined,
  }

  return (
    <ViewContainer
      backgroundColor={props.backgroundColor}
      backgroundSrc={props.backgroundSrc}
      backgroundBlur={props.backgroundBlur}
    >
      <div className={classes} style={style}>
        <MediaElement
          src={props.mediaSrc}
          alt={props.mediaAlt}
          autoplay={props.mediaAutoplay}
          onSizeChange={handleSizeChange}
          onError={props.onError}
          mediaControls={props.mediaControls}
        />
        <UrlPromptSection
          url={props.url}
          promptPrefix={props.promptPrefix}
          promptSuffix={props.promptSuffix}
          vrPromptPrefix={props.vrPromptPrefix}
        />
        <LogoSection logoSrc={props.logoSrc} logoAlt={props.logoAlt} />
      </div>
    </ViewContainer>
  )
}

export {
  MediaView,
}
