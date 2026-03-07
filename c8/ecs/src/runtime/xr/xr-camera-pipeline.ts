import type {World} from '../world'
import type {Camera, Scene, WebGLRenderer} from '../three-types'
import type {Eid} from '../../shared/schema'
import type {Events} from '../events'
import type {Quat, Vec3} from '../math/math'
import {vec3, quat} from '../math/math'
import {Position, Quaternion} from '../components'
import type {EcsRenderOverride} from './xr-ecs-types'
import type {FaceEffectCameraSchema, WorldEffectCameraSchema} from './xr-camera-pipeline-types'
import {CameraEvents} from '../camera-events'
import {resolveXr8, ensureXr8Loaded} from './xr-helpers'

declare const XR8: any

// TODO(alancastillo): this is just a temporary fix, delete this when HMD is fully supported
const isDeviceHeadset = async (): Promise<boolean> => {
  const agent = navigator.userAgent.toLocaleLowerCase()
  if (agent.includes('oculus') || agent.includes('quest') || agent.includes('headset')) {
    return true
  }
  return false
}

const ensureCameraFeedCanvasExist = (
  renderer: WebGLRenderer,
  elementId: string = 'camerafeed'
): HTMLCanvasElement => {
  if (renderer && renderer.domElement) {
    return renderer.domElement
  }

  // Make the canvas
  const newCanvas = document.createElement('canvas')
  newCanvas.id = elementId
  document.body.insertAdjacentElement('beforeend', newCanvas)
  return newCanvas
}

// Options for defining which portions of the face have mesh triangles returned.
const FaceMeshGeometry = {
  FACE: 'face',
  MOUTH: 'mouth',
  EYES: 'eyes',
  IRIS: 'iris',
} as const

interface CameraPipeline {
  start(): void
  stop(): void
}

interface CameraApi {
  getCameraTransform(): {t: Vec3, r: Quat, s: Vec3}
}

interface WorldState {
  camera: Camera
  cameraApi: CameraApi
  scene: Scene
  renderer: WebGLRenderer
  events: Events
  eid: Eid
  ecsRenderOverride: EcsRenderOverride
}

// We will initialize XR8 pipeline lazily on the first world/face/hand pipeline start
let isPipelineInitialized_ = false
const ensurePipelineInitialized = ({
  camera,
  cameraApi,
  scene,
  renderer,
  events,
  eid,
  ecsRenderOverride,
}: WorldState) => {
  if (isPipelineInitialized_) {
    //  Check if camera or scene has changed and reinitialize
    XR8.CloudStudioThreejs.updateSceneConfig({camera, cameraApi, scene, renderer, eid, events})
    return
  }

  XR8.addCameraPipelineModules([
    // Draws the camera feed.
    XR8.GlTextureRenderer.pipelineModule(),
    // Sync the camera with reality
    XR8.CloudStudioThreejs.pipelineModule({
      camera,
      cameraApi,
      scene,
      renderer,
      eid,
      events,
      ecsRenderOverride,
    }),
  ])
  isPipelineInitialized_ = true
}

const Z_FLIP = quat.yDegrees(180)
const buildCameraApi = (world: World, cameraEid: Eid): CameraApi => {
  const t = vec3.zero()
  const r = quat.zero()
  const s = vec3.one()
  const targetTrs = {t, r, s}
  return {
    getCameraTransform: () => {
      if (!Position.has(world, cameraEid) || !Quaternion.has(world, cameraEid)) {
        return targetTrs
      }
      t.setFrom(Position.get(world, cameraEid))
      r.setFrom(Quaternion.get(world, cameraEid))
      // Convert rotation from ecs into XR8's coordinate system
      r.setTimes(Z_FLIP)
      return targetTrs
    },
  }
}

const constructWorldCameraPipeline = async (
  world: World,
  eid: Eid,
  config: Partial<WorldEffectCameraSchema>,
  ecsRenderOverride: EcsRenderOverride
): Promise<CameraPipeline> => {
  // Since XR8 is a singleton, we cannot create multiple instances of the camera pipeline
  // We instead will store the config and construct the camera pipeline on start.
  const config_ = {...config}
  let xrModuleName_: string | null = null
  const eid_ = eid
  const isHeadset = await isDeviceHeadset()

  resolveXr8()
  await ensureXr8Loaded
  // Load slam chunk if it is not already loaded.
  // Check if loadChunk is defined as it may not be available in engine depending what gets released
  // first.
  if (XR8.loadChunk) {
    await XR8.loadChunk('slam')
  }

  return {
    start: () => {
      const xrModule = XR8.XrController.pipelineModule()
      xrModuleName_ = xrModule.name

      // We assume that the user has already called stop() by another camera pipeline
      // or the XR8 instance is clean
      ensurePipelineInitialized({
        // NOTE(dat): This is a hack. We should be using the sub-camera that eid is pointing to.
        camera: world.three.activeCamera,
        cameraApi: buildCameraApi(world, eid),
        scene: world.three.scene,
        renderer: world.three.renderer,
        events: world.events,
        eid: eid_,
        ecsRenderOverride,
      })
      XR8.addCameraPipelineModule(xrModule)
      XR8.XrController.configure({
        disableWorldTracking: config_.disableWorldTracking,
        enableLighting: config_.enableLighting,
        enableWorldPoints: config_.enableWorldPoints,
        leftHandedAxes: config_.leftHandedAxes,
        scale: config_.enableVps ? 'responsive' : config_.scale,
      })
      XR8.run({
        canvas: ensureCameraFeedCanvasExist(world.three.renderer),
        verbose: false,
        cameraConfig: config_.direction,
        allowedDevices: config_.allowedDevices,
        ownRunLoop: isHeadset,
        sessionConfiguration: {
          disableDesktopCameraControls: true,
          defaultEnvironment: {
            disabled: true,
          },
        },
      })
    },
    stop: () => {
      world.events.dispatch(eid, CameraEvents.XR_CAMERA_STOP, {})
      XR8.stop()
      if (xrModuleName_ !== null) {
        XR8.removeCameraPipelineModule(xrModuleName_)
        xrModuleName_ = null
      }
    },
  }
}

