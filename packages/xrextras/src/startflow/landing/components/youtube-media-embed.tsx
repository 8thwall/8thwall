import {h} from 'preact'
import {useEffect} from 'preact/hooks'
import type {FunctionComponent as FC} from 'preact'

import styles from '../styles.module.scss'

interface IYoutubeMediaEmbed {
  videoId: string
  autoplay: boolean
  onSizeChange: (size: {width: number, height: number}) => void
  title: string
}

const EMBED_PREFIX = 'https://www.youtube.com/embed/'

const YoutubeMediaEmbed: FC<IYoutubeMediaEmbed> = ({
  videoId: id, onSizeChange, autoplay, title,
}) => {
  useEffect(() => {
    onSizeChange({width: 1600, height: 900})
  }, [id])

  const maybeAutoPlay = autoplay ? '&autoplay=1&mute=1' : ''

  const src = `${EMBED_PREFIX}${id}?modestbranding=1&loop=1&playlist=${id}${maybeAutoPlay}`
  return (
    <div className={styles.mediaEmbed}>
      <iframe
        src={src}
        width='160'
        height='90'
        title={title}
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
