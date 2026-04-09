import * as React from 'preact'
import {useEffect} from 'preact/hooks'
import type {FunctionComponent as FC} from 'preact'

interface IYoutubeMediaEmbed {
  videoId: string
  autoplay: boolean
  onSizeChange: (size: {width: number, height: number}) => void
}

const EMBED_PREFIX = 'https://www.youtube.com/embed/'

const YoutubeMediaEmbed: FC<IYoutubeMediaEmbed> = ({videoId: id, onSizeChange, autoplay}) => {
  useEffect(() => {
    onSizeChange({width: 1600, height: 900})
  }, [id])

  const maybeAutoPlay = autoplay ? '&autoplay=1&mute=1' : ''

  const src = `${EMBED_PREFIX}${id}?modestbranding=1&loop=1&playlist=${id}${maybeAutoPlay}`
  return (
    <div className='landing8-media-embed'>
      <iframe
        src={src}
        width='160'
        height='90'
        title='Video Player'
        frameBorder='0'
        allow='autoplay; fullscreen; picture-in-picture'
        allowFullScreen
      />
    </div>
  )
}

export {
  YoutubeMediaEmbed,
}
