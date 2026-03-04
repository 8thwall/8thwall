import type {World} from '../world'
import {Camera} from '../components'
import type {Eid} from '../../shared/schema'
import {
  constructFaceCameraPipeline,
  constructWorldCameraPipeline,
  mediaRecorderModulesManager,
} from './xr-camera-pipeline'
import type {
  CameraPipeline,
  FaceEffectCameraSchema,
  WorldEffectCameraSchema,
} from './xr-camera-pipeline'
import {getAllowedXrDevices} from '../../shared/xr-config'
import {quat} from '../math/quat'
import {CameraEvents} from '../camera-events'
import type {EcsRenderOverride} from './xr-ecs-types'
import type {XrManager, FrameInfo, ProgressInfo} from './xr-manager-types'
import {events as EventIds} from '../event-ids'

declare const XR8: any

const CAMERA_TRANSFORM_UPDATE = 'cameraupdate'
const Z_FLIP = quat.yDegrees(180)

// XrManager manages the running CameraPipeline for XR. Only one camera pipeline can run at a time
// so XrManager can switch between the active one. If users
// XrManager should not have async methods since we use it in the ECS system.
const createXrManager = (world: World, events: any): XrManager => {
  let activeHandle_: number | null = null
  const handleToCameraPipeline_: Record<number, Promise<CameraPipeline>> = {}
  let currentCameraEid_: Eid = 0n

  let ecsRenderOverride_: EcsRenderOverride

  let pipelineIsRunning_: boolean = false

  const mediaRecorderModulesManager_ = mediaRecorderModulesManager()

  const setEcsRenderOverride = (renderOverride: EcsRenderOverride) => {
    ecsRenderOverride_ = renderOverride
  }

  const getRandomInt = () => Math.floor(Math.random() * 1000000)
  const getNewHandle = () => {
    let handle = getRandomInt()
    while (handleToCameraPipeline_[handle]) {
      handle = getRandomInt()
    }
    return handle
  }

  const createWorldEffect = (
    config: Partial<WorldEffectCameraSchema>, eid: Eid
  ): number => {
    const handle = getNewHandle()
    handleToCameraPipeline_[handle] =
      constructWorldCameraPipeline(world, eid, config, ecsRenderOverride_)
    return handle
  }

  const startCameraPipeline = (handle: number): void => {
    if (activeHandle_ !== null && activeHandle_ === handle) {
      // already running this handle
      return
    }

    // stop the current camera pipeline and start the new one
    if (activeHandle_ !== null) {
      handleToCameraPipeline_[activeHandle_].then(pipeline => pipeline.stop())
      activeHandle_ = null
    }

    if (!handleToCameraPipeline_[handle]) {
      // eslint-disable-next-line no-console
      console.log('[ecs.xr] Handle not found. Did you call create{World,Face}Effect?')
      return
    }
    handleToCameraPipeline_[handle].then((pipeline) => {
      pipeline.start()
      activeHandle_ = handle
      pipelineIsRunning_ = true
    })
  }

  const stopCameraPipeline = (handle: number): void => {
    if (handleToCameraPipeline_[handle] === null) {
      // We have already cleaned up this pipeline
      return
    }

    // Stop this pipeline
    handleToCameraPipeline_[handle].then((pipeline) => {
      // NOTE(dat): Consider killing pipelines to save memory. Perhaps we need deleteWorldEffect()
      pipeline.stop()
      pipelineIsRunning_ = false
    })

    if (activeHandle_ === handle) {
      activeHandle_ = null
    }
  }

  const createFaceEffect = (
    config: Partial<FaceEffectCameraSchema>, eid: Eid
  ): number => {
    const handle = getNewHandle()
    handleToCameraPipeline_[handle] =
      constructFaceCameraPipeline(world, eid, config, ecsRenderOverride_)
    currentCameraEid_ = eid
    return handle
  }

  const tempQuat = quat.zero()
  const updateCameraTransform = (e: any) => {
    const {position, rotation, cameraFov} = e.data
    if (cameraFov) {
      Camera.set(world, currentCameraEid_, {fov: cameraFov})
    }
    if (position) {
      world.setPosition(currentCameraEid_, position.x, position.y, position.z)
    }
    if (rotation) {
      tempQuat.setXyzw(rotation.x, rotation.y, rotation.z, rotation.w)

      // Convert rotation from the engine back to ecs world
      tempQuat.setTimes(Z_FLIP)

      world.setQuaternion(currentCameraEid_, tempQuat.x, tempQuat.y, tempQuat.z, tempQuat.w)
    }
  }

  const startMediaRecorder = async () => {
    await mediaRecorderModulesManager_.addMediaRecorder()
    XR8.MediaRecorder.recordVideo({
      onStart: () => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_VIDEO_STARTED)
      },
      onStop: () => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_VIDEO_STOPPED)
      },
      onError: (data: Error) => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_VIDEO_ERROR, data)
      },
      onVideoReady: (data: {videoBlob: Blob}) => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_VIDEO_READY, data)
      },
      // Progress on preparing video download
      onFinalizeProgress: (data: ProgressInfo) => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_FINALIZE_PROGRESS, data)
      },
      // Video preview type is webm
      onPreviewReady: (data: {videoBlob: Blob}) => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_PREVIEW_READY, data)
      },
      onProcessFrame: (data: FrameInfo) => {
        world.events.dispatch(world.events.globalId, EventIds.RECORDER_PROCESS_FRAME, data)
      },
    })
  }

  const stopMediaRecorder = () => {
    XR8.MediaRecorder.stopRecording()
  }

  const takeScreenshot = async (): Promise<Blob> => {
    await mediaRecorderModulesManager_.addCanvasScreenshot()
    const data: string = await XR8.CanvasScreenshot.takeScreenshot()
    const bytes = atob(data)
    const array = new Uint8Array(Array.from(bytes, char => char.charCodeAt(0)))
    const blob = new Blob([array], {type: 'image/jpeg'})
    world.events.dispatch(world.events.globalId, EventIds.RECORDER_SCREENSHOT_READY, blob)
    return blob
  }

  const handleCameraChange = async (e: any) => {
    const {camera} = e.data
    let handle
    if (currentCameraEid_) {
      events.removeListener(currentCameraEid_, CAMERA_TRANSFORM_UPDATE, updateCameraTransform)
    }
    currentCameraEid_ = world.camera?.getActiveEid() ?? 0n
    if (currentCameraEid_) {
      events.addListener(currentCameraEid_, CAMERA_TRANSFORM_UPDATE, updateCameraTransform)
    }

    if (camera.userData?.xr?.xrCameraType === 'world') {
      handle = createWorldEffect({
        disableWorldTracking: camera.userData.xr.disableWorldTracking,
        enableLighting: camera.userData.xr.enableLighting,
        enableWorldPoints: camera.userData.xr.enableWorldPoints,
        leftHandedAxes: camera.userData.xr.leftHandedAxes,
        mirroredDisplay: camera.userData.xr.mirroredDisplay,
        scale: camera.userData.xr.scale,
        enableVps: camera.userData.xr.enableVps,
        direction: camera.userData.xr.direction,
        allowedDevices: getAllowedXrDevices({
          camera: camera.userData.xr.xrCameraType,
          phone: camera.userData.xr.phone,
          desktop: camera.userData.xr.desktop,
          headset: camera.userData.xr.headset,
        }),
      }, world.camera.getActiveEid())
    } else if (camera.userData?.xr?.xrCameraType === 'face') {
      handle = createFaceEffect({
        nearClip: camera.userData.xr.nearClip,
        farClip: camera.userData.xr.farClip,
        direction: camera.userData.xr.direction,
        meshGeometryFace: camera.userData.xr.meshGeometryFace,
        meshGeometryEyes: camera.userData.xr.meshGeometryEyes,
        meshGeometryIris: camera.userData.xr.meshGeometryIris,
        meshGeometryMouth: camera.userData.xr.meshGeometryMouth,
        uvType: camera.userData.xr.uvType,
        maxDetections: camera.userData.xr.maxDetections,
        enableEars: camera.userData.xr.enableEars,
        mirroredDisplay: camera.userData.xr.mirroredDisplay,
        allowedDevices: getAllowedXrDevices({
          camera: camera.userData.xr.xrCameraType,
          phone: camera.userData.xr.phone,
          desktop: camera.userData.xr.desktop,
          headset: camera.userData.xr.headset,
        }),
      }, world.camera.getActiveEid())
    }
    if (handle) {
      startCameraPipeline(handle)
    } else if (activeHandle_ !== null) {
      // Stop the active handle if we are not in XR.
      handleToCameraPipeline_[activeHandle_].then(pipeline => pipeline.stop())
      activeHandle_ = null
    }
  }

  const drawPausedBackground = () => {
    if (pipelineIsRunning_) {
      XR8?.GlTextureRenderer?.pipelineModule()?.onRender()
    }
  }

  const tick = () => {
    if (pipelineIsRunning_) {
      XR8.runPreRender(Date.now())
      XR8.runRender()
    }
  }

  const tock = () => {
    if (pipelineIsRunning_) {
      XR8.runPostRender()
    }
  }

  events.addListener(events.globalId, CameraEvents.ACTIVE_CAMERA_CHANGE, handleCameraChange)
  events.addListener(events.globalId, CameraEvents.XR_CAMERA_EDIT, handleCameraChange)

  return {
    createWorldEffect,
    createFaceEffect,
    startCameraPipeline,
    stopCameraPipeline,
    startMediaRecorder,
    stopMediaRecorder,
    takeScreenshot,
    drawPausedBackground: () => drawPausedBackground(),
    setEcsRenderOverride,
    attach: () => {},
    detach: () => {
      Object.values(handleToCameraPipeline_).forEach((handle) => {
        handle.then(p => p.stop())
      })
      mediaRecorderModulesManager_.remove()
    },
    tick,
    tock,
  }
}

export type {
  XrManager,
}

export {
  createXrManager,
}
