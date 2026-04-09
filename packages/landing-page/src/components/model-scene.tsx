/* eslint-disable react/react-in-jsx-scope */
import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useRef, useEffect} from 'preact/hooks'

import type {LandingParameters} from '../parameters'
import {createScene} from '../3d/scene'
import {defaultMaps} from '../3d/env-maps'

type SceneParameters = Pick<LandingParameters,
  'mediaSrc' |
  'mediaAnimation' |
  'sceneEnvMap' |
  'sceneOrbitIdle'|
  'sceneOrbitInteraction'|
  'sceneLightingIntensity'
>

interface IModelScene extends SceneParameters {
  onError: () => void
}

const ModelScene: FC<IModelScene> = ({
  mediaSrc, mediaAnimation, sceneLightingIntensity, sceneOrbitIdle, sceneOrbitInteraction,
  sceneEnvMap, onError,
}) => {
  const sceneRef = useRef<ReturnType<typeof createScene>>()
  const canvasRef = useRef<HTMLCanvasElement>()

  useEffect(() => {
    sceneRef.current = createScene(canvasRef.current)
    return () => {
      sceneRef.current.destroy()
    }
  }, [])

  useEffect(() => {
    sceneRef.current.setErrorHandler(onError)
  }, [onError])

  useEffect(() => {
    sceneRef.current.setModelSrc(mediaSrc)
  }, [mediaSrc])

  useEffect(() => {
    sceneRef.current.setMediaAnimation(mediaAnimation)
  }, [mediaAnimation])

  useEffect(() => {
    sceneRef.current.setLightingIntensity(sceneLightingIntensity)
  }, [sceneLightingIntensity])

  useEffect(() => {
    sceneRef.current.setOrbitIdle(sceneOrbitIdle)
  }, [sceneOrbitIdle])

  useEffect(() => {
    sceneRef.current.setOrbitInteraction(sceneOrbitInteraction)
  }, [sceneOrbitInteraction])

  useEffect(() => {
    sceneRef.current.setEnvMap(defaultMaps[sceneEnvMap] || sceneEnvMap)
  }, [sceneEnvMap])

  return (
    <canvas ref={canvasRef} className='landing8-model-canvas' />
  )
}

export {
  ModelScene,
}
