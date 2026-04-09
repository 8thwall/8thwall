import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

interface IMediaImage {
  src: string
  alt: string
  onSizeChange: (size: {width: number, height: number}) => void
  onError: () => void
}

const MediaImage: FC<IMediaImage> = ({src, alt, onSizeChange, onError}) => {
  // TODO(christoph): Handle loading errors
  const handleLoad = (e) => {
    onSizeChange({width: e.target.naturalWidth, height: e.target.naturalHeight})
  }

  return (
    <img
      className='landing8-media-image'
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
