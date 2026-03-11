import React from 'react'

import {
  TARGET_ERROR, TARGET_INDEX_RATIO, TRIANGLE_VERTEX_COUNT, VERTEX_POSITION_STRIDE,
  VERTEX_ROUNDING_FACTOR,
} from '@ecs/shared/mesh-optimizer-constants'
import type {GltfModel as Gltf, HiderMaterial, Shadow, VideoMaterial} from '@ecs/shared/scene-graph'
import {MeshoptSimplifier} from 'meshoptimizer'
import {
  LoopOnce,
  LoopPingPong,
  LoopRepeat,
  type Group,
  type CanvasTexture,
  Mesh,
  AnimationMixer,
  AnimationClip,
  AnimationAction,
  MeshBasicMaterial,
} from 'three'

import {mergeVertices} from 'three/examples/jsm/utils/BufferGeometryUtils'

import {useFrame, useThree} from '@react-three/fiber'

import type {DeepReadonly} from 'ts-essentials'

import {DEFAULT_TIME_SCALE, PLAY_ALL_ANIMATIONS_KEY} from '@ecs/shared/gltf-constants'
import {MATERIAL_DEFAULTS} from '@ecs/shared/material-constants'

import {useTranslation} from 'react-i18next'

import {useStudioStateContext} from './studio-state-context'

import {useGltfWithDraco} from './hooks/gltf'
import {useResourceUrl} from './hooks/resource-url'
import * as SkeletonUtils from './skeleton-utils'
import {GltfLoadBoundary} from './gltf-load-boundary'

import {HiderMaterial as MeshHiderMaterial} from './shader/mesh-hider-material'
import {enableOutlineRecursive, removeOutlineRecursive} from './selected-outline'
import {useVideoThumbnail} from './hooks/use-video-thumbnail'

const overrideWithHider = (scene: Group) => {
  const hiderMat = new MeshHiderMaterial({transparent: true})
  scene.traverse((object) => {
    if (object instanceof Mesh) {
      object.material = hiderMat
      object.renderOrder = -1
    }
  })
}

const overrideWithVideo = (
  scene: Group, video: VideoMaterial, videoThumbnail: CanvasTexture | undefined
) => {
  const opacity = video?.opacity ?? MATERIAL_DEFAULTS.opacity
  const thumbnailMat = new MeshBasicMaterial({
    opacity,
    color: video.color,
    map: videoThumbnail,
    transparent: opacity < 1,
  })
  scene.traverse((object) => {
    if (object instanceof Mesh) {
      object.material = thumbnailMat
    }
  })
}

interface IGltfModelInner {
  url: string
  shadow: DeepReadonly<Shadow> | undefined
  setVertices?: (vertices: number[]) => void
  model: DeepReadonly<Gltf>
  materialOverride?: HiderMaterial | VideoMaterial
  outlined?: boolean
}

