import {h} from 'preact'
import type {FunctionComponent} from 'preact'
import {useEffect, useRef, useState} from 'preact/hooks'

import type {StartFlowScreenOptions} from '../../types'
import {defaultMaps} from '../helpers/env-maps'
import {createScene} from '../helpers/scene'
import {loadThreeJs} from '../helpers/three'
import styles from '../styles.module.scss'

interface ModelProps {
  onError: () => void
  options: StartFlowScreenOptions
}

const ModelScene: FunctionComponent<ModelProps> = ({onError, options}) => {
  const canvasRef = useRef<HTMLCanvasElement>()
  const sceneRef = useRef<ReturnType<typeof createScene>>()
  useEffect(() => {
    sceneRef.current = createScene(canvasRef.current)
    return () => sceneRef.current?.destroy()
  }, [])
  useEffect(() => sceneRef.current?.setErrorHandler(onError), [onError])
  useEffect(() => sceneRef.current?.setModelSrc(options.mediaSrc ?? ''), [options.mediaSrc])
  useEffect(
    () => sceneRef.current?.setMediaAnimation(options.mediaAnimation),
    [options.mediaAnimation]
  )
  useEffect(
    () => sceneRef.current?.setLightingIntensity(options.sceneLightingIntensity),
    [options.sceneLightingIntensity]
  )
  useEffect(
    () => sceneRef.current?.setOrbitIdle(options.sceneOrbitIdle),
    [options.sceneOrbitIdle]
  )
  useEffect(
    () => sceneRef.current?.setOrbitInteraction(options.sceneOrbitInteraction),
    [options.sceneOrbitInteraction]
  )
  useEffect(() => {
    const envMap = options.sceneEnvMap
    sceneRef.current?.setEnvMap(defaultMaps[envMap] ?? envMap)
  }, [options.sceneEnvMap])
  return <canvas ref={canvasRef} className={styles.modelCanvas} />
}

const LandingModel: FunctionComponent<ModelProps> = ({onError, options}) => {
  const [ready, setReady] = useState(false)
  useEffect(() => {
    let active = true
    const load = async () => {
      try {
        await loadThreeJs()
        if (active) {
          setReady(true)
        }
      } catch {
        onError()
      }
    }
    load()
    return () => { active = false }
  }, [])
  return ready ? <ModelScene options={options} onError={onError} /> : null
}

export {LandingModel}