const constructFaceCameraPipeline = async (
  world: World,
  eid: Eid,
  config: Partial<FaceEffectCameraSchema>,
  ecsRenderOverride: EcsRenderOverride
): Promise<CameraPipeline> => {
  const config_ = {...config}
  let xrModuleName_: string | null = null
  const eid_ = eid

  resolveXr8()
  await ensureXr8Loaded
  // Load face chunk if it is not already loaded.
  // CloudStudioThreejs currently needs XrController to work.
  if (XR8.loadChunk) {
    await XR8.loadChunk('slam')
    await XR8.loadChunk('face')
  }

  return {
    start: () => {
      const xrModule = XR8.FaceController.pipelineModule()
      xrModuleName_ = xrModule.name

      // Fill out mesh geometry array (temporary)
      const meshGeometry = []
      if (config_.meshGeometryFace) {
        meshGeometry.push(FaceMeshGeometry.FACE)
      }
      if (config_.meshGeometryEyes) {
        meshGeometry.push(FaceMeshGeometry.EYES)
      }
      if (config_.meshGeometryMouth) {
        meshGeometry.push(FaceMeshGeometry.MOUTH)
      }
      if (config_.meshGeometryIris) {
        meshGeometry.push(FaceMeshGeometry.IRIS)
      }

      // Assume the user has already called stop() by another camera pipeline
      // or XR8 instance is clean
      ensurePipelineInitialized({
        camera: world.three.activeCamera,
        cameraApi: buildCameraApi(world, eid),
        scene: world.three.scene,
        renderer: world.three.renderer,
        eid: eid_,
        events: world.events,
        ecsRenderOverride,
      })
      XR8.addCameraPipelineModule(xrModule)
      XR8.FaceController.configure({
        nearClip: config_.nearClip,
        farClip: config_.farClip,
        meshGeometry,
        uvType: config_.uvType,
        maxDetections: config_.maxDetections,
        enableEars: config_.enableEars,
        coordinates: {mirroredDisplay: config_.mirroredDisplay},
      })
      XR8.run({
        canvas: ensureCameraFeedCanvasExist(world.three.renderer),
        verbose: false,
        cameraConfig: {direction: config_.direction},
        allowedDevices: config.allowedDevices,
        ownRunLoop: false,
      })
    },
    stop: () => {
      world.events.dispatch(eid, CameraEvents.XR_CAMERA_STOP, {})
      XR8.stop()
      if (xrModuleName_ !== null) {
        XR8.removeCameraPipelineModule(xrModuleName_)
        xrModuleName_ = null
      }
    },
  }
}

const mediaRecorderModulesManager = () => {
  let xrMediaRecorderName_: string | null = null
  let xrCanvasScreenshotName_: string | null = null
  let isRemoved_ = false
  const addMediaRecorder = async (): Promise<void> => {
    isRemoved_ = false
    resolveXr8()
    await ensureXr8Loaded
    if (isRemoved_) {
      return
    }
    if (!xrMediaRecorderName_) {
      const xrMediaRecorder = XR8.MediaRecorder.pipelineModule()
      xrMediaRecorderName_ = xrMediaRecorder.name
      XR8.addCameraPipelineModule(xrMediaRecorder)
    }
  }

  const addCanvasScreenshot = async (): Promise<void> => {
    isRemoved_ = false
    resolveXr8()
    await ensureXr8Loaded
    if (isRemoved_) {
      return
    }
    if (!xrCanvasScreenshotName_) {
      const xrCanvasScreenshot = XR8.CanvasScreenshot.pipelineModule()
      xrCanvasScreenshotName_ = xrCanvasScreenshot.name
      XR8.addCameraPipelineModule(xrCanvasScreenshot)
    }
  }

  const remove = () => {
    isRemoved_ = true
    if (xrMediaRecorderName_) {
      XR8.removeCameraPipelineModule(xrMediaRecorderName_)
      xrMediaRecorderName_ = null
    }
    if (xrCanvasScreenshotName_) {
      XR8.removeCameraPipelineModule(xrCanvasScreenshotName_)
      xrCanvasScreenshotName_ = null
    }
  }

  return {
    addCanvasScreenshot,
    addMediaRecorder,
    remove,
  }
}

export {
  constructWorldCameraPipeline,
  constructFaceCameraPipeline,
  mediaRecorderModulesManager,
}

export type {
  CameraPipeline,
  WorldEffectCameraSchema,
  FaceEffectCameraSchema,
}
