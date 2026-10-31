import {h} from 'preact'
import type {FunctionComponent as FC, TargetedEvent} from 'preact'

import styles from '../styles.module.scss'

interface IMediaImage {
  src: string
  alt: string
  onSizeChange: (size: {width: number, height: number}) => void
  onError: () => void
}

const MediaImage: FC<IMediaImage> = ({src, alt, onSizeChange, onError}) => {
  // TODO(christoph): Handle loading errors
  const handleLoad = (e: TargetedEvent<HTMLImageElement>) => {
    onSizeChange({width: e.currentTarget.naturalWidth, height: e.currentTarget.naturalHeight})
  }

  return (
    <img
      className={styles.mediaImage}
      src={src}
      alt={alt}
      onLoad={handleLoad}
      onError={onError}
    />
  )
}

export {
  MediaImage,
}
