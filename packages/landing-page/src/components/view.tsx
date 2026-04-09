import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useState, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'
import {MediaView} from './media-view'
import {BlankView} from './blank-view'
import {ModelView} from './model-view'

const View: FC<LandingParameters> = (props) => {
  const [didError, setDidError] = useState(false)

  useEffect(() => () => {
    setDidError(false)
  }, [props.mediaSrc])

  const handleLoadError = () => {
    // eslint-disable-next-line no-console
    console.error('[Landing Page] Failed to load content, falling back to blank view')
    setDidError(true)
  }

  if (!props.mediaSrc || didError) {
    return <BlankView {...props} />
  } else if (['.glb', '.gltf'].some(e => props.mediaSrc.endsWith(e))) {
    return <ModelView {...props} onError={handleLoadError} />
  } else {
    return <MediaView {...props} onError={handleLoadError} />
  }
}

export {
  View,
}
