import React from 'react'

import {loadScript} from '@ecs/shared/load-script'
import type {IModel} from '@ecs/shared/splat-model'

import {SPLAT_MODEL_URL, SPLAT_WORKER_URL} from '../../shared/studio/cdn-assets'
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
      loadScriptPromise = loadScript(SPLAT_MODEL_URL)
      loadScriptPromise.then(() => {
        Model = (window as any).Model
        Model.setInternalConfig({workerUrl: SPLAT_WORKER_URL})
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
