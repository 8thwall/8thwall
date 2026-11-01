import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useEffect} from 'preact/hooks'

import type {StartFlowContentProps} from '../../types'
import {MediaView} from './media-view'
import {BlankView} from './blank-view'
import {ModelView} from './model-view'

const View: FC<StartFlowContentProps> = ({options, strings}) => {
  const [didError, setDidError] = useState(false)

  useEffect(() => {
    setDidError(false)
  }, [options.mediaSrc])

  const handleLoadError = () => {
    // eslint-disable-next-line no-console
    console.error('[Landing Page] Failed to load content, falling back to blank view')
    setDidError(true)
  }

  if (!options.mediaSrc || didError) {
    return <BlankView options={options} strings={strings} />
  } else if (/\.(glb|gltf)(?:[?#]|$)/i.test(options.mediaSrc)) {
    return <ModelView options={options} strings={strings} onError={handleLoadError} />
  } else {
    return <MediaView options={options} strings={strings} onError={handleLoadError} />
  }
}

export {
  View,
}
