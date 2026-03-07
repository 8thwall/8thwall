import THREE from '../three'
import type * as THREE_TYPES from '../three-types'
import {assets} from '../assets'
import {getGltfLoader} from '../loaders'
import type {World} from '../world'
import {makeSystemHelper} from './system-helper'
import {GltfModel, Shadow} from '../components'
import type {Eid, ReadData} from '../../shared/schema'
import {events} from '../event-ids'
import {getGltfMeshData} from '../get-gltf-vertices'
import {Collider, physics} from '../physics'
import {PLAY_ALL_ANIMATIONS_KEY} from '../../shared/gltf-constants'
import type {Shadow as ShadowType} from '../../shared/scene-graph'
import {addChild, notifyChanged} from '../matrix-refresh'
import type {SchemaOf} from '../world-attribute'
import {disposeObject, markAsDisposable} from '../dispose'
import {createVideoTexture, deleteVideoTexture, createBlobVideoTexture} from '../video-texture'
import {getVideoManager} from '../video-manager'
import {THREE_TEXTURE_KEYS} from '../texture-constants'
import type {ThreeTextureKey} from '../texture-types'
import type {VideoUserData} from '../video-types'

const playAction = (
  action: THREE_TYPES.AnimationAction,
  loop: boolean, repetitions: number, reverse: boolean, paused: boolean,
  timeScale: number
) => {
  if (loop) {
    const animReps = repetitions > 0 ? repetitions : Infinity
    if (reverse) {
      // Double the internal reps so that full ping-pong cycle is completed in one repetition
      action.setLoop(THREE.LoopPingPong, animReps * 2)
      action.clampWhenFinished = false
    } else {
      action.setLoop(THREE.LoopRepeat, animReps)
      action.clampWhenFinished = true
    }
  } else {
    action.setLoop(THREE.LoopOnce, 1)
    action.clampWhenFinished = true
  }

  action.setEffectiveTimeScale(timeScale)
  action.paused = paused

  if (!action.isRunning()) {
    action.play()
  }
}

const maybeLoadVideoTextures = async (
  node: THREE_TYPES.Mesh<any, any, any>, gltf: THREE_TYPES.GLTF
) => {
  const videoTextures: THREE_TYPES.VideoTexture[] = []

  await Promise.all(THREE_TEXTURE_KEYS.map(async (textureKey: ThreeTextureKey) => {
    const needsSrgb = textureKey === 'map' || textureKey === 'emissiveMap'
    const videoData: VideoUserData = node.material.userData.videos?.[textureKey]
    if (videoData) {
      let videoTexture: THREE_TYPES.VideoTexture
      const {accessor: accessorIndex, mimeType} = videoData
      if (typeof accessorIndex === 'number' && mimeType) {
        const accessor = await gltf.parser.loadAccessor(accessorIndex)
        videoTexture = createBlobVideoTexture(new Blob([accessor.array], {type: mimeType}))
      } else {
        // Backwards compatibility using legacy localUrl data
        videoTexture = createVideoTexture(videoData.localUrl)
      }
      videoTexture.userData.src = videoData.url
      videoTexture.userData.textureKey = textureKey
      if (needsSrgb) {
        videoTexture.colorSpace = THREE.SRGBColorSpace
      }
      node.material[textureKey] = videoTexture
      videoTextures.push(videoTexture)
    }
  }))

  return videoTextures
}

type TextureId = string

const makeGltfSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(GltfModel)
  const mixers = new Map<Eid, THREE_TYPES.AnimationMixer>()
  const videos = new Map<Eid, TextureId[]>()
  const customShapeMap = new Map<string, {shapeId: number, refCount: number}>()
  const eidToCustomShapeKey = new Map<Eid, string>()
  const pendingCustomShapeDeletions = new Set<string>()

  const cleanupCustomShapeCache = () => {
    if (pendingCustomShapeDeletions.size === 0) {
      return
    }

    pendingCustomShapeDeletions.forEach((shapeKey) => {
      const shapeEntry = customShapeMap.get(shapeKey)
      if (shapeEntry && shapeEntry.refCount <= 0) {
        physics.unregisterConvexShape(world, shapeEntry.shapeId)
        customShapeMap.delete(shapeKey)
      }
    })
    pendingCustomShapeDeletions.clear()
  }

  const addToCustomShapeMap = (
    eid: Eid,
    url: string,
    gltfScene: THREE_TYPES.Group<THREE_TYPES.Object3DEventMap>,
    simplificationMode: string
  ) => {
    const shapeKey = `${url}:${simplificationMode}`
    const shapeEntry = customShapeMap.get(shapeKey)
    if (shapeEntry) {
      shapeEntry.refCount++
    } else {
      const meshData = getGltfMeshData(gltfScene)
      const shapeId = simplificationMode === 'convex'
        ? physics.registerConvexShape(world, meshData.vertices)
        : physics.registerCompoundShape(world, meshData.vertices, meshData.indices)
      customShapeMap.set(shapeKey, {shapeId, refCount: 1})
    }

    eidToCustomShapeKey.set(eid, shapeKey)
    return customShapeMap.get(shapeKey)!.shapeId
  }

  const removeFromCustomShapeMap = (eid: Eid) => {
    const shapeKey = eidToCustomShapeKey.get(eid)
    if (!shapeKey) {
      return
    }
    eidToCustomShapeKey.delete(eid)
    const shapeEntry = customShapeMap.get(shapeKey)
    if (shapeEntry) {
      shapeEntry.refCount--
      if (shapeEntry.refCount <= 0) {
        pendingCustomShapeDeletions.add(shapeKey)
      }
    }
  }

  const loadingPromises: Map<Eid, Promise<void>> = new Map()

  const clearModel = (eid: Eid) => {
    const textures = videos.get(eid)
    textures?.forEach((textureId) => {
      getVideoManager(world).maybeRemoveVideo(eid, textureId)
    })
    videos.delete(eid)
    mixers.delete(eid)
    loadingPromises.delete(eid)
    removeFromCustomShapeMap(eid)

    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    object.children.forEach((child) => {
      if (child.userData.gltf && child.userData.eid === eid) {
        disposeObject(child)
        object.remove(child)
      }
    })

    if (object.userData) {
      object.userData.model = null
      object.userData.gltf = null
    }
  }

  const animationFinishedEvent = (
    event: {action: THREE_TYPES.AnimationAction, direction: number} &
    THREE_TYPES.Event<'finished', THREE_TYPES.AnimationMixer>
  ) => {
    const {eid} = event.action.getRoot().userData
    world.events.dispatch(eid, events.GLTF_ANIMATION_FINISHED, {name: event.action.getClip().name})
  }

  const animationLoopEvent = (
    event: {action: THREE_TYPES.AnimationAction, loopDelta: number} &
    THREE_TYPES.Event<'loop', THREE_TYPES.AnimationMixer>
  ) => {
    const {eid} = event.action.getRoot().userData
    world.events.dispatch(eid, events.GLTF_ANIMATION_LOOP, {name: event.action.getClip().name})
  }

  const updateAnimation = (
    eid: Eid, gltf: THREE_TYPES.GLTF, model: ReadData<SchemaOf<typeof GltfModel>>,
    object: THREE_TYPES.Object3D
  ) => {
    // note(alan): if changed gltf-model.tsx needs to be updated as well
    const {
      animationClip, loop, paused, time, timeScale, reverse, repetitions,
      crossFadeDuration,
    } = model

    let mixer = mixers.get(eid)
    if (!animationClip) {
      if (mixer) {
        mixer.stopAllAction()
        mixers.delete(eid)
      }
      return
    }

    if (!mixer) {
      mixer = new THREE.AnimationMixer(gltf.scene)
      mixers.set(eid, mixer)
      mixer.addEventListener('finished', animationFinishedEvent)
      mixer.addEventListener('loop', animationLoopEvent)
    }

    if (animationClip === PLAY_ALL_ANIMATIONS_KEY) {
      if (object.userData.animationClip !== PLAY_ALL_ANIMATIONS_KEY) {
        mixer.setTime(0)
        mixer.stopAllAction()
      }

      gltf.animations.forEach((clip) => {
        const action = mixer.clipAction(clip)
        playAction(action, loop, repetitions, reverse, paused, timeScale)
      })

      if (object.userData.time !== time) {
        mixer.setTime(time)
        object.userData.time = time
      }
      return
    }

    const clip = THREE.AnimationClip.findByName(gltf.animations, animationClip)
    if (!clip) {
      return
    }

    const clipChanged = object.userData.animationClip !== clip
    const previousAction = mixer.existingAction(object.userData.animationClip)
    const action = mixer.clipAction(clip)
    const crossFade = crossFadeDuration && previousAction && previousAction !== action

    if (clipChanged) {
      action.reset()
      object.userData.animationClip = clip
    }

    if (clipChanged && !crossFade) {
      mixer.setTime(0)
      mixer.stopAllAction()
    }

    if (crossFade) {
      // 'warp' (transitioning timescale as part of crossfade) is generally the best option when
      // transitioning between two looped animations
      const warp = previousAction.isRunning() &&
        previousAction.loop !== THREE.LoopOnce && action.loop !== THREE.LoopOnce
      action.crossFadeFrom(previousAction, crossFadeDuration, warp)
    }

    playAction(action, loop, repetitions, reverse, paused, timeScale)

    // Changes to time within the mixer make no sense if we're starting a crossfade
    if (!crossFade && (clipChanged || object.userData.time !== time)) {
      mixer.setTime(time)
      object.userData.time = time
    }
  }

  const applyModel = (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const {url} = GltfModel.get(world, eid)

    if (!url) {
      return
    }

    const promise = new Promise<void>((resolve, reject) => {
      assets.load({url}).then(async (asset) => {
        getGltfLoader().parse(
          // NOTE(christoph): We load the main asset using the already loaded blob, but for
          // secondary relative assets like gltf -> png, we specify the remoteUrl to load from.
          await asset.data.arrayBuffer(),
          asset.remoteUrl?.replace(/\/[^/]*$/, '/') ?? '/',
          async (gltf) => {
            if (loadingPromises.get(eid) !== promise) {
              return resolve()
            }

            let shadowConfig: ShadowType
            if (Shadow.has(world, eid)) {
              shadowConfig = Shadow.get(world, eid)
            }

            const nodesWithVideos: THREE_TYPES.Mesh[] = []
            gltf.scene.traverse((node) => {
              if (shadowConfig && node.isObject3D) {
                node.castShadow = !!shadowConfig.castShadow
                node.receiveShadow = !!shadowConfig.receiveShadow
              }
              if (node instanceof THREE.Mesh) {
                node.geometry.computeBoundsTree()
                if (node.material?.userData.videos) {
                  nodesWithVideos.push(node)
                }
                markAsDisposable(node)
              }
            })

            const videoTexturesInMaterial = await Promise.all(
              nodesWithVideos.map(async node => maybeLoadVideoTextures(node, gltf))
            )
            const videoTexturesInGltf = videoTexturesInMaterial.flat()

            if (loadingPromises.get(eid) !== promise) {
              videoTexturesInGltf.forEach(texture => deleteVideoTexture(texture))
              return resolve()
            }

            if (videoTexturesInGltf.length) {
              getVideoManager(world).addVideo(eid, videoTexturesInGltf)
              videos.set(eid, videoTexturesInGltf.map(t => t.uuid))
            }

            const model = GltfModel.get(world, eid)
            object.userData.url = url
            object.userData.gltf = gltf
            gltf.scene.userData.gltf = true
            gltf.scene.userData.eid = eid
            object.userData.collider = model.collider
            addChild(object, gltf.scene)
            world.events.dispatch(eid, events.GLTF_MODEL_LOADED, {model: gltf.scene})

            updateAnimation(eid, gltf, model, object)
            // Animated mesh which has origin outside of viewport will get culled. It's possible
            // that threejs do this based on bounding box instead of origin.
            // Disable culling so the mesh appears.
            // NOTE(dat): There are several fixes for this
            //   * Reprocess the animations so that the root moves with the animation. (studio-time)
            //   * Figure out the object center based on animation. (runtime)
            if (gltf.animations.length > 0) {
              gltf.scene.traverse((node) => {
                if (!(node as THREE_TYPES.Mesh).isMesh) {
                  return
                }
                node.frustumCulled = false
              })
            }

            if (model.collider) {
              const collider = Collider.acquire(world, eid)
              const shapeId = addToCustomShapeMap(eid, url, gltf.scene, collider.simplificationMode)
              collider.shape = shapeId
              Collider.commit(world, eid)
            }
            return resolve()
          },
          reject
        )
      }).catch(reject)
    })

    loadingPromises.set(eid, promise)
  }

  const updateMixers = (deltaTime: number) => {
    const deltaTimeInSeconds = deltaTime / 1000
    mixers.forEach((mixer) => {
      mixer.update(deltaTimeInSeconds)
      notifyChanged(mixer.getRoot() as THREE_TYPES.Object3D)
    })
  }

  const handleChanged = (eid: Eid) => {
    const newModel = GltfModel.get(world, eid)
    const object = world.three.entityToObject.get(eid)
    const oldModelUrl = object?.userData.url
    const oldCollider = object?.userData.collider

    if (oldModelUrl && newModel && newModel.url === oldModelUrl &&
      newModel.collider === oldCollider
    ) {
      updateAnimation(eid, object.userData.gltf, newModel, object)
    } else {
      clearModel(eid)
      applyModel(eid)
    }
  }

  return () => {
    exit(world).forEach((eid) => {
      clearModel(eid)
    })
    enter(world).forEach(applyModel)
    changed(world).forEach(handleChanged)
    updateMixers(world.time.delta)
    cleanupCustomShapeCache()
  }
}

export {
  makeGltfSystem,
}
