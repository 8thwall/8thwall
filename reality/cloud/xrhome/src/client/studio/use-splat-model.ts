import React from 'react'

import {loadScript} from '@ecs/shared/load-script'
import {getResourceBase} from '@ecs/shared/resources'
import type {IModel} from '@ecs/shared/splat-model'

import {useAbandonableEffect} from '../hooks/abandonable-effect'

let loadScriptPromise: Promise<void> | null = null
let finishedLoading = false
let Model: IModel

const useSplatModelLoaded = () => {
  const [modelLoaded, setModelLoaded] = React.useState(finishedLoading)

  useAbandonableEffect(async (executor) => {
    if (finishedLoading) {
      return
    }

    if (!loadScriptPromise) {
      loadScriptPromise = loadScript(`${getResourceBase()}splat/splat-loader.js`)
      loadScriptPromise.then(() => {
        Model = (window as any).Model
        Model.setInternalConfig({workerUrl: `${getResourceBase()}splat/splat-worker.js`})
        finishedLoading = true
      })
    }

    await executor(loadScriptPromise)
    setModelLoaded(true)
  }, [])

  return modelLoaded && Model
}

export {
  useSplatModelLoaded,
}
