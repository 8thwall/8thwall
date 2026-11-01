/* eslint-disable react/react-in-jsx-scope */
import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'
import {useRef, useEffect} from 'preact/hooks'

import type {StartFlowScreenOptions} from '../../types'
import {createScene} from '../3d/scene'
import {defaultMaps} from '../3d/env-maps'
import styles from '../styles.module.scss'

interface IModelScene {
  options: StartFlowScreenOptions
  onError: () => void
}

const ModelScene: FC<IModelScene> = ({options, onError}) => {
  const {
    mediaAnimation, sceneLightingIntensity, sceneOrbitIdle, sceneOrbitInteraction, sceneEnvMap,
  } = options
  const {mediaSrc} = options
  const sceneRef = useRef<ReturnType<typeof createScene>>()
  const canvasRef = useRef<HTMLCanvasElement>()

  useEffect(() => {
    sceneRef.current = createScene(canvasRef.current)
    return () => {
      sceneRef.current?.destroy()
    }
  }, [])

  useEffect(() => {
    sceneRef.current?.setErrorHandler(onError)
  }, [onError])

  useEffect(() => {
    sceneRef.current?.setModelSrc(mediaSrc)
  }, [mediaSrc])

  useEffect(() => {
    sceneRef.current?.setMediaAnimation(mediaAnimation)
  }, [mediaAnimation])

  useEffect(() => {
    sceneRef.current?.setLightingIntensity(sceneLightingIntensity)
  }, [sceneLightingIntensity])

  useEffect(() => {
    sceneRef.current?.setOrbitIdle(sceneOrbitIdle)
  }, [sceneOrbitIdle])

  useEffect(() => {
    sceneRef.current?.setOrbitInteraction(sceneOrbitInteraction)
  }, [sceneOrbitInteraction])

  useEffect(() => {
    sceneRef.current?.setEnvMap(defaultMaps[sceneEnvMap] || sceneEnvMap)
  }, [sceneEnvMap])

  return (
    <canvas ref={canvasRef} className={styles.modelCanvas} />
  )
}

export {
  ModelScene,
}