const GltfModelInner: React.FC<IGltfModelInner> = ({
  url, shadow, setVertices, model, materialOverride, outlined,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const {scene: cachedScene, animations} = useGltfWithDraco(url)
  const stateCtx = useStudioStateContext()
  const {state: {isPreviewPaused}} = stateCtx
  const mixer = React.useRef<AnimationMixer | null>(null)

  const videoSrc = materialOverride?.type === 'video' && materialOverride?.textureSrc
  const videoThumbnail = useVideoThumbnail(videoSrc)

  React.useEffect(() => {
    if (!cachedScene || !setVertices) {
      return
    }
    setVertices([])
    const vertexPositions: number[] = []
    try {
      cachedScene.traverse((object) => {
        if (!(object as Mesh).isMesh) {
          return
        }
        const {geometry} = object as Mesh

        if (!geometry) {
          return
        }

        const deduplicatedGeometry = mergeVertices(geometry)

        const positions = deduplicatedGeometry.attributes.position.array as Float32Array
        const indices = new Uint32Array(deduplicatedGeometry.index.array)

        const targetIndexCount = Math.floor(
          indices.length * (TARGET_INDEX_RATIO / TRIANGLE_VERTEX_COUNT)
        ) * TRIANGLE_VERTEX_COUNT

        const [simplifiedIndices] = MeshoptSimplifier.simplify(
          indices, positions, VERTEX_POSITION_STRIDE, targetIndexCount, TARGET_ERROR
        )

        const simplifiedVertices = new Float32Array(
          simplifiedIndices.length * VERTEX_POSITION_STRIDE
        )

        for (let i = 0; i < simplifiedIndices.length; i++) {
          const index = simplifiedIndices[i] * VERTEX_POSITION_STRIDE
          for (let j = 0; j < VERTEX_POSITION_STRIDE; j++) {
            simplifiedVertices[i * VERTEX_POSITION_STRIDE + j] =
                Math.round(positions[index + j] * VERTEX_ROUNDING_FACTOR) / VERTEX_ROUNDING_FACTOR
          }
        }

        vertexPositions.push(...simplifiedVertices)
      })
      if (vertexPositions.length > 0) {
        setVertices(vertexPositions)
      }
    } catch (error) {
      stateCtx.update({errorMsg: t('collider_configurator.error.auto_collider')})
    }
  }, [cachedScene, setVertices])

  // NOTE(christoph): We need to clone the scene to avoid the cached original being
  // inserted into the scene multiple times
  const scene: Group = React.useMemo(() => {
    if (cachedScene) {
      const newScene = SkeletonUtils.clone(cachedScene)
      if (materialOverride?.type === 'hider') {
        overrideWithHider(newScene)
      } else if (materialOverride?.type === 'video') {
        overrideWithVideo(newScene, materialOverride, videoThumbnail)
      }
      return newScene
    }
    return null
  }, [cachedScene, materialOverride, videoThumbnail])

  React.useEffect(() => {
    scene?.traverse((object) => {
      if (object.isObject3D) {
        object.castShadow = !!shadow?.castShadow
        object.receiveShadow = !!shadow?.receiveShadow
      }
    })
  }, [scene, shadow])

  React.useEffect(() => {
    if (scene && outlined) {
      enableOutlineRecursive(scene)

      return () => {
        removeOutlineRecursive(scene)
      }
    }

    return undefined
  }, [scene, outlined])

  const playAction = (
    action: AnimationAction, loop: boolean, repetitions: number, reverse: boolean,
    paused: boolean, timeScale: number | undefined
  ) => {
    if (loop) {
      const animeReps = repetitions > 0 ? repetitions : Infinity
      if (reverse) {
        action.setLoop(LoopPingPong, animeReps * 2)
        action.clampWhenFinished = false
      } else {
        action.setLoop(LoopRepeat, animeReps)
        action.clampWhenFinished = true
      }
    } else {
      action.setLoop(LoopOnce, 1)
      action.clampWhenFinished = true
    }
    action.timeScale = timeScale ?? DEFAULT_TIME_SCALE
    action.paused = paused

    if (!action.isRunning()) {
      action.play()
    }
  }

  React.useEffect(() => {
    if (!outlined) {
      return undefined
    }

    // note: Gltf animation follows the same pattern as the one in built-in-systems
    if (animations.length === 0 || !model.animationClip) {
      return undefined
    }

    mixer.current = new AnimationMixer(scene)
    const myMiser = mixer.current

    const cancelAnimations = () => {
      myMiser.setTime(0)
      myMiser.stopAllAction()
    }

    if (model.animationClip === PLAY_ALL_ANIMATIONS_KEY) {
      animations.forEach((clip) => {
        const action = myMiser.clipAction(clip)
        playAction(
          action, model.loop, model.repetitions, model.reverse, model.paused, model.timeScale
        )
      })
      return cancelAnimations
    }

    // Run a single animation
    const clip = AnimationClip.findByName(animations, model.animationClip)
    const action = myMiser.clipAction(clip)
    playAction(
      action, model.loop, model.repetitions, model.reverse, model.paused, model.timeScale
    )
    return cancelAnimations
  }, [model, outlined, animations])

  const {invalidate} = useThree()
  useFrame((_, delta) => {
    // Continue animating if this object is outlined (selected).
    // mixer.current is only set when outlined is true.
    if (mixer.current && !isPreviewPaused) {
      mixer.current.update(delta)
      invalidate()
    }
  })

  return scene && <primitive object={scene} />
}

interface IGltfModel {
  model: Gltf
  shadow: Shadow | undefined
  setVertices?: (vertices: number[]) => void
  materialOverride?: HiderMaterial | VideoMaterial
  outlined?: boolean
}

const GltfModel: React.FC<IGltfModel> = ({model, ...rest}) => {
  const url = useResourceUrl(model.src)

  if (!url) {
    return null
  }

  return (
    <GltfLoadBoundary key={url}>
      <GltfModelInner url={url} {...rest} model={model} />
    </GltfLoadBoundary>
  )
}

export {
  GltfModel,
}
