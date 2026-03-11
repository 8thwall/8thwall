import React from 'react'
import {useTranslation} from 'react-i18next'
import {useThree} from '@react-three/fiber'
import GUI, {Controller} from 'lil-gui'
import {AnimationMixer, Clock, Object3D} from 'three'
import {ENV_MAP_PRESETS} from '@ecs/shared/environment-maps'

interface DebugLightConfig {
  ambientIntensity: number
  directionalIntensity: number
  ambientColor: string
  directionalColor: string
}

interface useGltfDebugGuiOptions {
  visible: boolean
  scene: Object3D
  lightConfig: DebugLightConfig
  onEnvMapChange?: (envName: string) => void
  onWireframeChange?: (wireframe: boolean) => void
  onLightConfigChange?: (config: DebugLightConfig) => void
}

const useGltfDebugGui = ({
  visible, scene, lightConfig, onEnvMapChange, onWireframeChange, onLightConfigChange,
}: useGltfDebugGuiOptions) => {
  const {t} = useTranslation(['cloud-studio-pages'])

  const three = useThree()
  const {domElement: canvas} = three.gl

  const debugMainGui = React.useRef<GUI>()
  const debugAnimationControllers = React.useRef<Controller[]>()
  const animationTime = React.useRef(0)
  const mixer = React.useMemo(() => (scene ? new AnimationMixer(scene) : null), [scene])

  const clock = new Clock()
  const getAnimationPlayingCount = () => (
    debugMainGui.current
      ? debugAnimationControllers.current.reduce((prev, cur) => prev + (cur.getValue() ? 1 : 0), 0)
      : 0
  )

  // NOTE (jeffha): We use requestAnimationFrame to create a smooth animation loop that
  // synchronizes all active animations selected by the user. This shared loop ensures consistent
  // timing across multiple animations while remaining active only when needed,
  // preventing unnecessary renders when no animations are playing.
  let cancelAnimation: number | null = null
  const animateFrames = () => {
    const count = getAnimationPlayingCount()
    if (count > 0) {
      cancelAnimation = requestAnimationFrame(animateFrames)

      const deltaTime = clock.getDelta()
      mixer.update(deltaTime)
      animationTime.current = deltaTime
    } else if (cancelAnimation !== null) {
      cancelAnimationFrame(cancelAnimation)
      cancelAnimation = null
    }
  }

  React.useEffect(() => {
    if (!debugMainGui.current) {
      return
    }

    debugMainGui.current.show(visible)
  }, [debugMainGui, visible])

  const modelObjData = {
    ...lightConfig,
    wireframe: false,
    envMap: 'none',
    playbackSpeed: 1,
    animations: {},
    playAll: () => debugAnimationControllers.current.forEach(c => c.setValue(true)),
  }

  const onLightChange = () => onLightConfigChange?.({
    ambientIntensity: modelObjData.ambientIntensity,
    ambientColor: modelObjData.ambientColor,
    directionalIntensity: modelObjData.directionalIntensity,
    directionalColor: modelObjData.directionalColor,
  })

  React.useEffect(() => {
    if (visible && canvas && scene) {
      // By default, collapse the GUI
      const gui = new GUI({container: canvas.parentElement}).close()
      gui.title(t('studio_model_preview.debug.title'))
      gui
        .add(modelObjData, 'wireframe')
        .name(t('studio_model_preview.debug.wireframe'))
        .onChange(onWireframeChange)
      debugMainGui.current = gui

      const lighting = gui.addFolder(t('studio_model_preview.debug.lighting')).close()
      lighting
        .add(modelObjData, 'envMap', Object.fromEntries(
          Object.entries(ENV_MAP_PRESETS).map(([key, value]) => [t(value), key])
        ))
        .name(t('studio_model_preview.debug.env_map'))
        .onChange(onEnvMapChange)

      lighting
        .add(modelObjData, 'ambientIntensity', 0, 2)
        .name(t('studio_model_preview.debug.ambient_intensity'))
        .onChange(onLightChange)

      lighting
        .addColor(modelObjData, 'ambientColor')
        .name(t('studio_model_preview.debug.ambient_color'))
        .onChange(onLightChange)

      lighting
        .add(modelObjData, 'directionalIntensity', 0, 2)
        .name(t('studio_model_preview.debug.directional_intensity'))
        .onChange(onLightChange)

      lighting
        .addColor(modelObjData, 'directionalColor')
        .name(t('studio_model_preview.debug.directional_color'))
        .onChange(onLightChange)

      const animations = gui.addFolder(t('studio_model_preview.debug.animation'))
      animations
        .add(modelObjData, 'playbackSpeed', 0, 1)
        .name(t('studio_model_preview.debug.playback_speed'))
        .onChange(speed => (
          scene.animations.forEach(clip => mixer.clipAction(clip).setEffectiveTimeScale(speed))
        ))

      animations
        .add(modelObjData, 'playAll')
        .name(t('studio_model_preview.debug.play_all'))

      modelObjData.animations = {}
      debugAnimationControllers.current = []
      scene.animations.forEach((clip, idx) => {
        const action = mixer.clipAction(clip)
        const animName = clip.name || `Animation_${idx}`
        modelObjData.animations[clip.uuid] = false  // by default, not playing

        const configureAnimation = (play: boolean) => {
          if (play) {
            action.time = animationTime.current
            action.play()
            if (cancelAnimation === null) {
              // only start the animation loop if it's not already running
              animateFrames()
            }
          } else {
            action.stop()
          }
        }

        const item = animations
          .add(modelObjData.animations, clip.uuid)
          .name(`${idx + 1}. ${animName}`)
          .onChange(enabled => configureAnimation(enabled))
        debugAnimationControllers.current.push(item)

        if (idx === 0) {
          // play the first animation by default
          item.setValue(true)
        }
      })
    }

    return () => {
      if (debugMainGui.current) {
        debugMainGui.current.destroy()
        debugMainGui.current = null
      }
    }
  }, [canvas, scene, visible])
}

export {
  useGltfDebugGui,
}

export type {
  DebugLightConfig,
}
